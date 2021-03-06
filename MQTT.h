#ifndef _MQTT_
#define _MQTT_

// -----------------------------------------------------------------------
// --------------------------------Include--------------------------------
// -----------------------------------------------------------------------

// AsyncMqttClient
#include <AsyncMqttClient.h>

#include <Wifi.h>

extern "C" {
  #include "freertos/timers.h"
}
// -----------------------------------------------------------------------
// --------------------------------Define---------------------------------
// -----------------------------------------------------------------------

extern AsyncMqttClient mqttClient;

// #define MQTT_HOST "192.168.1.4"
// #define MQTT_PORT 1883
#define MQTT_HOST "113.161.225.11"
#define MQTT_PORT 3005
#define username1 "username"
#define password1 "password"
// MQTT Topics
#define MQTT_PUB_TEMP "esp/publish"

// length + 12
#define MQTT_R1_TP "relay1/devID/"
#define MQTT_R2_TP "relay2/devID/"

// -----------------------------------------------------------------------
// -------------------------------Dedcleare-------------------------------
// -----------------------------------------------------------------------

extern TimerHandle_t mqttReconnectTimer;
extern String relay1_mqtt;
extern String relay2_mqtt;

extern char R1_TP_CHAR[16];
extern char R2_TP_CHAR[16];

void connectToMqtt();
void onMqttConnect(bool sessionPresent);
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
void onMqttSubscribe(uint16_t packetId, uint8_t qos);
void onMqttUnsubscribe(uint16_t packetId);
void onMqttPublish(uint16_t packetId);
void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, 
                    size_t len, size_t index, size_t total);

#endif //_MQTT_
