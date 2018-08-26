// TEA5767 Example
#include <SPI.h>
#include <Wire.h>
#include <TEA5767.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Display initialisierung
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
int AnVal ;
int NewFreq ;

//Zusatz ini
int status_led = 7;

const int volButtonplus = 3;
const int volButtonminu = 2;

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
 

  //Status LED Initialisieren
  pinMode(status_led,OUTPUT); 
  pinMode(volButtonplus, INPUT);
  pinMode(volButtonminu, INPUT);
  
  //radio.setVolume(2); //momentan keine Funktion
  
}

void loop()
{
  digitalWrite(status_led ,LOW);
  AnVal = analogRead(A3); //Poti wird ausgelesen
  NewFreq = map(AnVal, 0, 862, 8900, 10460); //Mapping wird durchgeführt, Analoger Wert auf die Frequenz umgerechnet
  radio.setFrequency(NewFreq); //Frequenz wird gesetzt
  show_frequenz(NewFreq); //Neue Frequenz wird auf dem Display dargestellt
  
  Serial.println(AnVal);
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
void show_frequenz(float x){
  x = x/100;
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(4);
  display.setCursor(0,0);
  display.println(x);
  display.display();
}

void volume_set(){
  
}

