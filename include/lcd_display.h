#pragma once
#include <Arduino.h>

void lcdInit();
void lcdPrint2(const String &l1, const String &l2);
void showBoot();
void showWiFiPortal();
void showWiFiOK();
void showReady();
void showUpdating();
void showUpdateDone(bool ok);
