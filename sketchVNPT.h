#ifndef _sketch_
#define _sketch_


// -----------------------------------------------------------------------
// --------------------------------Include--------------------------------
// -----------------------------------------------------------------------

// ArduinoJson
#include <ArduinoJson.h>
extern "C" {
  #include "freertos/FreeRTOS.h"
  #include "freertos/timers.h"
}

// Ethernet and Wifi
#include "sketchWifi.h"

#include <SPI.h>
#include <EEPROM.h>
#include <stdio.h>

// GrapeRemote, Scheduler
// #include "GrapeRemote.h"
// #include "GrapeRemoteType.h"
#include "Scheduler.h"

#include "FS.h"
#include "SPIFFS.h"
#include "aes.h"
#include "string.h"

// 
#include "MQTT.h"
#include "ChaCha20.h"

// toCharArray
#include <Arduino.h>

// -----------------------------------------------------------------------
// --------------------------------Define---------------------------------
// -----------------------------------------------------------------------



// -----------------------------------------------------------------------
// -------------------------------Dedcleare-------------------------------
// -----------------------------------------------------------------------

extern TimerHandle_t wifiReconnectTimer;
extern unsigned long previousMillis;   // Stores last time temperature was published
extern const long interval;        // Interval at which to publish sensor readings

/*******************************************************************************
* Global variable definition
*******************************************************************************/
extern uint8_t key[16];
extern struct AES_ctx ctx;
static const int spiClk = 2000000; // 1 MHz


//Timer Parameters
extern uint32_t currentTime;
extern uint32_t startTime[3];


//Status Device Parameters
extern uint8_t giDynamo_status;
extern uint8_t Dynamo_flag;
extern uint8_t giRelay1_status;
extern uint8_t giRelay2_status;
extern unsigned long Relay1_timer;
extern unsigned long Relay2_timer;
extern int giUpdateWiFiConfig;
extern volatile uint8_t Disconnected_flag;
extern volatile unsigned long Disconnected_timer;
extern uint8_t eeprom_commit_call;
extern uint8_t wifi_flag;

extern uint8_t gucOverheat_flag;
extern uint32_t gulOverheat_timer;
extern uint8_t gucRelay_Warning[2];


/* create a hardware timer */
extern hw_timer_t * timer;
extern hw_timer_t * countTimer;
extern SPIClass * vspi;
extern SPIClass * hspi;

extern String status1;
extern String status2;
extern String rl_warning1;
extern String rl_warning2;
extern String temp_warning;
extern String dynamo_status;
extern String wifi_status;

/* Timer interrupt call back */
void IRAM_ATTR onTimer();

/*******************************************************************************
* Task Function definition
*******************************************************************************/
float A2F(byte start_byte);

extern String relay1_mqtt;
extern String relay2_mqtt;
void RelayTask(void);

void DynamoTask(void);

extern String temp ;
void TemperatureTask(void);

extern String cur1,cur2,vol1,vol2;
void CurrentAndVoltageTask(void);

void CheckingWifiTask(void);

/*******************************************************************************
* Task WiFi Handler Function definition
*******************************************************************************/

void Wifi_Handle_Event(WiFiEvent_t event, WiFiEventInfo_t info);
wl_status_t WiFiStationGotIP(WiFiEventInfo_t info);
wl_status_t WiFiStationDisconnected(WiFiEventInfo_t info);
/*******************************************************************************
* Wifi Function deployment
*******************************************************************************/
void Wifi_BkUp();
int getWiFi_INFO(int ssid_size_add, int pass_size_add);
void ConnectWifi(String ssid, String password);

//Function
byte transferAndWait(byte c);
float Read_vspi();
String eep_readString(uint16_t start_add, uint16_t size_add);
int eep_writeString(uint16_t start_add, String str);
void TurnOn_Relay(uint8_t RelayOn);
void TurnOff_Relay(uint8_t RelayOff);

#endif //_sketch_
