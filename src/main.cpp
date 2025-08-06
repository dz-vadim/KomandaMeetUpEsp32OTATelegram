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
  lcdShow("Booting...", "FW " + String(FW_VERSION));

  telegramInit();
  setupWiFiWithPortal(deviceName);
  lcdShow("WiFi OK", WiFi.localIP().toString());

  sendTelegram("Device online. FW " + String(FW_VERSION) + 
               "\nIP: " + WiFi.localIP().toString() + 
               "\nSend /update");
  lcdShow("Ready", "v" + String(FW_VERSION));
}

void loop() {
  unsigned long now = millis();

  if (!updating && (now - lastPollMs > pollIntervalMs)) {
    lastPollMs = now;

    if (fetchTelegramAndCheckUpdateCmd()) {
      updating = true;
      sendTelegram("Starting OTA...");
      lcdShow("Updating...", "");
      if (!doOTAFromLatestRelease()) {
        updating = false;
      }
    }
  }
}
