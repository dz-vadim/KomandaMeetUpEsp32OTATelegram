#pragma once
#include <Arduino.h>

// Ініціалізація Telegram-модуля (NVS/offset)
void telegramInit();

// Відправити текст у Telegram
bool sendTelegram(const String &text);

// Опитати Telegram: повертає true, якщо прийшла команда /update
bool fetchTelegramAndCheckUpdateCmd();

// Одноразово "з'їсти" старі апдейти на старті, щоб /update не повторився після ребута
void drainOldTelegramUpdatesOnce();

// Прапорець запиту /info (повертає true один раз на одну команду)
bool isInfoRequested();

// Останній отриманий текст команди (для діагностики/логів)
String getLastCommand();
