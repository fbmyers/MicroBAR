
void setupPCInterface() {
   Serial.begin(PCBAUD);  
   uart_buffer = "";
}

void pollUART() {
  char new_char = 0;
  while (Serial.available() > 0) {
    new_char = (char)Serial.read();
    if (lcd_echo_mode)
      LCDUART.write(new_char);
    if (new_char==COMMAND_DELIMITER) {
      //a complete command has been received, so act on it
      parseCommand(uart_buffer);
      uart_buffer = "";
      Serial.flush();
    }
    else {
      uart_buffer += new_char;      
    }
  }   
}

void parseCommand(String command) {
  int delimiter_loc;
  char cmd[20];
  char param[20];
  
  delimiter_loc = command.indexOf(' ');
  if (delimiter_loc==-1)
      delimiter_loc = command.length();
  command.substring(0,delimiter_loc).toCharArray(cmd,20);
  command.substring(delimiter_loc+1).toCharArray(param,20);  
  
  
  if (strcmp(cmd,"GETSTATE")==0) {
    getControllerState();
    getGPSState();
    getHeaterState();
    getLightsState();
    getPhototransistorsState();
    testLCD();    
  }
  else if (strcmp(cmd,"SETLIGHTS")==0) { setLights(atoi(param)); }
  else if (strcmp(cmd,"TESTLIGHTS")==0) { testLights(); }
  else if (strcmp(cmd,"SETTEMP")==0)   { heater_sp = atoi(param);  }
  else if (strcmp(cmd,"POLLSINGLEWELL")==0)   { setWell(atoi(param)); poll_single_well_mode = 1; } //deprecated
  else if (strcmp(cmd,"POLLSINGLEWELLOFF")==0)   { poll_single_well_mode = 0; } //deprecated
  else if (strcmp(cmd,"POLLALLWELLS")==0) { pollAllPhototransistors(); }  //deprecated
  else if (strcmp(cmd,"STARTASSAY")==0){ assay_state = 1; }
  else if (strcmp(cmd,"ABORTASSAY")==0){ assay_state = 5; } 
  else if (strcmp(cmd,"FREERUNON")==0){ freerun_mode = 1; }
  else if (strcmp(cmd,"FREERUNOFF")==0){ freerun_mode = 0; }
  else if (strcmp(cmd,"STARTWELL")==0) { start_well = atoi(param); }
  else if (strcmp(cmd,"STOPWELL")==0) { stop_well = atoi(param); }
  else if (strcmp(cmd,"LCDECHOON")==0){ lcd_echo_mode = 1; }
  else if (strcmp(cmd,"LCDECHOOFF")==0){ lcd_echo_mode = 0; } 
  else if (strcmp(cmd,"LCDRESET")==0){ resetLCD(); } 
  else if (strcmp(cmd,"SETASSAYNAME")==0){ assay_name = param; }
  else if (strcmp(cmd,"SETASSAYTIMESTEP")==0){ assay_timestep = atoi(param); }
  else if (strcmp(cmd,"SETASSAYNUMSTBSTEPS")==0){ assay_numstabilizesteps = atoi(param); }
  else if (strcmp(cmd,"SETASSAYNUMRUNSTEPS")==0){ assay_numrunsteps = atoi(param); }
  else if (strcmp(cmd,"SETASSAYHEATERSP")==0){ assay_heater_sp = atof(param); }
  else if (strcmp(cmd,"SETASSAYLIGHTSLEVEL")==0){ assay_lights_level = atoi(param); }
  else if (strcmp(cmd,"SETASSAYSTARTWELL")==0){ assay_startwell = atoi(param); }
  else if (strcmp(cmd,"SETASSAYSTOPWELL")==0){ assay_stopwell = atoi(param); }  
}

