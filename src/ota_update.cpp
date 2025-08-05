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
  showUpdating();

  WiFiClientSecure client; 
  client.setInsecure();

  httpUpdate.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS); // слідуємо за 302 GitHub
  httpUpdate.rebootOnUpdate(true);

  String url = ghLatestReleaseUrl();
  t_httpUpdate_return ret = httpUpdate.update(client, url);

  switch (ret) {
    case HTTP_UPDATE_OK:
      showUpdateDone(true);
      return true; // пристрій перезавантажиться автоматично
    case HTTP_UPDATE_NO_UPDATES:
      sendTelegram("No updates available.");
      break;
    case HTTP_UPDATE_FAILED:
    default:
      sendTelegram(String("Update failed: ") + httpUpdate.getLastErrorString());
      break;
  }
  showUpdateDone(false);
  delay(3000);
  return false;
}
