
/* Steuerung für das Radioboard der Maker Community Braunschweig
 * 
 * Autoren: Rudolf Leue
 *          Martin Dehmel
 *          
 * Stand: 26.09.2018
 * 
 */

// Bibliotheken einbinden
#include <SPI.h>
#include <Wire.h>
#include <TEA5767.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>


//Display initialisieren
#define OLED_RESET 4
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
Adafruit_SSD1306 display(OLED_RESET);

static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B11111111, B11111111,
  B11111111, B11111111,
  B11111111, B11111111,
  B11111111, B11111111,
  B11111111, B11111111,
  B11111111, B11111111,
  B11111111, B11111111,
  B11111111, B11111111,
  B11111111, B11111111,
  B11111111, B11111111,
  B11111111, B11111111,
  B11111111, B11111111,
  B11111111, B11111111,
  B11111111, B11111111,
  B11111111, B11111111,
  B11111111, B11111111  
};

 
//Radio-Chip initialisieren
TEA5767 radio = TEA5767();
int AnVal;
int OldAnVal = 0;
int NewFreq;

// Beschleunigungssensor initialisieren
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
   #define Serial SerialUSB
#endif

//Zusatz ini
int status_led = 7;

// Lautstärketasten initialisieren
#define VOLPLUS 3
#define VOLMIN 2
int volume = 0;
const int max_vol = 15;
const int min_vol = 0;

 // Timer für "Delays" initialisieren
unsigned long currentTime = millis();
unsigned long previousVolChange = 0;
unsigned long previousShake = 0;

void setup()
{ 
  //Schnittstellen initialisieren
  Serial.begin(9600);          //  setup serial
  Wire.begin();
  delay(100);
  radio.init();

  Serial.println("i2c Bus initialisiert...");
  delay(100);
  
  //Display initialisieren
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  Serial.println("Display initialisiert...");
 
  // Status-LED
  pinMode(status_led,OUTPUT); 
  pinMode(VOLPLUS, INPUT);
  pinMode(VOLMIN, INPUT);

  // Beschleunigungssensor
  #ifndef ESP8266
    while (!Serial);     // will pause Zero, Leonardo, etc until serial console opens
  #endif

  if (! lis.begin(0x19)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    while (1);
  }
  Serial.println("LIS3DH found!");
  
  lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!
  
  Serial.print("Range = "); Serial.print(2 << lis.getRange());  
  Serial.println("G");

  // Startmenü
  start_display();

}

void loop()
{
  digitalWrite(status_led ,LOW);
  AnVal = analogRead(A3); //Poti wird ausgelesen
  Serial.println(AnVal);
  NewFreq = map(AnVal, 0, 862, 8900, 10460);      // Mapping wird durchgeführt, analoger Wert auf die Frequenz umgerechnet
  NewFreq = 8900;
  radio.setFrequency(NewFreq);                    // Frequenz wird gesetzt
  update_display();                        // Neue Frequenz wird auf dem Display dargestellt
  OldAnVal = AnVal;

  // Laustärketasten checken
  if ((digitalRead(VOLPLUS) == 0) & (digitalRead(VOLMIN) == 0)) {
    mute();
  } else if (digitalRead(VOLPLUS) == 0) {
    volume_change(1);
  } else if (digitalRead(VOLMIN) == 0) {
    volume_change(-1);
  }
  
  // BS-Sensor auslesen
  lis.read();      // get X Y and Z data at once
    /* Or....get a new sensor event, normalized */ 
  sensors_event_t event; 
  lis.getEvent(&event);

  /* Calculate the magnitude of acceleration */
  float ax = event.acceleration.x;
  float ay = event.acceleration.y;
  float az = event.acceleration.z;
  float a = sqrt(pow(ax,2)+pow(ay,2)+pow(az,2));   

  if (abs(a-10) > 5) {
    shake_detected();
  }
}

//Mapping-Funktion auf Float geändert
float maps(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//Displayanzeige der Frequenz
void update_display(){
  float x = (float)NewFreq/100;
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(0,0);
  display.println(x);
  currentTime = millis();
  if (currentTime < previousVolChange + 10000) {
    display.drawRect(0, 25, 94, 6, 1);
    display.drawFastHLine(2, 27, volume*6, 1);
    display.drawFastHLine(2, 28, volume*6, 1);
    display.setTextSize(1);
    display.setCursor(102,23);
    display.println(volume);
  } else {
    display.setTextSize(1);
    display.setCursor(0,23);
    display.println("Radio MAKER COMMUNITY");
  }  
  display.display();
}


void volume_change(int steps) {
  currentTime = millis();                         // aktuelle Zeit auslesen
  if (currentTime > previousVolChange + 200) {    // 200ms "Delay" sicherstellen
    if (volume + steps > max_vol) {
      volume = max_vol;                           // obere Grenze: VOL = 15
    } else if (volume + steps < min_vol) {
      volume = min_vol;                           // untere Grenze: VOL = 0
    } else {
      volume += steps;                            // Lautstärke anpassen
    }
    Serial.println("VOL");
    update_display();
    radio.setVolume(volume);                      //momentan keine Funktion
    previousVolChange = currentTime;
  } else {
      // do nothing
  }
}

void mute() {
  volume = 0;
  update_display();
}

void shake_detected() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0,0);
  display.println("Shake \ndetected!");
  display.display();
  delay(5000);  
}

void start_display() {
  display.drawBitmap(0, 0,  logo16_glcd_bmp, 16, 16, 1);
  display.display();
  delay(1000);
  
}
