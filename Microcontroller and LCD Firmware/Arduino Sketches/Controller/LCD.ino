void setupLCD() {
    LCDUART.begin(LCDBAUD);
}

void testLCD() {
  //char out[] = {0xD7,0x30,0x30,'a','b','c',0x00};
  //LCDUART.write(out);

}

void resetLCD() {
  char out[] = {0xA0,0x02,0xFF,0xFF,0x60};
  //char out[] = {0xD9,0x30,0x30,0x30,0x35,0x30,0x30,0x30,0x37,0x30,0x30,0x36,0x35,0x30,0x30,0x36,0x37,0x31,0x34};
  for (int i=0;i<5;i++)
    LCDUART.write(out[i]);  
}

void pollLCD() {
  char new_char = 0;
  while (LCDUART.available() > 0) {
    new_char = (char)LCDUART.read();
    if (lcd_echo_mode)
      Serial.write(new_char);
    if (new_char==0x00) {
      lcd_buffer = "";
      LCDUART.flush();
    }
    else if (new_char==COMMAND_DELIMITER) {
      //a complete command has been received, so act on it
      parseCommand(lcd_buffer);
      lcd_buffer = "";
      LCDUART.flush();
    }
    else {
      lcd_buffer += new_char;      
    }
  }      
    
}

