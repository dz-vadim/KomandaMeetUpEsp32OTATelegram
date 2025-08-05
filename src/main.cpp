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
static const unsigned long pollIntervalMs = 2000; // 2s, швидка реакція
static bool updating = false;

void setup() {
  Serial.begin(115200);

  lcdInit();
  showBoot();

  telegramInit();        // підготує NVS/offset
  setupWiFiWithPortal(deviceName);
  showWiFiOK();

  drainOldTelegramUpdatesOnce(); // "з'їдаємо" старі апдейти один раз

  sendTelegram("✅ Device online. FW v" + String(FW_VERSION) + 
               "\nWiFi: " + String(WiFi.SSID()) +
               "\nIP: " + WiFi.localIP().toString() + 
               "\nSend /info or /update");
  showReady();
}

void loop() {
  unsigned long now = millis();

  if (!updating && (now - lastPollMs > pollIntervalMs)) {
    lastPollMs = now;

    // 1) Перевіряємо команди
    bool shouldUpdate = fetchTelegramAndCheckUpdateCmd();

    // 2) /info — відповідаємо миттєво і показуємо на LCD
    if (isInfoRequested()) {
      String infoMsg = "📟 ESP32 Info:\n";
      infoMsg += "FW: " + String(FW_VERSION) + "\n";
      infoMsg += "WiFi: " + String(WiFi.SSID()) + "\n";
      infoMsg += "IP: " + WiFi.localIP().toString() + "\n";
      infoMsg += "RSSI: " + String(WiFi.RSSI()) + " dBm";
      sendTelegram(infoMsg);

      // Стиснутий варіант на LCD (влізає у 16 символів)
      String l1 = "WiFi:" + String(WiFi.SSID()).substring(0, 10);
      String l2 = "IP:" + WiFi.localIP().toString();
      lcdPrint2(l1, l2);
    }

    // 3) /update — запускаємо OTA
    if (shouldUpdate) {
      updating = true; // блокуємо повторний запуск
      sendTelegram("⏳ Update command received. Starting OTA...");
      showUpdating();
      if (!doOTAFromLatestRelease()) {
        // якщо OTA не вдалось — дозволимо наступні спроби
        updating = false;
      }
      // при успіху пристрій сам перезавантажиться
    }
  }

  // (опц.) інші таски...
}
