/*
  TTGO T-Display True Type Font
  based on https://github.com/k-omura/truetype_Arduino

  "ESP32 Sketch Data Upload" before writing the program
  https://github.com/me-no-dev/arduino-esp32fs-plugin
*/

#include "FS.h"
#include "SPIFFS.h"
#include "truetype_Arduino.h"
#include <TFT_eSPI.h>

#define FORMAT_SPIFFS_IF_FAILED true

TFT_eSPI tft = TFT_eSPI(); 
uint8_t *framebuffer;

#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 132
//#define BitmapBufferSize 32400 //TFT ressolution 240*135 in TTGO T-display

truetypeClass truetype = truetypeClass();
File fontFile;

void setup() {
  Serial.begin(115200);

  // ----------
  //    TFT
  // ----------
  tft.begin();               // init TFT
  tft.setRotation(1);        // landscape
  tft.fillScreen(TFT_BLACK); // clear screen
  
  //https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts  
  tft.setFreeFont(&FreeSerif18pt7b);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(35, 30);
  tft.printf("Truetype Font");
  
  tft.setFreeFont(&FreeSerif12pt7b);
  tft.setCursor(5, 60);
  tft.printf("ghostyguo@gmail.com");
  
  //frameBuffer init
  framebuffer = (uint8_t *)malloc(DISPLAY_WIDTH * DISPLAY_HEIGHT); //1bit color
  if (!framebuffer) {
    Serial.println("alloc memory failed !!!");
    while (1);
  }
  Serial.println("memory allocated!!!");
  memset(framebuffer, 0x00, DISPLAY_WIDTH * DISPLAY_HEIGHT);  //1bit color
  Serial.println("memory cleared!!!");

  // ---------------
  //    True Type
  // ---------------
  Serial.setDebugOutput(true);

  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
      Serial.println("SPIFFS Mount Failed");
      return;
  }
  Serial.println("File system mounted");
  
  fontFile = SPIFFS.open("/times.ttf");
  if (!fontFile) {
    Serial.println("Error opening the file");
    return;
  }
  Serial.println(fontFile.name());
  Serial.println(fontFile.size());
  if (!truetype.setTtfFile(fontFile)) {
    Serial.println("read ttf failed");
    return;
  }
  Serial.println("read ttf OK");
  
  truetype.setFramebuffer(DISPLAY_WIDTH, DISPLAY_HEIGHT, 8, 0, framebuffer); //1bit, 
  truetype.setCharacterSize(32);
  truetype.setCharacterSpacing(0);
  //setTextBoundary(uint16_t _start_x, uint16_t _end_x, uint16_t _end_y)
  truetype.setTextBoundary(0,DISPLAY_WIDTH, DISPLAY_HEIGHT);
  truetype.setTextColor(0x01, 0x01); //Both fill & border are 1

  // Test for Draw True Type String
  truetype.textDraw(0, 0, "This is a Book");  

  tft.setSwapBytes(true); // hardware dependent settings

  // The following code is just to check the truetype decode result
  for (int y=0; y<32; y++) {    
      //Serial output for debug
      if (y<10) Serial.print(" ");
      Serial.print(y);
      Serial.print(": ");
    for (int x=0; x<DISPLAY_WIDTH; x++) {
      byte v=framebuffer[y*DISPLAY_WIDTH+x];
      Serial.print(v);
      if (v==1)
        tft.drawPixel(x, y+80, 0x001F ); //0x001F=blue
    }
    Serial.println();
  }
}

void loop() {
}
