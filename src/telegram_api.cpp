#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include "secrets.h"

// ---- внутрішній стан ----
static Preferences prefs;
static const char* NVS_NS = "tg";
static long lastTelegramUpdateId = 0;
static bool infoRequested = false;
static String lastCmd = "";

// ---- API ----
void telegramInit() {
  prefs.begin(NVS_NS, false);
  lastTelegramUpdateId = prefs.getLong("lastId", 0);
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

// З'їдаємо старі апдейти на старті, щоб /update не повторився після ребута
void drainOldTelegramUpdatesOnce() {
  WiFiClientSecure client; client.setInsecure();
  HTTPClient http;
  String url = "https://api.telegram.org/bot" + String(TG_BOT_TOKEN) + "/getUpdates?timeout=1";
  if (!http.begin(client, url)) return;

  int code = http.GET();
  if (code == HTTP_CODE_OK) {
    JsonDocument doc;
    if (!deserializeJson(doc, http.getStream())) {
      for (JsonObject upd : doc["result"].as<JsonArray>()) {
        long updId = upd["update_id"] | 0;
        if (updId > lastTelegramUpdateId) lastTelegramUpdateId = updId;
      }
      if (lastTelegramUpdateId > 0) {
        prefs.putLong("lastId", lastTelegramUpdateId);
      }
    }
  }
  http.end();
}

// Повертає true, якщо знайдена /update; також виставляє прапорець infoRequested для /info
bool fetchTelegramAndCheckUpdateCmd() {
  WiFiClientSecure client; client.setInsecure();
  HTTPClient http;
  String url = "https://api.telegram.org/bot" + String(TG_BOT_TOKEN) + "/getUpdates?timeout=1";
  if (lastTelegramUpdateId > 0) {
    url += "&offset=" + String(lastTelegramUpdateId + 1);
  }

  if (!http.begin(client, url)) return false;
  int code = http.GET();
  if (code != HTTP_CODE_OK) { http.end(); return false; }

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, http.getStream());
  http.end();
  if (err) return false;

  bool shouldUpdate = false;
  infoRequested = false; // скидаємо перед обробкою

  for (JsonObject upd : doc["result"].as<JsonArray>()) {
    long updId = upd["update_id"] | 0;
    if (updId > lastTelegramUpdateId) lastTelegramUpdateId = updId;

    JsonObject msg = upd["message"];
    if (!msg.isNull()) {
      String chatId = String((long)msg["chat"]["id"]);
      String text = String((const char*)msg["text"]);
      if (chatId == TG_CHAT_ID) {
        lastCmd = text;
        if (text.equalsIgnoreCase("/update")) {
          shouldUpdate = true;
        } else if (text.equalsIgnoreCase("/info")) {
          infoRequested = true;
        }
      }
    }
  }

  // Зберігаємо offset, щоб після ребута команди не повторились
  prefs.putLong("lastId", lastTelegramUpdateId);
  return shouldUpdate;
}

bool isInfoRequested() {
  if (infoRequested) {
    infoRequested = false; // одноразове спрацьовування
    return true;
  }
  return false;
}

String getLastCommand() {
  return lastCmd;
}
