#include "MQTT.h"

AsyncMqttClient mqttClient;

TimerHandle_t mqttReconnectTimer;

String relay1_mqtt;
String relay2_mqtt;

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}
void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");
  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}
void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}
void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void onMqttPublish(uint16_t packetId) {
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}
void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  qos: ");
  Serial.println(properties.qos);
  Serial.print("  dup: ");
  Serial.println(properties.dup);
  Serial.print("  retain: ");
  Serial.println(properties.retain);
  Serial.print("  len: ");
  Serial.println(len);
  Serial.print("  index: ");
  Serial.println(index);
  Serial.print("  total: ");
  Serial.println(total);
  Serial.println(String(payload[1]));
//  khi mqtt broker publish với các topic tương ứng thì sẽ xử lý và gán vào 2 biến relay1_mqtt và relay2_mqtt để xử lý ở hàm relay task
  if((String(payload[1]) == "N")&&(String(topic) == "/node/relay1"))
  {
    Serial.print("\nBat device 1\n");
    relay1_mqtt = "ON";
    Serial.print("relay1 mqtt:");
    Serial.println(relay1_mqtt);
  }
//  nếu nhận được topic là relay1 on thì gán topic đó vào relay1_mqtt
  else if((String(payload[1]) == "F")&&(String(topic) == "/node/relay1"))
  {
    Serial.print("\nTat device 1\n");
    relay1_mqtt = "OFF";
    Serial.print("relay1 mqtt:");
    Serial.println(relay1_mqtt);
  }
// tương tự các hàm còn lại
    
  if((String(payload[1]) == "N")&&(String(topic) == "/node/relay2"))
  {
    Serial.print("\nBat device 2\n");
    relay2_mqtt = "ON";
    Serial.print("relay2 mqtt:");
    Serial.println(relay2_mqtt);
  }
//  nếu nhận được topic là relay1 on thì gán topic đó vào relay2_mqtt
  else if((String(payload[1]) == "F")&&(String(topic) == "/node/relay2"))
  {
    Serial.print("\nTat device 2\n");
    relay2_mqtt = "OFF";
    Serial.print("relay2 mqtt:");
    Serial.println(relay2_mqtt);
  }
}
