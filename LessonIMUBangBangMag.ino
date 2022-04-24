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
  

  Wire.begin();


  pinMode(BLUELED, OUTPUT); 
  pinMode(YELLOWLED, OUTPUT); 

  while(myIMU.begin()==false){
    Serial.println("IMU ERROR");  
    delay(100);
  }
  Serial.println("IMU Success");  
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


void deactivate(){
  digitalWrite(BLUELED, LOW);
  digitalWrite(YELLOWLED, LOW);
}

float activate(float gVal, float mVal){   
  gVal=smooth(gVal);


if(mVal>=0){
  if(gVal>LIMIT)
    digitalWrite(BLUELED, HIGH);
  
  if(gVal<-LIMIT)
    digitalWrite(YELLOWLED, HIGH);
  
  }
  else {
  if(gVal>LIMIT){}
  else
    digitalWrite(BLUELED, HIGH);

  if(gVal<-LIMIT){}
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
  }
}


void mag(){
  //Look for reports from the IMU
  if (myIMU.dataAvailable() == true)
  {
    mX = myIMU.getMagX();
    mY = myIMU.getMagY();
    mZ = myIMU.getMagZ();
  }
}


unsigned long lastTime=0;
int step=0;


void loop()
{
  unsigned long currentTime=millis();

  if(currentTime<lastTime+30)
    return;
  else
    lastTime=currentTime;

  Serial.print(gX, 2);
  Serial.print(F(","));
  Serial.println(sqrt(mX), 2);



  switch (step){

  case 0: 
    myIMU.enableGyro(10);    
    break;
  case 1:
    rotation();    
    break;
  case 2:
    deactivate();
    myIMU.enableMagnetometer(10);
    break;  
  case 3:
    mag();
    activate(gX,mX);
    break;      
  }

  step++;
  if (step>5) step=0;  
}


void loop1(){
  step++;
  if(step%2==0){
    Serial.println("ON");
    digitalWrite(YELLOWLED, LOW);
    digitalWrite(BLUELED, LOW);
  }
  else{
    Serial.println("OFF");
    digitalWrite(YELLOWLED, HIGH);
    digitalWrite(BLUELED, HIGH);
  }
  delay(5000);
}
