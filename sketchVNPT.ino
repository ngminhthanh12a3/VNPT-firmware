#include "sketchVNPT.h"

/*******************************************************************************
* MCU Setup
*******************************************************************************/

void setup() {  
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println("Start program");
  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));
  WiFi.onEvent(Wifi_Handle_Event);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  // If your broker requires authentication (username and password), set them below
  mqttClient.setCredentials(username1, password1);
  connectToWifi();
   /* Local variable declaration */
  SchedulerReturnType lenReturnValue;
  lenReturnValue = SCH_SUCCESS;
  /*
   * GPIO setting for peripheral
   */
  /* Setup OUTPUT pins */
  pinMode(LED1_IO, OUTPUT);
  pinMode(LED2_IO, OUTPUT);
  pinMode(RELAY1_IO, OUTPUT);
  pinMode(RELAY2_IO, OUTPUT);

  /* Setup INPUT pins */
  pinMode(UPDATE_IO, INPUT);
  pinMode(WE_IO, OUTPUT);
  /* Set initialize level */
  digitalWrite(LED1_IO, LOW);
  digitalWrite(LED2_IO, LOW);
  digitalWrite(RELAY1_IO, RELAY_DEACTIVE);
  digitalWrite(RELAY2_IO, RELAY_DEACTIVE);

  #if (USE_CONFIG_BOOT == STD_OFF)

  gsWifiSSID = WIFI_SSID;
  gsWifiPassword = WIFI_PASSWORD;
  #else

  delay(500);
  /* Print config for debugging */
  #if (SHOW_CONFIG == STD_ON)

  /*No Action*/
  #endif

  /*No Action*/
  
  #endif /* End of if (USE_CONFIG_BOOT == STD_OFF) */
   
  EEPROM.begin(EEPROM_SIZE);
  AES_init_ctx(&ctx, key);
  /* Connect the wifi */
  Disconnected_flag = 0;
  
  Serial.print("STATUS bit: ");
  Serial.println(EEPROM.readByte(STATUS_REG_ADD));
  


  vspi = new SPIClass(VSPI);
  vspi->begin();
  pinMode(VSPI_SSL_IO, OUTPUT); //VSPI SS

  /* Setup timer interrupt */
  /* Use 1st timer of 4 */
  /* 1 tick take 1/(80MHZ/80) = 1us so we set divider 80 and count up */
  timer = timerBegin(0, 80, true);
  /* Attach onTimer function to our timer */
  timerAttachInterrupt(timer, &onTimer, true);
  
  /* Set alarm to call onTimer function every second 1 tick is 1us => 1ms second is 1000us */
  /* Repeat the alarm (third parameter) */
  timerAlarmWrite(timer, 1000, true);

  /* Start an alarm */
  timerAlarmEnable(timer);
  Serial.println("start timer");

  countTimer = timerBegin(1, 800, true);
 
  


  /*
  This option allows get and delete functions (PUT and DELETE HTTP requests) works for device connected behind the
  Firewall that allows only GET and POST requests.
  
  /* Setup the scheduler */
  Scheduler_Init();
   lenReturnValue = Scheduler_AddTask(RelayTask, RELAY_TASK_TIME);
   lenReturnValue = Scheduler_AddTask(TemperatureTask, TEMPERATURE_TASK_TIME);
   lenReturnValue = Scheduler_AddTask(CurrentAndVoltageTask, CURRENT_TASK_TIME);
   lenReturnValue = Scheduler_AddTask(CheckingWifiTask, CHECKING_WIFI_TASK_TIME);
   lenReturnValue = Scheduler_AddTask(DynamoTask, DYNAMO_TASK_TIME);
}
unsigned int devID = 9;

/*******************************************************************************
* MCU Loop Function
*******************************************************************************/

void loop() 
{
  
      // put your main code here, to run repeatedly:
  if(wifi_status_event == WL_CONNECTED){
    /*Scheduler handle*/
  wifi_status = "ACTIVE";
    if(giUpdateWiFiConfig == 1 && wifi_flag == 1) {
      wifi_status = "ACTIVE";

      wifi_flag = 0;
    } 
    (void)Scheduler_Dispatcher();
  }
  else{
    /*no action*/
  }
    unsigned long currentMillis = millis();
  // Every X number of seconds (interval = 10 seconds) 
  // it publishes a new MQTT message
  
  if (currentMillis - previousMillis >= interval) {
    // Save the last time a new reading was published
    previousMillis = currentMillis;
    uint16_t packetIdSub2 = mqttClient.subscribe("/node/relay1", 2);
    Serial.print("Subscribing at QoS 2, packetId: ");
    Serial.println(packetIdSub2);
    uint16_t packetIdSub3 = mqttClient.subscribe("/node/relay2", 2);
    Serial.print("Subscribing at QoS 2, packetId: ");
    Serial.println(packetIdSub3);
    uint16_t packetIdSub = mqttClient.subscribe("/get/type_data", 2);
    Serial.print("Subscribing at QoS 2, packetId: ");
    Serial.println(packetIdSub);
    // New temperature readings
//    sensors.requestTemperatures(); 
    // Temperature in Celsius degrees
//    temp = sensors.getTempCByIndex(0);
    // Temperature in Fahrenheit degrees
    //temp = sensors.getTempFByIndex(0);
    
    DynamicJsonDocument doc(400);
    doc["devID"] = String(devID);
    doc["status1"] = status1;
    doc["status2"] = status2;
    doc["current1"] = cur1;
    doc["current2"] = cur2;
    doc["voltage1"] = vol1;
    doc["voltage2"] = vol2;
    doc["relay_warning1"] = rl_warning1;
    doc["relay_warning2"] = rl_warning2;
    doc["temperature_warning"] = temp_warning;
    doc["temperature"] = temp;
    doc["dynamo_status"] = dynamo_status;
    doc["wifi_status"] = wifi_status;

    serializeJson(doc, Serial);
    char buffer[400];
    size_t n = serializeJson(doc, buffer);
    uint16_t packetIdPub1 = mqttClient.publish(MQTT_PUB_TEMP, n, true, buffer);
    Serial.println("");
    Serial.println("send data to server");
    
//    // Publish an MQTT message on topic esp32/ds18b20/temperature
//    uint16_t packetIdPub1 = mqttClient.publish(MQTT_PUB_TEMP, 1, true, String(temp).c_str());                            
    Serial.printf("Publishing on topic %s at QoS 1, packetId: ", MQTT_PUB_TEMP);
    Serial.println(packetIdPub1);

  }
//  // put your main code here, to run repeatedly:
//  if(wifi_status_event == WL_CONNECTED){
//    /*Scheduler handle*/
//  wifi_status = "DEACTIVE";
//    if(giUpdateWiFiConfig == 1 && wifi_flag == 1) {
//      wifi_status = "DEACTIVE";
//
//      wifi_flag = 0;
//    } 
//    (void)Scheduler_Dispatcher();
//  }
//  else{
//    /*no action*/
//  }
}