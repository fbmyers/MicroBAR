void setupSDCard() {
  pinMode(sdCSPin, OUTPUT);     
  pinMode(sdCDPin, INPUT);    
  pinMode(sdWPPin, INPUT);   

  //I screwed up and accidentally connected these instead. note that on the board I've added jumpers to the correct SCK/MISO/MOSI pins (51, 52, 53)
  pinMode(sdSCKPin, INPUT);   
  pinMode(sdMISOPin, INPUT);    
  pinMode(sdMOSIPin, INPUT);     

  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(53, OUTPUT);
 
  // see if the card is present and can be initialized:
  if (!SD.begin(sdCSPin)) {
    Serial.println("SD Card failed or not present");
    sd_card_present = false;
    return;
  }
  Serial.println("SD card initialized."); 
  sd_card_present = true;
 
  sdWrite("TEST");
}

//writes a string to the SD card
//TODO: check files on card and sequentially number new files
//TODO: whatever gets sent to PC should also get sent to SD card (log file)
void sdWrite(String txt) {
  File dataFile = SD.open("microbar.log", FILE_WRITE);  
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.print(txt);
    dataFile.close();
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }  
  
}
