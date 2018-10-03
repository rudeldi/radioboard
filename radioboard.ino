
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
Adafruit_SSD1306 display(OLED_RESET);

 
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

//Status-LED initialisieren
int status_led = 7;

// Lautstärketasten initialisieren
#define VOLPLUS 2
#define VOLMIN 3
int volume = 0;
const int max_vol = 15;
const int min_vol = 0;
int radio_mode = 1;
int dim_status = 0;


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
 
  // Pin Modes Lautstärketasten und Status LED
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

  // Display löschen
  start_display();

}

void loop()
{
  digitalWrite(status_led ,LOW);
  AnVal = analogRead(A7);                           //Poti wird ausgelesen
  Serial.println(AnVal);
  NewFreq = map(AnVal, 0, 1023, 8900, 10800);       // Mapping wird durchgeführt, analoger Wert auf die Frequenz umgerechnet
  NewFreq = 5*round((float)NewFreq/5);              // Wert wird auf .05 gerundet
  radio.setFrequency(NewFreq);                      // Frequenz wird gesetzt
  if (radio_mode == 1) {
    update_frequency();                             // Neue Frequenz wird auf dem Display dargestellt
  }

  // Laustärketasten checken
  if ((digitalRead(VOLPLUS) == 0) & (digitalRead(VOLMIN) == 0)) {
    radio_mode = 2;
    update_display();
  } else if (digitalRead(VOLPLUS) == 0) {
    radio_mode = 1;
    volume_change(1);
  } else if (digitalRead(VOLMIN) == 0) {
    radio_mode = 1;
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
  display.clearDisplay();
  if (radio_mode == 1) {
    currentTime = millis();
    float x = (float)NewFreq/100;  
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(0,0);
    display.println(x);
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
  } else if (radio_mode == 2) {                    // Sendersuchlauf
      display.setTextSize(2);
      display.setCursor(0,0);
      display.println("Suchlauf");
      display.setTextSize(1);
      display.setCursor(0,23);
      display.println("> > > > > > > > > > > >");
  }
  display.display();
}

void update_frequency() {
  float x = (float)NewFreq/100;
  display.fillRect(0,0,100,20,0);
  display.setTextSize(2);
  display.setCursor(0,0);
  display.println(x);
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
  if(dim_status == 0) {
    display.println("Display \ngedimmt");
    display.dim(1);
    dim_status = 1;
    delay(500);
    update_display();
  } else if (dim_status == 1) {
    display.println("Display \nnormal");
    display.dim(0);
    dim_status = 0;
    delay(500);
    update_display();
  }
  display.display();
  delay(500);  
}

void start_display() {
  display.clearDisplay();
  display.display();
}
