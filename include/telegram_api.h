#pragma once
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "secrets.h"

// Глобальні об'єкти/змінні, які визначені в telegram_api.cpp
extern WiFiClientSecure tgClient;
extern UniversalTelegramBot bot;
extern int lastProcessedId;

void telegramInit();
bool sendTelegram(const String &text);
bool fetchTelegramAndCheckUpdateCmd();
