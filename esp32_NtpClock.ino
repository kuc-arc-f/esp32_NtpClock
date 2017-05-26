#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

// For the ESP-WROVER_KIT, these are the default.
#define TFT_CS   22
#define TFT_DC   21
#define TFT_MOSI 23
#define TFT_CLK  19
#define TFT_RST  18
#define TFT_MISO 25
#define TFT_LED   5  // GPIO not managed by library

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
//
uint32_t mTimerNTP=0;
//
void backlighting(bool state) {
    if (state) {
        digitalWrite(TFT_LED, LOW);
    }
    else {
        digitalWrite(TFT_LED, HIGH);
    }
}
//
void setup() {
  Serial.begin(115200);
  Serial.println("ILI9341 Test!"); 
  tft.begin();
  pinMode(TFT_LED, OUTPUT);
  backlighting(true);

  // read diagnostics (optional but can help debug problems)
  uint8_t x = tft.readcommand8(ILI9341_RDMODE);
  Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDMADCTL);
  Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDPIXFMT);
  Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDIMGFMT);
  Serial.print("Image Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDSELFDIAG);
  Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX);

  uint8_t rotation= 3;
  tft.setRotation(rotation);
  //tft.fillScreen(ILI9341_WHITE);
  //
  Serial.println(F("Done!") );

  //udp
  init_udpClient();
}
//unsigned long testText(String sTime) {
unsigned long testText(String sTime ,String sDate) {
  tft.fillScreen(ILI9341_WHITE);
  unsigned long start = micros();
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(20, 20);
  tft.setTextSize(2);
  tft.println("IoT Clock, v0.9.1"   );
  //
  tft.setTextSize(5);
  tft.println("");
  tft.println(" " +sTime );
  tft.setTextSize(4);
  tft.println("");
  tft.setTextSize(4 );
//  tft.setTextColor(ILI9341_BLUE );
  tft.println( " " + sDate);
  tft.setTextColor(ILI9341_BLACK);
  return micros() - start;
}
uint32_t mTimer=0;
uint32_t mTimerDisp=0;
//
void loop(void) {
    if(millis() >mTimerDisp ){
        mTimerDisp=millis() + (10 * 1000) ;
        testText(get_mCTime(),  get_mCDate()  );
    }
    if(millis() > mTimerNTP){
        mTimerNTP= mTimerNTP + ( 600 * 1000);
        exec_syncNtp();
    }
    if(millis() >mTimer ){
        mTimer=millis() + 1000;
        time_display();
    }
    // Serial.println("#date="+ get_mCDate() );
    //delay(1000 );
}

