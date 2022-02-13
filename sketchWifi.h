#ifndef _sketchWifi_
#define _sketchWifi_

// -----------------------------------------------------------------------
// --------------------------------Include--------------------------------
// -----------------------------------------------------------------------

#include <ETH.h>
#include <Wifi.h>   
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiGeneric.h>
#include <WiFiMulti.h>
#include <WiFiScan.h>
#include <WiFiServer.h>
#include <WiFiSTA.h>
#include <WiFiType.h>
#include <WiFiUdp.h>

#include "GrapeRemote.h"

// -----------------------------------------------------------------------
// --------------------------------Define---------------------------------
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
// -------------------------------Dedcleare-------------------------------
// -----------------------------------------------------------------------

//--------------------------------Function----------------------------
// extern wl_status_t WiFiStationConnected(WiFiEventInfo_t info);
// extern wl_status_t WiFiStationDisconnected(WiFiEventInfo_t info);
// extern void Wifi_BkUp();
// extern void ConnectWifi(String Ssid, String password);
// extern int getWiFi_INFO(int size_add, int len_add, int data_add);
wl_status_t WiFiStationConnected(WiFiEventInfo_t info);
wl_status_t WiFiStationDisconnected(WiFiEventInfo_t info);
void Wifi_BkUp();
void ConnectWifi(String Ssid, String password);
int getWiFi_INFO(int size_add, int len_add, int data_add);

void connectToWifi();

//WiFi Parameters
extern String gsWifiSSID;
extern String gsWifiPassword;
extern String gsWifiEssid;
extern String gsWifiEpassword;
extern String gsWifiSsidUpdate;
extern String gsWifiPasswordUpdate;
extern String gsWifiSsid_connected;
extern String gsWifiPass_connected;

//Define wifi handler object
extern volatile wl_status_t wifi_status_event;

#endif //_sketchWifi_
