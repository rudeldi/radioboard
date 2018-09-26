
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

  //radio.setVolume(2); //momentan keine Funktion

  // main_menu();
  
}

void loop()
{
  digitalWrite(status_led ,LOW);
  AnVal = analogRead(A3); //Poti wird ausgelesen
  if (OldAnVal != AnVal) {
    NewFreq = map(AnVal, 0, 862, 8900, 10460);      // Mapping wird durchgeführt, analoger Wert auf die Frequenz umgerechnet
    radio.setFrequency(NewFreq);                    // Frequenz wird gesetzt
    update_display();                        // Neue Frequenz wird auf dem Display dargestellt
    OldAnVal = AnVal;
  }
  
  if (digitalRead(VOLPLUS) == 0) {
    volume_increase(5);
  }
  if (digitalRead(VOLMIN) == 0) {
    volume_decrease(5);
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
  display.drawRect(0, 25, 104, 6, 1);
  display.drawFastHLine(2, 27, volume, 1);
  display.drawFastHLine(2, 28, volume, 1);
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

void main_menu() {

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(0,0);
  display.println("Radio");
  display.setTextSize(1);
  display.setCursor(0,20);
  display.println("MAKER COMMUNITY");
  display.drawFastHLine(0, 30, 100, 1);
  display.display();
  delay(2000);
  
}

void volume_increase(int steps) {
  if (volume + steps > 100) {
  volume = 100;
  } else {
    volume += steps;
  }
  Serial.println("VOL +");
  update_display();
  delay(50);
}

void volume_decrease(int steps) {
  if (volume - steps < 0) {
    volume = 0;
  } else {
    volume -= steps;
  }
  Serial.println("VOL -");
  update_display();
  delay(50);
}
