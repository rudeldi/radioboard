#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Display initialisieren
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

const int bmax = 128;
const int hmax = 32;


void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);          //  setup serial
  //Display initialisieren
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  Serial.println("Display initialisiert...");

  lines();
  circles();
  pixels();
  

}

void loop() {
  // put your main code here, to run repeatedly:

}

void circles() {
  display.clearDisplay();
  boolean draw_mode = 1;
  for(int j = 1; j <= 4; j++) {
    for(int i = 1; i <= 5; i++) {
    display.drawCircle(bmax/2,hmax/2,3*i,draw_mode);
    display.display();
    delay(100);
    }
    draw_mode = !draw_mode;
  }
  delay(5000);
}

void pixels() {
  display.clearDisplay();
  for (int i = 1; i <= 50; i++) {
    int x = random(128);
    int y = random(32);
    display.drawPixel(x,y,1);
    display.display();
  }
}

void lines() {
  display.clearDisplay();
  for (int i = 1; i <= 100; i++) {
    int x0 = random(128);
    int y0 = random(32);
    int x1 = x0 + random(20)-10;
    int y1 = y0 + random(20)-10;
    display.drawLine(x0,y0,x1,y1,1);
    display.display();
  }  
}

