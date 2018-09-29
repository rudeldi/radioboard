// Basic demo for accelerometer readings from Adafruit LIS3DH

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

// Used for software SPI
//#define LIS3DH_CLK 13
//#define LIS3DH_MISO 12
//#define LIS3DH_MOSI 11
// Used for hardware & software SPI
//#define LIS3DH_CS 10

// software SPI
//Adafruit_LIS3DH lis = Adafruit_LIS3DH(LIS3DH_CS, LIS3DH_MOSI, LIS3DH_MISO, LIS3DH_CLK);
// hardware SPI
//Adafruit_LIS3DH lis = Adafruit_LIS3DH(LIS3DH_CS);
// I2C
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
   #define Serial SerialUSB
#endif

void setup(void) {
#ifndef ESP8266
  while (!Serial);     // will pause Zero, Leonardo, etc until serial console opens
#endif

  Serial.begin(9600);
  Serial.println("LIS3DH test!");
  
  if (! lis.begin(0x19)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    while (1);
  }
  Serial.println("LIS3DH found!");
  
  lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!
  
  Serial.print("Range = "); Serial.print(2 << lis.getRange());  
  Serial.println("G");
}

void loop() {
  lis.read();      // get X Y and Z data at once

  /* Or....get a new sensor event, normalized */ 
  sensors_event_t event; 
  lis.getEvent(&event);

  /* Calculate the magnitude of acceleration */
  float ax = event.acceleration.x;
  float ay = event.acceleration.y;
  float az = event.acceleration.z;

  float a = sqrt(pow(ax,2)+pow(ay,2)+pow(az,2));   
  Serial.print(" \ta: "); Serial.print(a);   Serial.println(" m/s^2 ");

  if (abs(a-10) > 20) {
    Serial.println("Very big shake");
  } else if (abs(a-10) > 15) {
    Serial.println("Big shake");
  } else if (abs(a-10) > 10) {
    Serial.println("Medium shake");
  } else if (abs(a-10) > 5) {
    Serial.println("Small shake");
  } else {
    
  }
     

  Serial.println();
 
  delay(200); 
}
