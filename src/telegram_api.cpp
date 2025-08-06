#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "secrets.h"

void telegramInit() {
  // Нічого не робимо — спрощено
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
    String chatId = String((long)upd["message"]["chat"]["id"]);
    String text = String((const char*)upd["message"]["text"]);
    if (chatId == TG_CHAT_ID && text.equalsIgnoreCase("/update")) {
      return true;
    }
  }
  return false;
}
