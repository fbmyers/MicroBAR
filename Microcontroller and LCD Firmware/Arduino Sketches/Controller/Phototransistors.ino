/*address lines are:
  Array Position: 0  1  2  3  4  5  6  7
  LAMP Board:     U2 U4 U5 A0 A1 A2 A3 A4
  P1 Pin:         1  2  3  4  5  6  7  8
  Arduino IO Pin: 4  5  6  7  8  9  10 11
  
  Note that enable lines are active low, while address lines are active high.
  
  */

/* these are the addresses of each phototransistor */  
const int address[] = 
{ 
B11100000,  //D0
B10101011,  //D1
B10100111,  //D2
B10100011,  //D3
B01101111,  //D4
B01101011,  //D5
B01100111,  //D6
B01100011,  //D7
B01110000,  //D8
B01110101,  //D9
B01111000,  //D10
B01111100,  //D11
B10110000,  //D12
B10101010,  //D13
B10100110,  //D14
B10100010,  //D15
B01101110,  //D16
B01101010,  //D17
B01100110,  //D18
B01100010,  //D19
B01110001,  //D20
B01110100,  //D21
B01111001,  //D22
B01111101,  //D23
B10110001,  //D24
B10101001,  //D25
B10100101,  //D26
B10100001,  //D27
B01101101,  //D28
B01101001,  //D29
B01100101,  //D30
B01100001,  //D31
B01110010,  //D32
B01110110,  //D33
B01111010,  //D34
B01111110,  //D35
B10110010,  //D36
B10101000,  //D37
B10100100,  //D38
B10100000,  //D39
B01101100,  //D40
B01101000,  //D41
B01100100,  //D42
B01100000,  //D43
B01110011,  //D44
B01110111,  //D45
B01111011,  //D46
B01111111,  //D47
B10110011,  //D48
B11001111,  //D49
B11001000,  //D50
B11000100,  //D51
B11000000,  //D52
B11011100,  //D53
B11011000,  //D54
B11010100,  //D55
B11010000,  //D56
B10101111,  //D57
B10111100,  //D58
B10111000,  //D59
B10110100,  //D60
B11001110,  //D61
B11001001,  //D62
B11000101,  //D63
B11000001,  //D64
B11011101,  //D65
B11011001,  //D66
B11010101,  //D67
B11010001,  //D68
B10101110,  //D69
B10111101,  //D70
B10111001,  //D71
B10110101,  //D72
B11001101,  //D73
B11001010,  //D74
B11000110,  //D75
B11000010,  //D76
B11011110,  //D77
B11011010,  //D78
B11010110,  //D79
B11010010,  //D80
B10101101,  //D81
B10111110,  //D82
B10111010,  //D83
B10110110,  //D84
B11001100,  //D85
B11001011,  //D86
B11000111,  //D87
B11000011,  //D88
B11011111,  //D89
B11011011,  //D90
B11010111,  //D91
B11010011,  //D92
B10101100,  //D93
B10111111,  //D94
B10111011,  //D95
B10110111,  //D96
};

void setupPhototransistors() {
  pinMode(U2OutPin, OUTPUT);  
  pinMode(U4OutPin, OUTPUT); 
  pinMode(U5OutPin, OUTPUT); 
  pinMode(A0OutPin, OUTPUT); 
  pinMode(A1OutPin, OUTPUT); 
  pinMode(A2OutPin, OUTPUT); 
  pinMode(A3OutPin, OUTPUT); 
  pinMode(A4OutPin, OUTPUT); 
  setWell(0);
}

int setWell(int wellNum) {
  digitalWrite(U2OutPin,((address[wellNum]&B10000000)!=0)?HIGH:LOW);
  digitalWrite(U4OutPin,((address[wellNum]&B01000000)!=0)?HIGH:LOW);
  digitalWrite(U5OutPin,((address[wellNum]&B00100000)!=0)?HIGH:LOW);
  digitalWrite(A4OutPin,((address[wellNum]&B00010000)!=0)?HIGH:LOW);
  digitalWrite(A3OutPin,((address[wellNum]&B00001000)!=0)?HIGH:LOW);
  digitalWrite(A2OutPin,((address[wellNum]&B00000100)!=0)?HIGH:LOW);
  digitalWrite(A1OutPin,((address[wellNum]&B00000010)!=0)?HIGH:LOW);
  digitalWrite(A0OutPin,((address[wellNum]&B00000001)!=0)?HIGH:LOW); 
  pt_wellNum = wellNum;
}

void pollAllPhototransistors() {
  long collector;
  float average;
  for (int i=start_well;i<=stop_well;i++) {
    setWell(i);
    delay(PT_SWITCH_TO_READ_DELAY);
    collector = 0;
    for (int j=0;j<pt_numSamplesToAverage;j++) {
      collector += analogRead(phototransistorInPin); 
    }
    average = ((float)collector)/((float)pt_numSamplesToAverage) - PT_OFFSET;
    Serial.print("pt "); Serial.print(i); Serial.print(" "); Serial.println(average);
  
    char add[3];
    char val[5];
   
    sprintf(add,"%02X",i);
    sprintf(val,"%04X",(int)average); 
    LCDUART.write(0xD6);
    LCDUART.print(add);
    LCDUART.print(val);
    LCDUART.write(0x00); 
    Serial.println("test");  
    Serial.println(add);
    Serial.println(val);
    Serial.println("test");    
  }  
}

void pollSinglePhototransistor() {
  long collector;
  float average;
  collector = 0;
  for (int j=0;j<pt_numSamplesToAverage;j++) {
    collector += analogRead(phototransistorInPin); 
  }
  average = ((float)collector)/((float)pt_numSamplesToAverage) - PT_OFFSET;
  Serial.print("pt "); Serial.print(pt_wellNum); Serial.print(" "); Serial.println(average);  
}

void getPhototransistorsState() {
    Serial.print("pt_wellnum "); Serial.println(pt_wellNum);
    Serial.print("pt_numsamplestoavg "); Serial.println(pt_numSamplesToAverage);  
}

