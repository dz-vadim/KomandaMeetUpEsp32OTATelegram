#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include "ota_update.h"
#include "lcd_display.h"
#include "telegram_api.h"
#include "secrets.h"

#ifndef FW_VERSION
#define FW_VERSION "local"
#endif

static String ghLatestReleaseUrl() {
  // https://github.com/<owner>/<repo>/releases/latest/download/<filename>
  return "https://github.com/" + String(GH_REPO) + "/releases/latest/download/" + String(GH_FW_FILENAME);
}

bool doOTAFromLatestRelease() {
  lcdShow("Updating...", "Please wait");

  WiFiClientSecure client; 
  client.setInsecure();

  httpUpdate.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS); // для 302 від GitHub
  httpUpdate.rebootOnUpdate(true);

  String url = ghLatestReleaseUrl();
  t_httpUpdate_return ret = httpUpdate.update(client, url);

  switch (ret) {
    case HTTP_UPDATE_OK:
      lcdShow("Update OK", "Rebooting...");
      return true; // пристрій перезавантажиться
    case HTTP_UPDATE_NO_UPDATES:
      sendTelegram("No updates available.");
      break;
    case HTTP_UPDATE_FAILED:
    default:
      sendTelegram(String("Update failed: ") + httpUpdate.getLastErrorString());
      break;
  }
  lcdShow("Update FAIL", "See Telegram");
  delay(2000);
  return false;
}
