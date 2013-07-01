import processing.serial.*;
import processing.opengl.*;
import controlP5.*;
import java.io.File;

static final float UI_VERSION = 0.2;

ControlP5 controlP5;
Serial myPort;  // Create object from Serial class
String uart_buffer;
boolean do_save_data;
boolean do_log_data;
Textfield txt_outputfolder;
Textfield txt_lights_level;
Textfield txt_heater_sp;
Textfield txt_assay_name;
Textfield txt_assay_timestep;
Textfield txt_assay_numstabilizesteps;
Textfield txt_assay_numrunsteps;
Textfield txt_assay_heater_sp;
Textfield txt_assay_lights_level;
Textfield txt_startwellnum;
Textfield txt_stopwellnum;
Textfield txt_assay_startwell;
Textfield txt_assay_stopwell;
Textfield txt_com_port;

Button btn_freerun;
Button btn_lcdecho;

String state_names[] = {"Idle", "Startup", "Warming", "Stabilizing", "Running", "Shutdown"};

int assay_step;
int assay_state;
String assay_name;
int assay_numrunsteps;
int assay_numstabilizesteps;
int assay_timestep;
float assay_heater_sp;
int assay_lights_level;
int assay_startwell;
int assay_stopwell;
boolean heater_on;
boolean temp_ok;
float heater_sp;
int lights_level;
float firmware_ver;
float gps_longitude;
float gps_latitude;
float gps_altitude;
int pt_wellnum;
int pt_numsamplestoavg;

float[] data = new float[96];
long assay_time;
float batt_voltage;
float heater_temp;
float heater_coeff;
float heater_offset;
float heater_tolerance;

void setup() 
{
  size(750, 650, OPENGL);
  
  textFont(loadFont("ArialMT-12.vlw"));

  
  controlP5 = new ControlP5(this);
  controlP5.addButton("setassay",0,570,500,140,30).setLabel("Set Assay Params");  
  controlP5.addButton("startassay",0,570,550,140,30).setLabel("Start Assay");
  controlP5.addButton("abortassay",0,570,600,140,30).setLabel("Abort Assay");
  txt_startwellnum = controlP5.addTextfield("",220,400,30,20);
  txt_startwellnum.setText("1");
  txt_stopwellnum = controlP5.addTextfield("",250,400,30,20);
  txt_stopwellnum.setText("96");
  btn_freerun = controlP5.addButton("freerun",0,280,400,80,20);
  btn_freerun.setLabel("Polling ON");
  controlP5.addButton("getstate",0,380,400,80,20).setLabel("Get State");
  controlP5.addButton("lcdreset",0,20,430,80,20).setLabel("LCD Reset");
  btn_lcdecho = controlP5.addButton("lcdecho",0,120,430,80,20);
  btn_lcdecho.setLabel("LCD Echo ON");
  txt_heater_sp = controlP5.addTextfield("",20,400,30,20);
  txt_heater_sp.setText("0");
  controlP5.addButton("settemp",0,50,400,50,20).setLabel("Set Temp");
  txt_lights_level = controlP5.addTextfield("",120,400,30,20);
  txt_lights_level.setText("0");
  controlP5.addButton("setlights",0,150,400,50,20).setLabel("Set Lights");
  txt_com_port = controlP5.addTextfield("",570,20,50,20);
  txt_com_port.setText("COM10");
  controlP5.addButton("connect",0,630,20,80,20).setLabel("Connect");  

  fill(0);

  txt_outputfolder = controlP5.addTextfield("txt_outputfolder",20,480,340,20);
  txt_outputfolder.setText("C:\\Data\\LAMP\\");

  txt_assay_name = controlP5.addTextfield("txt_assay_name",40,560,100,20);
  txt_assay_name.setText("test1");

  txt_assay_timestep = controlP5.addTextfield("txt_assay_timestep",180,560,50,20);
  txt_assay_timestep.setText("2000");

  txt_assay_numstabilizesteps = controlP5.addTextfield("txt_assay_numstabilizesteps",280,560,40,20);
  txt_assay_numstabilizesteps.setText("10");

  txt_assay_numrunsteps = controlP5.addTextfield("txt_assay_numrunsteps",380,560,40,20);
  txt_assay_numrunsteps.setText("6000");  

  txt_assay_heater_sp = controlP5.addTextfield("txt_assay_heater_sp",40,610,40,20);
  txt_assay_heater_sp.setText("60"); 

  txt_assay_lights_level = controlP5.addTextfield("txt_assay_lights_level",120,610,40,20);
  txt_assay_lights_level.setText("1");   

  txt_assay_startwell = controlP5.addTextfield("txt_assay_startwell",200,610,40,20);
  txt_assay_startwell.setText("1");   
  
  txt_assay_stopwell = controlP5.addTextfield("txt_assay_stopwell",280,610,40,20);
  txt_assay_stopwell.setText("96");   
  
  connect();
}

public void connect() {
    try {
      myPort.stop();
    }
    catch(Exception e) {
    }
    try {
      myPort = new Serial(this, txt_com_port.getText(), 115200);
      delay(100);
      getstate();
      delay(100);
      getstate();
      delay(100);
    }
    catch(Exception e) {
      javax.swing.JOptionPane.showMessageDialog(null, "MicroBAR Not Found on " + txt_com_port.getText() + "!");      
    }


}

void draw()
{
  check_uart();
  background(255);
  draw_wells();
  draw_plots();
  draw_text();

  //cant get the textlabel control to work!
  text("Output Path:",20,470);  
  text("Assay Parameters:",20,530);
  text("Name:",40,550);  
  text("Step Time (ms):",180,550);
  text("# Stabilize Steps:",280,550);
  text("# Run Steps:",380,550);
  text("Temp:",40,600);
  text("LEDs:",120,600);
  text("Start Well:",200,600);
  text("Stop Well:",280,600);
  
}

public void check_uart() { //TODO: i think this needs some work w.r.t handling sequential commands in rapid fire
    if (myPort!=null) {
      while((uart_buffer = myPort.readStringUntil('\n'))!=null) {
        print(uart_buffer);
        parse_data(uart_buffer);
        if (do_log_data)
          log_data(trim(uart_buffer));
      }
    }   
}

void send_command(String cmd) { 
  if (myPort!=null) {
    print("> " + cmd);     
    myPort.write(cmd);
    log_data("> " + cmd);
  }
}

void draw_wells() {
  int well_diam = 30;
  int well_sp = 15;
  int margin = 20;
  int start_x = 0;
  int start_y = 0;
  int w_x,w_y,t_x,t_y,col;
  
  textAlign(CENTER,CENTER);
  
  for (int i=0;i<96;i++) {
    w_x = (start_x + margin + well_diam/2) + i%12*(well_diam + well_sp);
    w_y = (start_y + margin + well_diam/2) + i/12*(well_diam + well_sp);
    col = (int)(data[i]/2);
    fill(0,col,0); //from 0 - 255
    ellipse(w_x,w_y,well_diam,well_diam);
    fill(255);
    text(Integer.toString(i+1),w_x,w_y); 
    fill(0); 
    text(Float.toString(data[i]),w_x,w_y+well_diam*0.7); 
  }  
}

void draw_plots() {

}


void draw_text() {
  int start_x = 570;
  int start_y = 60;
  textAlign(LEFT,BASELINE);
  fill(0);
  String status_string = 
      String.format("System Settings:\n" +
                    "   UI Version: %.1f\n" +      
                    "   Firmware Version: %.1f\n" +
                    "   Thermistor Coeff: %.2f\n" +
                    "   Thermistor Offset: %.2f\n" +
                    "   Temp Tolerance: %.1f\n" +
                    "   Samples to Average: %d\n\n" +                    
                    "Current State:\n" +
                    "   Assay State: %s\n" +
                    "   Assay Step: %d\n" +
                    "   Assay Time: %d\n" +
                    "   Temperature: %.1f\n" +
                    "   Temp Setpoint: %.1f\n" +   
                    "   Temp OK: %b\n" + 
                    "   Heater ON: %b\n" +                      
                    "   LED Brightness: %d\n" +      
                    "   Battery Voltage: %.2f\n" +  
                    "   GPS Longitude: %.5f\n" + 
                    "   GPS Latitude: %.5f\n" + 
                    "   GPS Altitude: %.5f\n\n" +                    
                    "Loaded Assay:\n" +
                    "   Name: %s\n" +                    
                    "   Num Stablize Steps: %d\n" +
                    "   Num Run Steps: %d\n" +                    
                    "   Sampling Interval: %d\n" + 
                    "   Temp Setpoint: %.1f\n" +
                    "   LED Brightness: %d\n" +
                    "   Start Well: %d\n" +
                    "   Stop Well: %d\n",                    
                    UI_VERSION,
                    firmware_ver,
                    heater_coeff,
                    heater_offset,
                    heater_tolerance,
                    pt_numsamplestoavg,
                    state_names[assay_state],
                    assay_step,
                    assay_time,
                    heater_temp,
                    heater_sp,
                    temp_ok,
                    heater_on,
                    lights_level,
                    batt_voltage,
                    gps_longitude,
                    gps_latitude,
                    gps_altitude,
                    assay_name,
                    assay_numstabilizesteps,
                    assay_numrunsteps,
                    assay_timestep,
                    assay_heater_sp,
                    assay_lights_level,
                    assay_startwell,
                    assay_stopwell
                    );
                  
  text(status_string,start_x,start_y);
}

public void log_data(String dat) {
  try {
    BufferedWriter bw = new BufferedWriter(new FileWriter((txt_outputfolder.getText() + ".log"), true));
    bw.write(dat);
    bw.newLine();
    bw.flush();
    bw.close();
  }
  catch (IOException ioe) {
    println("ERROR: Could not write to disk:\n" + ioe.toString());
  }  
}

//TODO: fix this function or remove it (stick w/ log format)
public void save_data() {
  //save(assay_step-1);
  //File fol = new File(txt_foldername.getText());
  //fol.mkdir(); //TODO: check to see if it already exists
  
  //TODO: this might be better off in the initial start assay code
  String dat;
  
  if (assay_step==1) {
    //add the heater
    dat = "step\ttime\ttemp\tbatt\t"; 
    for (int i=1;i<=96;i++) 
      dat += String.format("d%d\t",i);       
  }
  else {
    //add the step#, temp, and voltage to the output row.
    dat = String.format("%d\t%d\t%.1f\t%.2f\t",assay_step-1,assay_time,heater_temp,batt_voltage);
    //add all the well values
    for (int i=0;i<96;i++) 
      dat += String.format("%f\t",data[i]);
    
  }
  
  try {
    BufferedWriter bw = new BufferedWriter(new FileWriter((txt_outputfolder.getText() + ".dat"), true));
    bw.write(dat);
    bw.newLine();
    bw.flush();
    bw.close();
  }
  catch (IOException ioe) {
    println("ERROR: Could not write to disk:\n" + ioe.toString());
  }

}

public void startassay() {
  send_command("STARTASSAY\n"); //TODO: check the filename first
  //do_save_data = true;
  //do_log_data = true;
}

public void abortassay() {
  send_command("ABORTASSAY\n");
  do_save_data = false;
  do_log_data = false;
}

public void freerun() {
  if (btn_freerun.label()=="Polling ON") {
    btn_freerun.setLabel("Polling OFF");
    send_command("STARTWELL " + txt_startwellnum.getText() + "\n");
    delay(100);
    send_command("STOPWELL " + txt_stopwellnum.getText() + "\n");
    delay(100);
    send_command("FREERUNON\n");
    delay(100);
  }
  else {
    btn_freerun.setLabel("Polling ON");
    send_command("FREERUNOFF\n");    
  }
}

public void getstate() {
  delay(100);
  send_command("GETSTATE\n");
  delay(100);
}

public void lcdreset() {
  send_command("LCDRESET\n");
}

public void lcdecho() {
  if (btn_lcdecho.label()=="LCD Echo ON") {
    btn_lcdecho.setLabel("LCD Echo OFF");
    send_command("LCDECHOON\n");
  }
  else {
    btn_lcdecho.setLabel("LCD Echo ON");
    send_command("LCDECHOOFF\n");    
  }
}

public void settemp() {
  send_command("SETTEMP " + txt_heater_sp.getText() + "\n");
  delay(100);
  getstate();
}

public void setlights() {
  send_command("SETLIGHTS " + txt_lights_level.getText() + "\n");
  delay(100);
  getstate();
}

public void setassay() {
  send_command("SETASSAYNAME " + txt_assay_name.getText() + "\n");
  delay(100);
  send_command("SETASSAYTIMESTEP " + txt_assay_timestep.getText() + "\n");
  delay(100);
  send_command("SETASSAYNUMSTBSTEPS " + txt_assay_numstabilizesteps.getText() + "\n");
  delay(100);
  send_command("SETASSAYNUMRUNSTEPS " + txt_assay_numrunsteps.getText() + "\n");
  delay(100);
  send_command("SETASSAYHEATERSP " + txt_assay_heater_sp.getText() + "\n");
  delay(100);
  send_command("SETASSAYLIGHTSLEVEL " + txt_assay_lights_level.getText() + "\n");
  delay(100);
  send_command("SETASSAYSTARTWELL " + txt_assay_startwell.getText() + "\n");
  delay(100);
  send_command("SETASSAYSTOPWELL " + txt_assay_stopwell.getText() + "\n");
  delay(100);
  getstate();
}


public void parse_data(String data_string) {
  int delimiter_loc = data_string.indexOf(' ');  //TODO: look at str tokenizer
  String resp_type, resp_data;
  
  if (delimiter_loc==-1) {
      resp_type = trim(data_string);
      resp_data = "0";
  }
  else {
      resp_type = trim(data_string.substring(0,delimiter_loc));
      resp_data = trim(data_string.substring(delimiter_loc+1)); 
  }
  
  if (resp_type.equals("pt")) {
    delimiter_loc = resp_data.indexOf(' ');
    int well =  Integer.parseInt(resp_data.substring(0,delimiter_loc));
    float val = Float.parseFloat(resp_data.substring(delimiter_loc+1));
    data[well-1] = val;  
  }
  else if (resp_type.equals("assay_step")) { 
    //this is is the first thing that is sent when the controller starts a new step.
    assay_step = Integer.parseInt(resp_data);
        
    save_data(); //saves the PREVIOUS step, with all the current values

    //TODO: when assay completes (assay_state=4), need to save last sample;
  }
  else if (resp_type.equals("assay_state")) {
      assay_state = Integer.parseInt(resp_data);
      if (assay_state==5) { //closing
        save_data(); //this saves the last row.
      
    }
  }
  else if (resp_type.equals("assay_time"))       { assay_time = Long.parseLong(resp_data); }
  else if (resp_type.equals("assay_name"))       { assay_name = resp_data; }
  else if (resp_type.equals("assay_numstabilizesteps"))   { assay_numstabilizesteps = Integer.parseInt(resp_data); }  
  else if (resp_type.equals("assay_numrunsteps"))   { assay_numrunsteps = Integer.parseInt(resp_data); }
  else if (resp_type.equals("assay_timestep"))   { assay_timestep = Integer.parseInt(resp_data); }
  else if (resp_type.equals("assay_heater_sp"))    { assay_heater_sp = Float.parseFloat(resp_data); }
  else if (resp_type.equals("assay_lights_level")) { assay_lights_level = Integer.parseInt(resp_data); }
  else if (resp_type.equals("assay_startwell")) { assay_startwell = Integer.parseInt(resp_data); }
  else if (resp_type.equals("assay_stopwell")) { assay_stopwell = Integer.parseInt(resp_data); }
  else if (resp_type.equals("heater_temp"))      { heater_temp = Float.parseFloat(resp_data); }
  else if (resp_type.equals("heater_coeff"))      { heater_coeff = Float.parseFloat(resp_data); }
  else if (resp_type.equals("heater_offset"))      { heater_offset = Float.parseFloat(resp_data); }
  else if (resp_type.equals("heater_tolerance"))      { heater_tolerance = Float.parseFloat(resp_data); }
  else if (resp_type.equals("heater_on"))        { heater_on = (Integer.parseInt(resp_data)==1); }
  else if (resp_type.equals("temp_ok"))          { temp_ok = (Integer.parseInt(resp_data)==1); }
  else if (resp_type.equals("heater_sp"))        { heater_sp = Float.parseFloat(resp_data); }
  else if (resp_type.equals("lights_level"))     { lights_level = Integer.parseInt(resp_data); }
  else if (resp_type.equals("batt_voltage"))     { batt_voltage = Float.parseFloat(resp_data); }
  else if (resp_type.equals("firmware_ver"))     { firmware_ver = Float.parseFloat(resp_data); }
  else if (resp_type.equals("gps_longitude"))    { gps_longitude = Float.parseFloat(resp_data); }  
  else if (resp_type.equals("gps_latitude"))     { gps_latitude = Float.parseFloat(resp_data); } 
  else if (resp_type.equals("gps_altitude"))     { gps_altitude = Float.parseFloat(resp_data); } 
  else if (resp_type.equals("pt_wellnum"))       { pt_wellnum = Integer.parseInt(resp_data); } 
  else if (resp_type.equals("pt_numsamplestoavg"))       { pt_numsamplestoavg = Integer.parseInt(resp_data); } 
  else if (resp_type.equals("assay_begin")) {
     do_log_data = true;
     do_save_data = true;
  }
  else if (resp_type.equals("assay_end")) {
    do_log_data = false;
    do_save_data = false;
  }
  
 
  //TODO: save ALL to a current state var and IF there's to be logging, afterwards place this in the step array
  //TODO: dynamically allocate state array based on the assay_numsteps command.  This will also resize graphs.
  
}
