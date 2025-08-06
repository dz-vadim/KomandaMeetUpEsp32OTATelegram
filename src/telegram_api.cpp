#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include "secrets.h"

static Preferences prefs;
static long lastProcessedId = 0;

void telegramInit() {
  prefs.begin("tg", false);
  lastProcessedId = prefs.getLong("lastId", 0);
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
  bool ok = (http.POST(body) == HTTP_CODE_OK);
  http.end();
  return ok;
}

bool fetchTelegramAndCheckUpdateCmd() {
  WiFiClientSecure client; client.setInsecure();
  HTTPClient http;
  String url = "https://api.telegram.org/bot" + String(TG_BOT_TOKEN) + "/getUpdates?timeout=1";
  if (lastProcessedId > 0) url += "&offset=" + String(lastProcessedId + 1);

  if (!http.begin(client, url)) return false;
  if (http.GET() != HTTP_CODE_OK) { http.end(); return false; }

  JsonDocument doc;
  if (deserializeJson(doc, http.getStream())) { http.end(); return false; }
  http.end();

  bool shouldUpdate = false;
  for (JsonObject upd : doc["result"].as<JsonArray>()) {
    long updId = upd["update_id"] | 0;
    String chatId = String((long)upd["message"]["chat"]["id"]);
    String text   = String((const char*)upd["message"]["text"]);

    if (updId > lastProcessedId) {
      lastProcessedId = updId;
      prefs.putLong("lastId", lastProcessedId);
    }

    if (chatId == TG_CHAT_ID && text.equalsIgnoreCase("/update")) {
      shouldUpdate = true;
    }
  }
  return shouldUpdate;
}
