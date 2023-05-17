#include <Arduino.h>
#include <M5Atom.h>
#include <M5UnitGLASS.h>
#include "Unit_Encoder.h"
#include "DFRobot_LIDAR07.h"

M5UnitGLASS display;
LGFX_Sprite sprite(&display);
LGFX_Sprite spriteNum(&display);
DFRobot_LIDAR07_IIC LIDAR07(Wire);
Unit_Encoder encoder;

int width, height;
float pitch, roll, yaw;
uint16_t distance = 0;
int encoderOffset = 0;
bool canMeasure = true;

SemaphoreHandle_t binarySemaphore;
TaskHandle_t thp[2];
void startScreen(void *pvParameters) {

  sprite.fillScreen(TFT_BLACK);

  for (int i = 0; i < 10; i++) {
    sprite.drawLine(0, 0, i, 0, TFT_WHITE);
    sprite.drawLine(0, height * 0.2, i, height * 0.2, TFT_WHITE);
    sprite.drawLine(0, height * 0.4, i, height * 0.4, TFT_WHITE);
    sprite.drawLine(0, height * 0.6, i, height * 0.6, TFT_WHITE);
    sprite.drawLine(0, height * 0.8, i, height * 0.8, TFT_WHITE);
    sprite.drawLine(0, height, i, height, TFT_WHITE);

    sprite.drawLine(width, 0, width - i, 0, TFT_WHITE);
    sprite.drawLine(width, height * 0.2, width - i, height * 0.2, TFT_WHITE);
    sprite.drawLine(width, height * 0.4, width - i, height * 0.4, TFT_WHITE);
    sprite.drawLine(width, height * 0.6, width - i, height * 0.6, TFT_WHITE);
    sprite.drawLine(width, height * 0.8, width - i, height * 0.8, TFT_WHITE);
    sprite.drawLine(width, height, width, height, TFT_WHITE);
    sprite.pushSprite(0, 0);
    delay(20);
  }

  // 中央のロール計(pitch)

  int sightX = width / 2;
  int sightY = height / 2;

  for (int i = 0; i < 10; i++) {
    sprite.drawLine(sightX, sightY, sightX, sightY + i * 0.5, TFT_WHITE);
    sprite.drawLine(sightX, sightY, sightX, sightY - i * 0.5, TFT_WHITE);
    sprite.drawLine(sightX, sightY, sightX - i * 1, sightY, TFT_WHITE);
    sprite.drawLine(sightX, sightY, sightX + i * 1, sightY, TFT_WHITE);
    sprite.pushSprite(0, 0);
    delay(20);
  }
  // sprite.setTextFont(3);
  // sprite.setTextSize(1);
  // sprite.setCursor(width * 0.5, height * 0.8);
  // sprite.printf("%4.1f", 99.9);

  // sprite.pushSprite(0, 0);

  spriteNum.setTextSize(1);
  spriteNum.setCursor(0, 0);
  spriteNum.setFont(&fonts::Font7);
  spriteNum.print("8");
  spriteNum.pushRotateZoomWithAA(&sprite, width * 0.9, height * 0.02, 0, 0.35, 0.35);
  sprite.pushSprite(0, 0);
  delay(20);
  spriteNum.print("8");
  spriteNum.pushRotateZoomWithAA(&sprite, width * 0.9, height * 0.02, 0, 0.35, 0.35);
  sprite.pushSprite(0, 0);
  delay(20);
  spriteNum.print(".");
  spriteNum.pushRotateZoomWithAA(&sprite, width * 0.9, height * 0.02, 0, 0.35, 0.35);
  sprite.pushSprite(0, 0);
  delay(20);
  spriteNum.print("8");
  spriteNum.pushRotateZoomWithAA(&sprite, width * 0.9, height * 0.02, 0, 0.35, 0.35);
  sprite.pushSprite(0, 0);
  delay(20);
  spriteNum.setCursor(0, 0);

  spriteNum.pushRotateZoomWithAA(&sprite, width * 0.9, height * 0.02, 0, 0.35, 0.35);
  sprite.setCursor(width * 0.85, height * 0.05);
  sprite.setFont(&fonts::Font2);
  sprite.print("m");
  sprite.pushSprite(0, 0);

  vTaskDelete(NULL);
}
void measure(void *pvParameters) {

  while (true) {
    // if (pdTRUE == xSemaphoreTake(binarySemaphore, 100)) {

    // // delay(50);
    M5.IMU.getAhrsData(&pitch, &roll, &yaw);
    Serial.printf("p:%5.1f, r:%5.1f, y:%5.1f, d:%d, encOff:%d\n", pitch, roll, yaw, distance, encoderOffset);
    // xSemaphoreGive(binarySemaphore);
    // }
    delay(10);
  }
}

void drawDisplay(void *pvParameters) {
  while (true) {
    // if (pdTRUE == xSemaphoreTake(binarySemaphore, 200)) {

    if (canMeasure) {

      LIDAR07.startMeasure();
      if (LIDAR07.getValue()) {
        distance = LIDAR07.getDistanceMM();
        // Serial.printf("Distance:%6dmm ", LIDAR07.getDistanceMM());
      }
    }
    int shift = (int)(roll) % (height / 5);

    // int shift = 0;

    sprite.fillScreen(TFT_BLACK);
    sprite.drawLine(0, shift, 10, shift, TFT_WHITE);
    sprite.drawLine(0, height * 0.2 + shift, 10, height * 0.2 + shift, TFT_WHITE);
    sprite.drawLine(0, height * 0.4 + shift, 10, height * 0.4 + shift, TFT_WHITE);
    sprite.drawLine(0, height * 0.6 + shift, 10, height * 0.6 + shift, TFT_WHITE);
    sprite.drawLine(0, height * 0.8 + shift, 10, height * 0.8 + shift, TFT_WHITE);
    sprite.drawLine(0, height + shift, 10, height + shift, TFT_WHITE);

    sprite.drawLine(width - 10, shift, width, shift, TFT_WHITE);
    sprite.drawLine(width - 10, height * 0.2 + shift, width, height * 0.2 + shift, TFT_WHITE);
    sprite.drawLine(width - 10, height * 0.4 + shift, width, height * 0.4 + shift, TFT_WHITE);
    sprite.drawLine(width - 10, height * 0.6 + shift, width, height * 0.6 + shift, TFT_WHITE);
    sprite.drawLine(width - 10, height * 0.8 + shift, width, height * 0.8 + shift, TFT_WHITE);
    sprite.drawLine(width - 10, height + shift, width, height + shift, TFT_WHITE);

    if (!encoder.getButtonStatus()) {
      encoderOffset = encoder.getEncoderValue();
      Serial.printf("encoderOffset:%d\n", encoderOffset);
    }
    int y = 1.0 * pow((float)distance, 2) * 0.00001;

    // int distanceShift = 0;

    int distanceShift = map(y, 0, 250 - (encoder.getEncoderValue() - encoderOffset), 0, -height * 0.7);

    // int distanceShift = (encoder.getEncoderValue() - encoderOffset);
    int sightX = width / 2;
    int sightY = height * 0.9;
    // sprite.drawLine(sightX, sightY, sightX - 15, sightY, TFT_WHITE);
    // sprite.drawLine(sightX, sightY, sightX, sightY - 5, TFT_WHITE);
    // sprite.drawLine(sightX, sightY, sightX, sightY + 5, TFT_WHITE);
    // sprite.drawLine(sightX, sightY, sightX + 15, sightY, TFT_WHITE);
    sprite.drawLine(sightX, sightY + distanceShift, sightX - 10 * cos(pitch * DEG_TO_RAD), sightY + 5 * sin(pitch * DEG_TO_RAD) + distanceShift, TFT_WHITE);
    sprite.drawLine(sightX, sightY + distanceShift, sightX + 10 * cos(pitch * DEG_TO_RAD), sightY - 5 * sin(pitch * DEG_TO_RAD) + distanceShift, TFT_WHITE);
    sprite.drawLine(sightX, sightY + distanceShift, sightX - 10 * sin(pitch * DEG_TO_RAD), sightY - 5 * cos(pitch * DEG_TO_RAD) + distanceShift, TFT_WHITE);
    sprite.drawLine(sightX, sightY + distanceShift, sightX + 10 * sin(pitch * DEG_TO_RAD), sightY + 5 * cos(pitch * DEG_TO_RAD) + distanceShift, TFT_WHITE);

    spriteNum.setTextSize(1);
    spriteNum.setCursor(0, 0);
    spriteNum.setFont(&fonts::Font7);
    spriteNum.printf("%04.1f", (float)distance / 1000);

    // spriteNum.pushRotateZoomWithAA(&sprite, width * 0.9, height * 0.15, 0, 0.3, 0.3);
    spriteNum.pushRotateZoomWithAA(&sprite, width * 0.9, height * 0.02, 0, 0.35, 0.35);
    spriteNum.setTextSize(0.5);
    sprite.setCursor(width * 0.85, height * 0.05);
    sprite.setFont(&fonts::Font2);
    sprite.print("m");
    sprite.setCursor(0, 0);
    sprite.print(canMeasure ? "M" : "H");

    sprite.pushSprite(0, 0);
    // xSemaphoreGive(binarySemaphore);
  }
  delay(100);
  // }
}
void setup(void) {

  // binarySemaphore = xSemaphoreCreateBinary();
  Wire.begin(26, 32);
  M5.begin(true, false, false);
  display.init(GPIO_NUM_26, GPIO_NUM_32, 400000u);
  M5.IMU.Init();
  LIDAR07.begin();
  uint32_t version = LIDAR07.getVersion();
  Serial.print("VERSION: ");
  Serial.print((version >> 24) & 0xFF, HEX);
  Serial.print(".");
  Serial.print((version >> 16) & 0xFF, HEX);
  Serial.print(".");
  Serial.print((version >> 8) & 0xFF, HEX);
  Serial.print(".");
  Serial.println((version)&0xFF, HEX);
  LIDAR07.startFilter();
  encoder.begin();

  // initialize
  // 初期化
  //  SDA, SCL, Freq, I2C PortNumber

  // Set the display orientation
  // ディスプレイの向きを設定
  // 0 = 270 deg / 1 = normal / 2 = 90 deg / 3 = 180 deg / 4~7 = upside down
  display.setRotation(2);
  width = display.width();
  height = display.height();
  Serial.printf("width=%d,height=%d\n", width, height);

  sprite.createSprite(width, height);
  spriteNum.createSprite(width * 2, height / 2);
  spriteNum.setCursor(0, 0);
  spriteNum.setPivot(width * 2, 0);
  // display.startWrite();
  // for (int y = 0; y < display.height(); ++y) {
  //   for (int x = 0; x < display.width(); ++x) {
  //     display.drawPixel(x, y, display.color888(x * 2, x * 2 + y * 2, y * 2));
  //   }
  // }
  // display.endWrite();

  // for (int i = 0; i < 16; ++i) {
  //   int x = rand() % display.width();
  //   int y = rand() % display.height();
  //   display.drawCircle(x, y, 16, rand());
  // }

  xTaskCreateUniversal(
      startScreen,
      "startScreen",
      8192,
      NULL,
      1,
      NULL,
      APP_CPU_NUM);
  // After enabling the filter, it can be stopped by
  // calling LIDAR07.stopFilter()

  unsigned long start = millis();
  // 5秒間空読みして収束させる
  while (millis() - start < 5000) {
    M5.IMU.getAhrsData(&pitch, &roll, &yaw);
    delay(10);
  }

  // for (int i = 0; i < 100; i++) {
  //   M5.IMU.getAhrsData(&pitch, &roll, &yaw);
  //   delay(50);
  // }
  encoderOffset = encoder.getEncoderValue();

  xTaskCreateUniversal(
      measure,
      "measure",
      8192,
      NULL,
      1,
      &thp[0],
      APP_CPU_NUM);
  xTaskCreateUniversal(
      drawDisplay,
      "drawDisplay",
      8192,
      NULL,
      1,
      &thp[1],
      APP_CPU_NUM);
}

void loop(void) {
  M5.update();
  if (M5.Btn.wasReleased()) {
    canMeasure = !canMeasure;
  }

  // M5.IMU.getAhrsData(&pitch, &roll, &yaw);
  // Serial.printf("pitch:%5.1f, roll:%5.1f, yaw:%5.1f\n", pitch, roll, yaw);

  // display.drawLine(-10, -10, 50, 50);
  // static const char test[] = "test";
  // display.drawString(test, 0, 0);
  // display.drawLine(-10, -10, 50, 50);  }
  // int pd = pdFALSE;
  // xSemaphoreGiveFromISR(binarySemaphore, &pd);
  delay(100);
}