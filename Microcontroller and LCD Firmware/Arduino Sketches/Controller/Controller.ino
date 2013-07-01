
#include <TinyGPS.h>
#include <SPI.h>
#include <SD.h>

#define SOFTWARE_VERSION "0.2"
#define COMMAND_DELIMITER '\n'
#define GPSUART Serial3
#define CELLUART Serial2
#define LCDUART Serial1
#define PCUART Serial
#define GPSBAUD 4800
#define LCDBAUD 115200
#define CELLBAUD 9600
#define PCBAUD 115200
#define PT_SWITCH_TO_READ_DELAY 1
#define PT_OFFSET 512
#define BATT_VOLTAGE_COEFF 5.0/1024.0
#define NUM_THERMISTOR_SAMPLES_TO_AVG 1000

#define STARTUP_BEEP 1
#define RUN_COMPLETE_BEEP 2
#define ERROR_BEEP 3
#define ASSAY_START_BEEP 4

//digital output control pins
const int heaterControlPin =  35;
const int lightControlPin =  34;
const int stat1LEDPin = 37;
const int stat2LEDPin = 36;
const int buzzerPin = 33;

const int sdCSPin = 8;
const int sdCDPin = 7;
const int sdWPPin = 6;
const int sdSCKPin = 13;
const int sdMISOPin = 12;
const int sdMOSIPin = 11;

//pd address pins
const int U2OutPin = 22;
const int U4OutPin = 23;
const int U5OutPin = 24;
const int A0OutPin = 25;
const int A1OutPin = 26;
const int A2OutPin = 27;
const int A3OutPin = 28;
const int A4OutPin = 29;
//analog input pins
const int batteryInPin = A3;
const int thermistorInPin = A1;
const int phototransistorInPin = A2;

//gps state
float gps_latitude, gps_longitude, gps_altitude;
int gps_year;
byte gps_month, gps_day, gps_hour, gps_minute, gps_second, gps_hundredths;
//TinyGPS gps;

//SD card
boolean sd_card_present = 0;

//heater state
float heater_coeff = 1.0383; //degree C / digital val
float heater_offset = -529.97; //deg C
float heater_tolerance = 0.1;
float heater_sp = 0;
float heater_temp = 0;
boolean heater_on = 0;

//lights state
int lights_level;

//uart state
static String uart_buffer;
static String lcd_buffer;

//TODO: all of these initializations should probably go in a reset() fxn
//pt state
int pt_wellNum = 0;
int pt_numSamplesToAverage = 100;

//controller state
float batt_voltage = 0.0;
int freerun_mode = 0;
int lcd_echo_mode = 0;
int poll_single_well_mode = 0;
int start_well = 1;
int stop_well = 96;
float low_battery_warning = 3.5;

String assay_name = "test_1";
int assay_numrunsteps = 6000;
int assay_numstabilizesteps = 10;
int assay_timestep = 2000;
float assay_heater_sp = 60;
int assay_lights_level = 1;
int assay_state = 0;
int assay_step = 1;
int assay_startwell = 1;
int assay_stopwell = 96;

/* Initializes all the peripherals and runs through a test sequence */
void setup() {
  setupLights();  
  setupHeater();
  setupPCInterface();
  setupLCD();
  setupGPS();
  setupCell();
  setupSDCard();
  setupPhototransistors();
  pinMode(stat1LEDPin, OUTPUT);    
  pinMode(stat2LEDPin, OUTPUT); 
  pinMode(buzzerPin, OUTPUT); 
  
  Serial.println("BioPOETS LAMP Assay System");
  beep(STARTUP_BEEP);
  testLights();
  digitalWrite(stat1LEDPin, HIGH);
  delay(200);
  digitalWrite(stat1LEDPin, LOW);
  delay(200);
  digitalWrite(stat2LEDPin, HIGH);
  delay(200);
  digitalWrite(stat2LEDPin, LOW);
  delay(200);

  //this sends out all the initial state variables to the PC
  getControllerState();
  getGPSState();
  getHeaterState();
  getLightsState();
  getPhototransistorsState();   
}

/* Main program loop */
void loop()
{
  //in the idle state, the sytem continuously queries all the components and streams data to the PC
  //when an assay is run, it is triggered through the UART/LCD.
  pollUART();  
  pollLCD();   
  if (assay_state>0)
    runAssay();
  else if (freerun_mode==1) { 
    pollAllPhototransistors();      
  } 
  else {
    //these polling functions run continuously, even when an assay isn't being run
    pollController();
    pollHeater();  
    pollGPS();      
  }
}

/* runs through the steps of an assay and transmits data to PC for recording */
void runAssay() {
  //set system state
  //state 0 = no assay running
  static unsigned long start_time;
  static unsigned long temp_time;  //todo: redo this so it's an interrupt or something...and have heater PID going at faster loop rate
  if (assay_state>1) { //these 3 numbers mark the beginning of each datapoint
    Serial.print("assay_time "); Serial.println(millis() - start_time);  
    Serial.print("assay_state "); Serial.println(assay_state);
    Serial.print("assay_step "); Serial.println(assay_step); //this should be the keyword that starts a new row...

    LCDUART.write(0xD7);
    LCDUART.print("04");
    LCDUART.print(millis() - start_time);
    LCDUART.write(0x00); 
    
    LCDUART.write(0xD7);
    LCDUART.print("05");
    switch (assay_state) {
      case 0:  LCDUART.print("IDLE"); break;
      case 1:  LCDUART.print("SETUP"); break; 
      case 2:  LCDUART.print("WARMING"); break; 
      case 3:  LCDUART.print("STABILIZE"); break; 
      case 4:  LCDUART.print("RUN"); break; 
      case 5:  LCDUART.print("SHUTDWN"); break;     
    }  
    LCDUART.write(0x00); 
    
    LCDUART.write(0xD7);
    LCDUART.print("06");
    LCDUART.print(assay_step);
    LCDUART.write(0x00); 
  
  }
  switch (assay_state) {
    case 1: //setup
      heater_sp = assay_heater_sp;
      start_well = assay_startwell;
      stop_well = assay_stopwell;
      assay_state = 2;
      assay_step = 1;        
      start_time = millis();
      Serial.print("assay_begin\n"); //signal that the assay is starting and do a dump of all the system state...this will go at top of file
      digitalWrite(stat1LEDPin,HIGH);
      beep(ASSAY_START_BEEP);
      getControllerState();
      getGPSState();
      getHeaterState();
      getLightsState();
      getPhototransistorsState();       
      break;  
    case 2: //warming up
      if (getTempOK()) {
        assay_state = 3;
        beep(ASSAY_START_BEEP);
      }
      break;    
    case 3: //stabilizing the amplifier
      temp_time = millis();
      setLights(assay_lights_level);
      pollAllPhototransistors();
      setLights(0);
      temp_time = assay_timestep - (millis() - temp_time);
      if (temp_time>0)
        delay(temp_time);

      if (assay_step==assay_numstabilizesteps) {
        assay_state = 4;
        beep(ASSAY_START_BEEP);
      }
      assay_step++;      
      break;
    case 4: //recording data
      temp_time = millis();
      setLights(assay_lights_level);
      pollAllPhototransistors();
      setLights(0);
      temp_time = assay_timestep - (millis() - temp_time);
      if (temp_time>0)
        delay(temp_time);
      if (assay_step==(assay_numrunsteps+assay_numstabilizesteps))
        assay_state = 5;
      assay_step++;  
      break;
    case 5: //shutdown
      setLights(0);
      heater_sp = 0;
      assay_state = 0;
      getControllerState();
      getGPSState();
      getHeaterState();
      getLightsState();
      getPhototransistorsState(); 
      digitalWrite(stat1LEDPin,LOW);
      beep(RUN_COMPLETE_BEEP);
      Serial.print("assay_end\n");      
      break;
  }   

  pollController();
  pollHeater();  
}

void pollController() {
  batt_voltage = analogRead(batteryInPin)*BATT_VOLTAGE_COEFF;  
  if (batt_voltage<low_battery_warning)
    digitalWrite(stat2LEDPin,HIGH);
  else
    digitalWrite(stat2LEDPin,LOW);
    
  Serial.print("batt_voltage "); Serial.println(batt_voltage);  
 
  LCDUART.write(0xD7);
  LCDUART.print("07");
  LCDUART.print(batt_voltage);
  LCDUART.write(0x00);    
}

void getControllerState() {
  pollController();
  Serial.print("firmware_ver "); Serial.println(SOFTWARE_VERSION);
  Serial.print("assay_name "); Serial.println(assay_name); 
  Serial.print("assay_numrunsteps "); Serial.println(assay_numrunsteps);
  Serial.print("assay_numstabilizesteps "); Serial.println(assay_numstabilizesteps);
  Serial.print("assay_timestep "); Serial.println(assay_timestep);
  Serial.print("assay_heater_sp "); Serial.println(assay_heater_sp);
  Serial.print("assay_lights_level "); Serial.println(assay_lights_level);
  Serial.print("assay_startwell "); Serial.println(assay_startwell);
  Serial.print("assay_stopwell "); Serial.println(assay_stopwell);
  Serial.print("assay_state "); Serial.println(assay_state);
  
  LCDUART.write(0xD7);
  LCDUART.print("02");
  LCDUART.print(assay_name);
  LCDUART.write(0x00);  
  
}

void beep(int beepType) {

 switch (beepType) {
  case 1: //startup beep
    digitalWrite(buzzerPin, HIGH);
    delay(50);
    digitalWrite(buzzerPin, LOW);
    delay(50);
    digitalWrite(buzzerPin, HIGH);
    delay(50);
    digitalWrite(buzzerPin, LOW);
    break;
  case 2: //assay complete beep
    digitalWrite(buzzerPin, HIGH);
    delay(1000);
    digitalWrite(buzzerPin, LOW);
    delay(1000);
    digitalWrite(buzzerPin, HIGH);
    delay(1000);
    digitalWrite(buzzerPin, LOW);
    delay(1000);
    digitalWrite(buzzerPin, HIGH);
    delay(1000);
    digitalWrite(buzzerPin, LOW);
    delay(1000);    
    break;
  case 3:
    digitalWrite(buzzerPin, HIGH);
    delay(20);
    digitalWrite(buzzerPin, LOW);
    delay(20);
    digitalWrite(buzzerPin, HIGH);
    delay(20);
    digitalWrite(buzzerPin, LOW);
    delay(20);
    digitalWrite(buzzerPin, HIGH);
    delay(20);
    digitalWrite(buzzerPin, LOW);
    delay(500); 
    digitalWrite(buzzerPin, HIGH);
    delay(20);
    digitalWrite(buzzerPin, LOW);
    delay(20);
    digitalWrite(buzzerPin, HIGH);
    delay(20);
    digitalWrite(buzzerPin, LOW);
    delay(20);
    digitalWrite(buzzerPin, HIGH);
    delay(20);
    digitalWrite(buzzerPin, LOW);
    delay(500); 
    digitalWrite(buzzerPin, HIGH);
    delay(20);
    digitalWrite(buzzerPin, LOW);
    delay(20);
    digitalWrite(buzzerPin, HIGH);
    delay(20);
    digitalWrite(buzzerPin, LOW);
    delay(20);
    digitalWrite(buzzerPin, HIGH);
    delay(20);
    digitalWrite(buzzerPin, LOW);
    delay(500);    
    break;   
 case 4: //assay start beep
    digitalWrite(buzzerPin, HIGH);
    delay(500);
    digitalWrite(buzzerPin, LOW);    
 }
}


