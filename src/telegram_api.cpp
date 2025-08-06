#include "telegram_api.h"

WiFiClientSecure tgClient;
UniversalTelegramBot bot(TG_BOT_TOKEN, tgClient);
int lastProcessedId = 0;

void telegramInit() {
  // Для простоти і щоб не возитися з сертифікатами
  tgClient.setInsecure();
}

bool sendTelegram(const String &text) {
  // Повертає true/false за результатом відправлення
  return bot.sendMessage(TG_CHAT_ID, text, "Markdown");
}

bool fetchTelegramAndCheckUpdateCmd() {
  // Отримуємо тільки нові апдейти
  int numNew = bot.getUpdates(lastProcessedId + 1);
  bool shouldUpdate = false;

  for (int i = 0; i < numNew; i++) {
    auto &m = bot.messages[i];

    // Оновлюємо локальний lastProcessedId
    // (у UniversalTelegramBot update_id — це int)
    lastProcessedId = m.update_id;

    // Перевіряємо чат та команду
    if (m.chat_id == String(TG_CHAT_ID) &&
        (m.text == "/update" || m.text.startsWith("/update@"))) {
      shouldUpdate = true;
    }
  }
  return shouldUpdate;
}
