// Test to display light sensor value & LoRa status in LCD

#include <TFT_eSPI.h>
#include "disk91_LoRaE5.h"

Disk91_LoRaE5 lorae5(&Serial);  // Where the AT command and debut traces are printed
#define Frequency DSKLORAE5_ZONE_AS923_1
char deveui[] = "<your deveui>";
char appeui[] = "<your appeui>";
char appkey[] = "<your appkey>";

TFT_eSPI tft;
TFT_eSprite spr = TFT_eSprite(&tft); 

uint8_t tft_len = 0;
int tft_time = 0;
uint8_t tft_confi[4] = { 0 };  // display confidence of max 4 objects only
uint8_t len_prev = 0;

uint16_t color_lora = TFT_LIGHTGREY;  //TFT_LIGHTGREY; TFT_DARKCYAN
int cx_lora = 295;
int cy_lora = 24;

bool lora_flag = 0;

uint8_t tft_UplinkSent = 0;
uint8_t tft_DownlinkReceived = 0;

void data_decord(int val, uint8_t data[2]) {
  data[0] = val >> 8 & 0xFF;
  data[1] = val & 0xFF;
}

void lora_icon(bool lora_flag) {

  if (lora_flag) {
    color_lora = TFT_DARKCYAN;
  } else {
    color_lora = TFT_LIGHTGREY;
  }
  tft.drawCircle(cx_lora, cy_lora, 6, color_lora);   //draw circle with border
  tft.drawCircle(cx_lora, cy_lora, 7, color_lora);   //draw circle with border
  tft.drawCircle(cx_lora, cy_lora, 10, color_lora);  //draw circle with border
  tft.drawCircle(cx_lora, cy_lora, 11, color_lora);  //draw circle with border
  tft.drawCircle(cx_lora, cy_lora, 14, color_lora);  //draw circle with border
  tft.drawCircle(cx_lora, cy_lora, 15, color_lora);  //draw circle with border

  tft.fillTriangle(cx_lora, cy_lora, cx_lora + 17, cy_lora - 17, cx_lora - 17, cy_lora - 17, TFT_BLACK);  //fill triangle with color
  tft.fillTriangle(cx_lora, cy_lora, cx_lora + 17, cy_lora + 17, cx_lora - 17, cy_lora + 17, TFT_BLACK);  //fill triangle with color

  if (lora_flag) {
    tft.fillCircle(cx_lora, cy_lora, 2, color_lora);  //draw circle with border
    tft.fillCircle(cx_lora, cy_lora, 3, color_lora);  //draw circle with border
  } else {
    tft.drawCircle(cx_lora, cy_lora, 2, color_lora);  //draw circle with border
    tft.drawCircle(cx_lora, cy_lora, 3, color_lora);  //draw circle with border
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("begin");

  tft.begin();
  tft.setRotation(3);

  //Head
  tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(&FreeSansBoldOblique12pt7b);
  tft.setTextColor(TFT_GREEN);
  tft.drawString("Light & LoRa Test", 5, 12, 1);

  for (int8_t line_index = 0; line_index < 5; line_index++) {
    tft.drawLine(0, 50 + line_index, tft.width(), 50 + line_index, TFT_OLIVE);
  }


  tft.drawRoundRect(5, 60, (tft.width() / 2) - 20, tft.height() - 65, 10, TFT_WHITE);  // L1

  tft.setFreeFont(&FreeSansBoldOblique12pt7b);
  tft.setTextColor(TFT_GREENYELLOW);
  tft.drawString("Light", 7, 65, 1);
  tft.setTextColor(TFT_RED);
  tft.drawString("raw", 60, 100, 1);

  tft.setFreeFont(&FreeSansBoldOblique12pt7b);
  tft.setTextColor(TFT_GREENYELLOW);
  tft.drawString("Spare0", 7, 150, 1);
  tft.setTextColor(TFT_RED);
  tft.drawString("...", 70, 185, 1);

  tft.drawRoundRect((tft.width() / 2) - 10, 60, (tft.width() / 2) / 2, (tft.height() - 65) / 2, 10, TFT_BLUE);  // s1
  tft.setFreeFont(&FreeSansBoldOblique9pt7b);
  tft.setTextColor(TFT_GREENYELLOW);
  tft.drawString("Uplink", (tft.width() / 2) - 1, 70, 1);  
  tft.setTextColor(TFT_RED);
  tft.drawString("...", (tft.width() / 2) + 40, 100 + 3, 1);

  tft.drawRoundRect(((tft.width() / 2) + (tft.width() / 2) / 2) - 5, 60, (tft.width() / 2) / 2, (tft.height() - 65) / 2, 10, TFT_BLUE);  // s2
  tft.setFreeFont(&FreeSansBoldOblique9pt7b);
  tft.setTextColor(TFT_GREENYELLOW);
  tft.drawString("Dolink", ((tft.width() / 2) + (tft.width() / 2) / 2), 70, 1);  
  tft.setTextColor(TFT_RED);
  tft.drawString("...", ((tft.width() / 2) + (tft.width() / 2) / 2) + 41, 100 + 3, 1);


  tft.drawRoundRect((tft.width() / 2) - 10, (tft.height() / 2) + 30, (tft.width() / 2) / 2, (tft.height() - 65) / 2, 10, TFT_BLUE);  // s3
  tft.setFreeFont(&FreeSansBoldOblique9pt7b);
  tft.setTextColor(TFT_GREENYELLOW);
  tft.drawString("Spare3", (tft.width() / 2) - 1, (tft.height() / 2) + 40, 1); 
  tft.setTextColor(TFT_RED);
  tft.drawString("...", (tft.width() / 2) + 40, (tft.height() / 2) + 70, 1);


  tft.drawRoundRect(((tft.width() / 2) + (tft.width() / 2) / 2) - 5, (tft.height() / 2) + 30, (tft.width() / 2) / 2, (tft.height() - 65) / 2, 10, TFT_BLUE);  // s4
  tft.setFreeFont(&FreeSansBoldOblique9pt7b);
  tft.setTextColor(TFT_GREENYELLOW);
  tft.drawString("Spare4", ((tft.width() / 2) + (tft.width() / 2) / 2), (tft.height() / 2) + 40, 1);  
  tft.setTextColor(TFT_RED);
  tft.drawString("...", ((tft.width() / 2) + (tft.width() / 2) / 2) + 41, (tft.height() / 2) + 70, 1);

  // Add lora icon at top left corner
  lora_icon(false);

  if (!lorae5.begin(DSKLORAE5_SEARCH_WIO)) {
    Serial.println("LoRa E5 Init Failed");
    while (1)
      ;
  }
  // Setup the LoRaWan Credentials
  if (!lorae5.setup(
        Frequency,
        deveui,
        appeui,
        appkey)) {
    Serial.println("LoRa E5 Setup Failed");
    while (1)
      ;
  }
}

void loop() {

  int light = analogRead(WIO_LIGHT);  //Get the Wio Terminal light value.
  // Display light value on LCD.
  spr.createSprite(40, 30);
  spr.fillSprite(TFT_BLACK);
  spr.setFreeFont(&FreeSansBoldOblique12pt7b);
  spr.setTextColor(TFT_WHITE);
  spr.drawNumber(light, 0, 0, 1);  // light value
  spr.pushSprite(15, 100);
  spr.deleteSprite();
  Serial.print("Light Value: ");
  Serial.println(light);

  static uint8_t data[2] = { 0x00 };  //Use the data[] to store the values of the sensors
  data_decord(light, data);
  if (lorae5.send_sync(  //Sending the sensor values out
        8,               // LoRaWan Port
        data,            // data array
        sizeof(data),    // size of the data
        false,           // we are not expecting a ack
        7,               // Spread Factor
        14               // Tx Power in dBm
        )) {
    Serial.println("Uplink done");

    // Update lora icon at top left corner
    // -------------------------------------------------------------------
    lora_icon(true);
    tft_UplinkSent = 1;

    if (lorae5.isDownlinkReceived()) {
      Serial.println("A downlink has been received");
      tft_DownlinkReceived = 1;
      if (lorae5.isDownlinkPending()) {
        Serial.println("More downlink are pending");
      }
    } else {
      tft_DownlinkReceived = 0;
    }
  } else {
    tft_UplinkSent = 0;
  }
  spr.createSprite(30, 30);
  spr.setFreeFont(&FreeSansBoldOblique12pt7b);
  spr.setTextColor(TFT_WHITE);
  spr.drawNumber(tft_UplinkSent, 0, 0, 1);  // Confi1
  spr.pushSprite((tft.width() / 2) - 1, 100);
  spr.deleteSprite();

  spr.createSprite(30, 30);
  spr.setFreeFont(&FreeSansBoldOblique12pt7b);
  spr.setTextColor(TFT_WHITE);
  spr.drawNumber(tft_DownlinkReceived, 0, 0, 1);  // Confi2
  spr.pushSprite(((tft.width() / 2) + (tft.width() / 2) / 2), 100);
  spr.deleteSprite();

  delay(15000);
}