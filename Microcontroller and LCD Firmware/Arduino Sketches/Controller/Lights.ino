
void setupLights() {
  // set the digital pin as output:
  pinMode(lightControlPin, OUTPUT);      
}

/* Sets the LED intensity level */
void setLights(int level) {
  if (level<0)
    level=0;
  else if (level>15)
    level=15;
    
  lights_level = level;  
  
  digitalWrite(lightControlPin, LOW);
  delayMicroseconds(1500);
  digitalWrite(lightControlPin, HIGH);
  delayMicroseconds(40);
  for (int i=level;i<15;i++) {
    digitalWrite(lightControlPin, LOW);
    delayMicroseconds(20);
    digitalWrite(lightControlPin, HIGH);
    delayMicroseconds(20);  
  }
}

void getLightsState() {
  Serial.print("lights_level "); Serial.println(lights_level);  


  LCDUART.write(0xD7);
  LCDUART.print("0A");
  LCDUART.print(lights_level);
  LCDUART.write(0x00);   
 
  
}

/* A test function that runs at startup and cycles through all the different intensity levels */
void testLights() {
  setLights(0);
  delay(100);
  setLights(1);
  delay(100);   
  setLights(2);
  delay(100);
  setLights(3);
  delay(100);  
  setLights(4);
  delay(100);
  setLights(5);
  delay(100);  
  setLights(6);
  delay(100);
  setLights(7);
  delay(100);   
  setLights(8);
  delay(100);
  setLights(9);
  delay(100);   
  setLights(10);
  delay(100);
  setLights(11);
  delay(100);  
  setLights(12);
  delay(100);
  setLights(13);
  delay(100);  
  setLights(14);
  delay(100);
  setLights(15);
  delay(100);   
  setLights(0);
}


