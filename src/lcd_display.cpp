#include <Arduino.h>
#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include "lcd_display.h"
#include "secrets.h"

#ifndef LCD_I2C_ADDR
#define LCD_I2C_ADDR 0x27
#endif
#ifndef LCD_COLS
#define LCD_COLS 16
#endif
#ifndef LCD_ROWS
#define LCD_ROWS 2
#endif
#ifndef FW_VERSION
#define FW_VERSION "local"
#endif

static LiquidCrystal_I2C lcd(LCD_I2C_ADDR, LCD_COLS, LCD_ROWS);

void lcdInit() {
  lcd.init();
  lcd.backlight();
}

void lcdPrint2(const String &l1, const String &l2) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(l1.length() > LCD_COLS ? l1.substring(0, LCD_COLS) : l1);
  lcd.setCursor(0,1);
  lcd.print(l2.length() > LCD_COLS ? l2.substring(0, LCD_COLS) : l2);
}

void showBoot()        { lcdPrint2("Booting...", "FW v" + String(FW_VERSION)); }
void showWiFiPortal()  { lcdPrint2("WiFi Portal", "SSID:" + String(WM_AP_NAME)); }
void showWiFiOK()      { lcdPrint2("WiFi OK", WiFi.localIP().toString()); }
void showReady()       { lcdPrint2("Ready", "v" + String(FW_VERSION)); }
void showUpdating()    { lcdPrint2("Updating...", "Please wait"); }
void showUpdateDone(bool ok) { lcdPrint2(ok ? "Update OK" : "Update FAIL", "Rebooting..."); }
