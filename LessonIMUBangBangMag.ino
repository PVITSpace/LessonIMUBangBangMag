/*
  Using the BNO080 IMU
  By: Nathan Seidle
  SparkFun Electronics
  Date: December 21st, 2017
  SparkFun code, firmware, and software is released under the MIT License.
	Please see LICENSE.md for further details.

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14586

  This example shows how to output the parts of the calibrated gyro.

  It takes about 1ms at 400kHz I2C to read a record from the sensor, but we are polling the sensor continually
  between updates from the sensor. Use the interrupt pin on the BNO080 breakout to avoid polling.

  Hardware Connections:
  Attach the Qwiic Shield to your Arduino/Photon/ESP32 or other
  Plug the sensor onto the shield
  Serial.print it out at 115200 baud to serial monitor.
*/

#include <Wire.h>

#include "SparkFun_BNO080_Arduino_Library.h" // Click here to get the library: http://librarymanager/All#SparkFun_BNO080
BNO080 myIMU;

#define BLUELED 13
#define YELLOWLED 14
#define LIMIT 0.3

#define ARRAYSIZE 4

float signals[ARRAYSIZE]={0.0};
float gX,gY,gZ, mX,mY,mZ;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("BNO080 Read Example");

  Wire.begin();


  pinMode(BLUELED, OUTPUT); 
  pinMode(YELLOWLED, OUTPUT); 
  myIMU.begin();
  
  Wire.setClock(400000); //Increase I2C data rate to 400kHz
  myIMU.enableGyro(50); //Send data update every 50ms
  //myIMU.enableMagnetometer(50); //Send data update every 50ms

  Serial.println(F("x, y, z"));
}

float smooth(float tmp){
  int count;
  for(count=0;count<(ARRAYSIZE-1);count++){
    signals[count]=signals[count+1];
  }
   signals[count]=tmp;

   float favg=0.0;

  for(count=0;count<ARRAYSIZE;count++){
    favg=signals[count];
  }
  favg/=ARRAYSIZE;
  return favg;     
}



float activate(float gVal, float mVal){   
  gVal=smooth(gVal);


if(mVal>=0){
  if(gVal>LIMIT)
    digitalWrite(BLUELED, HIGH);
  else
    digitalWrite(BLUELED, LOW);

  if(gVal<-LIMIT)
    digitalWrite(YELLOWLED, HIGH);
  else
    digitalWrite(YELLOWLED, LOW);
  }
  else {
  if(gVal>LIMIT)
    digitalWrite(BLUELED, LOW);
  else
    digitalWrite(BLUELED, HIGH);

  if(gVal<-LIMIT)
    digitalWrite(YELLOWLED, LOW);
  else
    digitalWrite(YELLOWLED, HIGH);
  }
}



void rotation(){
  //Look for reports from the IMU
  if (myIMU.dataAvailable() == true)
  {
    gX = myIMU.getGyroX();
    gY = myIMU.getGyroY();
    gZ = myIMU.getGyroZ();

    
    Serial.print(gX, 2);
    Serial.print(F(","));
    Serial.print(gY, 2);
    Serial.print(F(","));
    Serial.print(gZ, 2);
    Serial.print(F(","));

    Serial.println();
    
  }
}


void mag(){
  //Look for reports from the IMU
  if (myIMU.dataAvailable() == true)
  {
    mX = myIMU.getMagX();
    mY = myIMU.getMagY();
    mZ = myIMU.getMagZ();

    
    Serial.print(mX, 2);
    Serial.print(F(","));
    Serial.print(mY, 2);
    Serial.print(F(","));
    Serial.print(mZ, 2);
    Serial.print(F(","));

    Serial.println();    
  }
}


unsigned long lastTime=0;
int step=0;
void loop()
{
  unsigned long currentTime=millis();

  if(currentTime<lastTime+50)
    return;
  else
    lastTime=currentTime;

  switch (step){

  case 0: 
    myIMU.enableGyro(50);    
    break;
  case 1:
    rotation();    
    break;
  case 2:
    myIMU.enableMagnetometer(50);
    break;  
  case 3:
    mag();
    break;    
  case 4:
    activate(gX,mX);
    break;        
  }

  step++;
  if (step>5) step=0;  
}
