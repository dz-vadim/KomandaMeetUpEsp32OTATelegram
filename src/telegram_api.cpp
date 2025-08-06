#include "telegram_api.h"
#include <Preferences.h>

WiFiClientSecure tgClient;
UniversalTelegramBot bot(TG_BOT_TOKEN, tgClient);
int lastProcessedId = 0;

static Preferences prefs;

void telegramInit() {
  tgClient.setInsecure();
  prefs.begin("tg", false);                 // namespace "tg"
  lastProcessedId = prefs.getInt("lastId", 0);
}

bool sendTelegram(const String &text) {
  return bot.sendMessage(TG_CHAT_ID, text, "Markdown");
}

bool fetchTelegramAndCheckUpdateCmd() {
  // Отримуємо тільки нові апдейти після збереженого ID
  int numNew = bot.getUpdates(lastProcessedId + 1);
  bool shouldUpdate = false;

  for (int i = 0; i < numNew; i++) {
    auto &m = bot.messages[i];

    // Якщо це справді новіший update — зберігаємо в NVS
    if (m.update_id > lastProcessedId) {
      lastProcessedId = m.update_id;
      prefs.putInt("lastId", lastProcessedId);   // ← персистентно
    }

    if (m.chat_id == String(TG_CHAT_ID) &&
        (m.text == "/update" || m.text.startsWith("/update@"))) {
      shouldUpdate = true;
    }
  }
  return shouldUpdate;
}
