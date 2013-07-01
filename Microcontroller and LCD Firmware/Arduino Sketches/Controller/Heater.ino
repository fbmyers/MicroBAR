

void setupHeater() {
  // set the digital pin as output:
  pinMode(heaterControlPin, OUTPUT);     
}

//note that analog values range from 0 - 1024 for 0 - 5V
float getTemp() {
  long collector = 0;
  for (int i=0;i<NUM_THERMISTOR_SAMPLES_TO_AVG;i++)
    collector += analogRead(thermistorInPin);
  
  return ((float)collector/NUM_THERMISTOR_SAMPLES_TO_AVG)*heater_coeff + heater_offset;
}

boolean getTempOK() {
  if (heater_sp==0)
    return true;
  else
    return (abs(heater_sp - heater_temp) < heater_tolerance);  
}

void pollHeater() {
  heater_temp = getTemp();
  heater_on = (heater_sp>heater_temp);
  digitalWrite(heaterControlPin, heater_on);
  Serial.print("heater_temp "); Serial.println(heater_temp); 
  Serial.print("heater_on "); Serial.println((int)heater_on); 
  Serial.print("temp_ok "); Serial.println((int)getTempOK()); 
  
  LCDUART.write(0xD7);
  LCDUART.print("08");
  LCDUART.print(heater_temp);
  LCDUART.write(0x00);  
  
 
}

void getHeaterState() {
  pollHeater();  
  Serial.print("heater_sp "); Serial.println(heater_sp);
  Serial.print("heater_coeff "); Serial.println(heater_coeff);
  Serial.print("heater_offset "); Serial.println(heater_offset);
  Serial.print("heater_tolerance "); Serial.println(heater_tolerance);
  
  LCDUART.write(0xD7);
  LCDUART.print("09");
  LCDUART.print(heater_sp);
  LCDUART.write(0x00);    
}

