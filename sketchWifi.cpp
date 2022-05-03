#include "sketchWifi.h"

//WiFi Parameters
String gsWifiSSID;
String gsWifiPassword;
String gsWifiEssid;
String gsWifiEpassword;
String gsWifiSsidUpdate;
String gsWifiPasswordUpdate;
String gsWifiSsid_connected;
String gsWifiPass_connected;

//Define wifi handler object
volatile wl_status_t wifi_status_event;

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());
}
