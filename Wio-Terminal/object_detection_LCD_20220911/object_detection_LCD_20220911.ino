#include "Seeed_Arduino_GroveAI.h"
#include <Wire.h>
#include <TFT_eSPI.h>

TFT_eSPI tft;
TFT_eSprite spr = TFT_eSprite(&tft); 

uint8_t tft_len = 0;
int tft_time = 0;
uint8_t tft_confi[4] = { 0 };  // display confidence of max 4 objects only
uint8_t len_prev = 0;

uint16_t color_lora = TFT_LIGHTGREY; //TFT_LIGHTGREY; TFT_DARKCYAN
int cx_lora = 295;
int cy_lora = 24;

GroveAI ai(Wire);
uint8_t state = 0;
void setup() {
  Wire.begin();
  Serial.begin(115200);

  Serial.println("begin");
  if (ai.begin(ALGO_OBJECT_DETECTION, MODEL_EXT_INDEX_1))  // Object detection and externel model 1
  {
    Serial.print("Version: ");
    Serial.println(ai.version());
    Serial.print("ID: ");
    Serial.println(ai.id());
    Serial.print("Algo: ");
    Serial.println(ai.algo());
    Serial.print("Model: ");
    Serial.println(ai.model());
    Serial.print("Confidence: ");
    Serial.println(ai.confidence());
    state = 1;
  } else {
    Serial.println("Algo begin failed.");
  }

  tft.begin();
  tft.setRotation(3);

  //Head
  tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(&FreeSansBoldOblique12pt7b);
  tft.setTextColor(TFT_GREEN);
  tft.drawString("Plastic Bottle Checker", 5, 12, 1);

  //Line
  for (int8_t line_index = 0; line_index < 5; line_index++) {
    tft.drawLine(0, 50 + line_index, tft.width(), 50 + line_index, TFT_OLIVE);
  }


  tft.drawRoundRect(5, 60, (tft.width() / 2) - 20, tft.height() - 65, 10, TFT_WHITE);  // L1

  tft.setFreeFont(&FreeSansBoldOblique12pt7b);
  tft.setTextColor(TFT_GREENYELLOW);
  tft.drawString("Count", 7, 65, 1);
  tft.setTextColor(TFT_RED);
  tft.drawString("pcs", 60, 100, 1);

  tft.setFreeFont(&FreeSansBoldOblique12pt7b);
  tft.setTextColor(TFT_GREENYELLOW);
  tft.drawString("Time", 7, 150, 1);
  tft.setTextColor(TFT_RED);
  tft.drawString("ms", 70, 185, 1);

  tft.drawRoundRect((tft.width() / 2) - 10, 60, (tft.width() / 2) / 2, (tft.height() - 65) / 2, 10, TFT_BLUE);  // s1
  tft.setFreeFont(&FreeSansBoldOblique9pt7b);
  tft.setTextColor(TFT_GREENYELLOW);
  tft.drawString("Confi1", (tft.width() / 2) - 1, 70, 1);  
  tft.setTextColor(TFT_RED);
  tft.drawString("%", (tft.width() / 2) + 40, 100+3, 1);

  tft.drawRoundRect(((tft.width() / 2) + (tft.width() / 2) / 2) - 5, 60, (tft.width() / 2) / 2, (tft.height() - 65) / 2, 10, TFT_BLUE);  // s2
  tft.setFreeFont(&FreeSansBoldOblique9pt7b);
  tft.setTextColor(TFT_GREENYELLOW);
  tft.drawString("Confi2", ((tft.width() / 2) + (tft.width() / 2) / 2), 70, 1);  // Print the test text in the custom font
  tft.setTextColor(TFT_RED);
  tft.drawString("%", ((tft.width() / 2) + (tft.width() / 2) / 2) + 41, 100+3, 1);


  tft.drawRoundRect((tft.width() / 2) - 10, (tft.height() / 2) + 30, (tft.width() / 2) / 2, (tft.height() - 65) / 2, 10, TFT_BLUE);  // s3
  tft.setFreeFont(&FreeSansBoldOblique9pt7b);
  tft.setTextColor(TFT_GREENYELLOW);
  tft.drawString("Confi3", (tft.width() / 2) - 1, (tft.height() / 2) + 40, 1);  // Print the test text in the custom font
  tft.setTextColor(TFT_RED);
  tft.drawString("%", (tft.width() / 2) + 40, (tft.height() / 2) + 70, 1);

 
  tft.drawRoundRect(((tft.width() / 2) + (tft.width() / 2) / 2) - 5, (tft.height() / 2) + 30, (tft.width() / 2) / 2, (tft.height() - 65) / 2, 10, TFT_BLUE);  // s4
  tft.setFreeFont(&FreeSansBoldOblique9pt7b);
  tft.setTextColor(TFT_GREENYELLOW);
  tft.drawString("Confi4", ((tft.width() / 2) + (tft.width() / 2) / 2), (tft.height() / 2) + 40, 1);  // Print the test text in the custom font
  tft.setTextColor(TFT_RED);
  tft.drawString("%", ((tft.width() / 2) + (tft.width() / 2) / 2) + 41, (tft.height() / 2) + 70, 1);

  // Add lora icon at top left corner
  // -------------------------------------------------------------------
  tft.drawCircle(cx_lora,cy_lora,6, color_lora); //draw circle with border 
  tft.drawCircle(cx_lora,cy_lora,7, color_lora); //draw circle with border 
  tft.drawCircle(cx_lora,cy_lora,10, color_lora); //draw circle with border 
  tft.drawCircle(cx_lora,cy_lora,11, color_lora); //draw circle with border 
  tft.drawCircle(cx_lora,cy_lora,14, color_lora); //draw circle with border 
  tft.drawCircle(cx_lora,cy_lora,15, color_lora); //draw circle with border 

  tft.fillTriangle(cx_lora,cy_lora ,cx_lora+17,cy_lora-17 ,cx_lora-17,cy_lora-17 ,TFT_BLACK); //fill triangle with color
  tft.fillTriangle(cx_lora,cy_lora ,cx_lora+17,cy_lora+17 ,cx_lora-17,cy_lora+17 ,TFT_BLACK); //fill triangle with color

  tft.drawCircle(cx_lora,cy_lora,2, color_lora); //draw circle with border 
  tft.drawCircle(cx_lora,cy_lora,3, color_lora); //draw circle with border 

}

void loop() {
  if (state == 1) {
    uint32_t tick = millis();
    if (ai.invoke())  // begin invoke
    {
      while (1)  // wait for invoking finished
      {
        CMD_STATE_T ret = ai.state();
        if (ret == CMD_STATE_IDLE) {
          break;
        }
        delay(20);
      }

      uint8_t len = ai.get_result_len();  // receive how many detect
      
      if (len) {
        int time1 = millis() - tick;  
        tft_len = len;
        tft_time = time1;
        Serial.print("Time consuming: ");
        Serial.println(time1);
        Serial.print("Number of bottle: ");
        Serial.println(len);
        object_detection_t data;  //get data

        for (int i = 0; i < len; i++) {
          Serial.println("result:detected");
          Serial.print("Detecting and calculating: ");
          Serial.println(i + 1);
          ai.get_result(i, (uint8_t*)&data, sizeof(object_detection_t));  //get result

          Serial.print("confidence:");
          Serial.print(data.confidence);

          Serial.println();
          /*
          Serial.println(data.target);
          Serial.println(data.x);
          Serial.println(data.y);
          Serial.println(data.w);
          Serial.println(data.h);
          */
          if (i < 4) {
            tft_confi[i] = data.confidence;
          }
          if( (len_prev - len) > 0){
            for(int i = len; i < len_prev; i++){
              tft_confi[i] = 0;
            }
          }
          len_prev = len;
        }
      } else {
        Serial.println("No identification");
        tft_len = 0;
        tft_time = 0;
        len_prev = 0;
        for (int i = 0; i < 4; i++) {
          tft_confi[i] = 0;
        }
      }

      spr.createSprite(40, 30);
      spr.fillSprite(TFT_BLACK);
      spr.setFreeFont(&FreeSansBoldOblique12pt7b);
      spr.setTextColor(TFT_WHITE);
      spr.drawNumber(tft_len, 0, 0, 1);  // Length value
      spr.pushSprite(15, 100);
      spr.deleteSprite();

      spr.createSprite(40, 30);
      spr.setFreeFont(&FreeSansBoldOblique12pt7b);
      spr.setTextColor(TFT_WHITE);
      spr.drawNumber(tft_time, 0, 0, 1);  // Time value
      spr.pushSprite(15, 185);
      spr.deleteSprite();

      spr.createSprite(30, 30);
      spr.setFreeFont(&FreeSansBoldOblique12pt7b);
      spr.setTextColor(TFT_WHITE);
      spr.drawNumber(tft_confi[0], 0, 0, 1);  // Confi1
      spr.pushSprite((tft.width() / 2) - 1, 100);
      spr.deleteSprite();

      spr.createSprite(30, 30);
      spr.setFreeFont(&FreeSansBoldOblique12pt7b);
      spr.setTextColor(TFT_WHITE);
      spr.drawNumber(tft_confi[1], 0, 0, 1);  // Confi2
      spr.pushSprite(((tft.width() / 2) + (tft.width() / 2) / 2), 100);
      spr.deleteSprite();

      spr.createSprite(30, 30);
      spr.setFreeFont(&FreeSansBoldOblique12pt7b);
      spr.setTextColor(TFT_WHITE);
      spr.drawNumber(tft_confi[2], 0, 0, 1);  // Confi3
      spr.pushSprite((tft.width() / 2) - 1, (tft.height() / 2) + 67);
      spr.deleteSprite();

      spr.createSprite(30, 30);
      spr.setFreeFont(&FreeSansBoldOblique12pt7b);
      spr.setTextColor(TFT_WHITE);
      spr.drawNumber(tft_confi[3], 0, 0, 1);  // Confi4
      spr.pushSprite(((tft.width() / 2) + (tft.width() / 2) / 2), (tft.height() / 2) + 67);
      spr.deleteSprite();
    } else {
      delay(1000);
      Serial.println("Invoke Failed.");
    }
  } else {
    state == 0;
  }
}