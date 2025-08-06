#include <Arduino.h>
#include <WiFi.h>
#include "lcd_display.h"
#include "wifi_setup.h"
#include "telegram_api.h"
#include "ota_update.h"

#ifndef FW_VERSION
#define FW_VERSION "local"
#endif

static String deviceName = "ESP32-OTA";
static unsigned long lastPollMs = 0;
static const unsigned long pollIntervalMs = 2000;
static bool updating = false;

void setup() {
  Serial.begin(115200);

  lcdInit();
  lcdShow(String(F("Booting...")), String(F("FW ")) + FW_VERSION);

  telegramInit();
  setupWiFiWithPortal(deviceName);
  lcdShow(String(F("WiFi OK")), WiFi.localIP().toString());

  sendTelegram(String(F("Device online. FW ")) + FW_VERSION +
               String(F("\nIP: ")) + WiFi.localIP().toString() +
               String(F("\nSend /update")));
  lcdShow(String(F("Ready")), String(F("v")) + FW_VERSION);
}

void loop() {
  unsigned long now = millis();

  if (!updating && (now - lastPollMs > pollIntervalMs)) {
    lastPollMs = now;

    if (fetchTelegramAndCheckUpdateCmd()) {
      updating = true;
      sendTelegram(F("Starting OTA..."));
      lcdShow(String(F("Updating...")), String());
      if (!doOTAFromLatestRelease()) {
        updating = false; // якщо оновлення не сталось — продовжуємо опитування
      }
    }
  }
}
