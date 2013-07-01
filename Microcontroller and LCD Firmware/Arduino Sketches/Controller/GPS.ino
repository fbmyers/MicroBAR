void setupGPS() {
  GPSUART.begin(GPSBAUD);  
}

void pollGPS()
{
  /*
  if (GPSUART.available()) {
    int c = GPSUART.read();    // load the data into a variable...
    if(gps.encode(c))      // if there is a new valid sentence...
    {
      gps.f_get_position(&gps_latitude, &gps_longitude);
      gps_altitude = gps.f_altitude();
      gps.crack_datetime(&gps_year,&gps_month,&gps_day,&gps_hour,&gps_minute,&gps_second,&gps_hundredths);
      
      //getgps(gps);         // then grab the data.
    }
  }
  Serial.print("gps_longitude "); Serial.println(gps_longitude);
  Serial.print("gps_latitude "); Serial.println(gps_latitude);
  Serial.print("gps_altitude "); Serial.println(gps_altitude);  
  Serial.print("gps_year "); Serial.println(gps_year);  
  Serial.print("gps_month "); Serial.println(gps_month);  
  Serial.print("gps_day "); Serial.println(gps_day);  
  Serial.print("gps_hour "); Serial.println(gps_hour);  
  Serial.print("gps_minute "); Serial.println(gps_minute);  
  Serial.print("gps_second "); Serial.println(gps_second);  
  Serial.print("gps_hundredths "); Serial.println(gps_hundredths);  
  */
}

void getGPSState()
{
  pollGPS();

}

// The getgps function will get and print the values we want.
//void getgps(TinyGPS &gps)
//{
  
  
  /*
  // Print data and time
  Serial.print("Date: "); Serial.print(month, DEC); Serial.print("/"); 
  Serial.print(day, DEC); Serial.print("/"); Serial.print(year);
  Serial.print("  Time: "); Serial.print(hour, DEC); Serial.print(":"); 
  Serial.print(minute, DEC); Serial.print(":"); Serial.print(second, DEC); 
  Serial.print("."); Serial.println(hundredths, DEC);
  //Since month, day, hour, minute, second, and hundr
  
  // Here you can print the altitude and course values directly since 
  // there is only one value for the function
  Serial.print("Altitude (meters): "); Serial.println(gps.f_altitude());  
  // Same goes for course
  Serial.print("Course (degrees): "); Serial.println(gps.f_course()); 
  // And same goes for speed
  Serial.print("Speed(kmph): "); Serial.println(gps.f_speed_kmph());
  Serial.println();
  
  // Here you can print statistics on the sentences.
  unsigned long chars;
  unsigned short sentences, failed_checksum;
  gps.stats(&chars, &sentences, &failed_checksum);
  //Serial.print("Failed Checksums: ");Serial.print(failed_checksum);
  //Serial.println(); Serial.println();
  */
//}


