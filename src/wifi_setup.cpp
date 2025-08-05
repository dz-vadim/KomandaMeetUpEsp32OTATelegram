#include <WiFi.h>
#include <WiFiManager.h>
#include "lcd_display.h"
#include "secrets.h"

void setupWiFiWithPortal(const String &deviceName) {
  WiFi.mode(WIFI_STA);
  WiFiManager wm;
  if (strlen(WM_AP_NAME) > 0) {
    wm.setHostname(deviceName.c_str());
    showWiFiPortal();
    bool ok = wm.autoConnect(WM_AP_NAME, strlen(WM_AP_PASS) ? WM_AP_PASS : nullptr);
    if (!ok) {
      lcdPrint2("WiFi failed", "Reboot...");
      delay(2000);
      ESP.restart();
    }
  }
}
