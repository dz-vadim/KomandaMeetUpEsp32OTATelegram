#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "secrets.h"

static long lastProcessedId = 0; // щоб не запускати повторно

void telegramInit() {
  lastProcessedId = 0;
}

bool sendTelegram(const String &text) {
  WiFiClientSecure client; client.setInsecure();
  HTTPClient http;
  String url = "https://api.telegram.org/bot" + String(TG_BOT_TOKEN) + "/sendMessage";
  if (!http.begin(client, url)) return false;
  http.addHeader("Content-Type", "application/json");

  JsonDocument doc;
  doc["chat_id"] = TG_CHAT_ID;
  doc["text"] = text;

  String body; serializeJson(doc, body);
  int code = http.POST(body);
  http.end();
  return (code == HTTP_CODE_OK);
}

bool fetchTelegramAndCheckUpdateCmd() {
  WiFiClientSecure client; client.setInsecure();
  HTTPClient http;
  String url = "https://api.telegram.org/bot" + String(TG_BOT_TOKEN) + "/getUpdates?timeout=1";

  if (!http.begin(client, url)) return false;
  int code = http.GET();
  if (code != HTTP_CODE_OK) { http.end(); return false; }

  JsonDocument doc;
  if (deserializeJson(doc, http.getStream())) { http.end(); return false; }
  http.end();

  for (JsonObject upd : doc["result"].as<JsonArray>()) {
    long updId = upd["update_id"] | 0;
    String chatId = String((long)upd["message"]["chat"]["id"]);
    String text = String((const char*)upd["message"]["text"]);

    // Ігноруємо, якщо це старе повідомлення
    if (updId <= lastProcessedId) continue;

    if (chatId == TG_CHAT_ID && text.equalsIgnoreCase("/update")) {
      lastProcessedId = updId; // запам'ятовуємо, щоб не повторювати
      return true;
    }
  }
  return false;
}
