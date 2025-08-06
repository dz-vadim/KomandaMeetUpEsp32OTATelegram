#pragma once
#include <Arduino.h>

void telegramInit();
bool sendTelegram(const String &text);
bool fetchTelegramAndCheckUpdateCmd();
