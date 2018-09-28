
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
#include <EEPROM.h>

//Display initialisieren
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

 
//Radio Deklarierung
TEA5767 radio = TEA5767();
int AnVal;
int OldAnVal = 0;
int NewFreq;

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

void setup()
{ 
  //Schnittstellen initialisieren
  Serial.begin(9600);          //  setup serial
  Wire.begin();
  delay(100);
  // radio.init();

  Serial.println("i2c Bus initialisiert...");
  
  delay(100);
  //Display initialisieren
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  Serial.println("Display initialisiert...");
 

  //Status LED Initialisieren
  pinMode(status_led,OUTPUT); 
  pinMode(VOLPLUS, INPUT);
  pinMode(VOLMIN, INPUT);

  // main_menu();

  Serial.println(sender_check(8990));
}

void loop()
{
  digitalWrite(status_led ,LOW);
  AnVal = analogRead(A3); //Poti wird ausgelesen
  Serial.println(AnVal);
  NewFreq = map(AnVal, 0, 862, 8900, 10460);      // Mapping wird durchgeführt, analoger Wert auf die Frequenz umgerechnet
  radio.setFrequency(NewFreq);                    // Frequenz wird gesetzt
  update_display();                        // Neue Frequenz wird auf dem Display dargestellt
  OldAnVal = AnVal;

  if (digitalRead(VOLPLUS) == 0) {
    volume_change(1);
  }
  if (digitalRead(VOLMIN) == 0) {
    volume_change(-1);
  }
  
  //delay(500);
  //digitalWrite(status_led ,LOW);
  //delay(500);
  
}

//Mapping Funktion auf Float geändert
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

void drawchar(char* text, int size, int xpos, int ypos) {
  display.setTextColor(WHITE);
  display.setTextSize(size);
  display.setCursor(xpos,ypos);
  display.println(text);
  display.display();
  delay(1);
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

char * sender_check(int frequ) {
  int stations = sizeof(sender);
  for(int i = 1; i <= stations; i++) {
    if (frequ == frequenzen[i]) {
      return sender[i];
    } 
  }
  return "Kein Sender";
}
