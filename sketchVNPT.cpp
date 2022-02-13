#include "sketchVNPT.h"

TimerHandle_t wifiReconnectTimer;
unsigned long previousMillis = 0;   // Stores last time temperature was published
const long interval = 5000;        // Interval at which to publish sensor readings

/*******************************************************************************
* Global variable definition
*******************************************************************************/
uint8_t key[16] = { (uint8_t)0x00, (uint8_t)0x01, (uint8_t)0x02, (uint8_t)0x03, (uint8_t)0x04, (uint8_t)0x05, (uint8_t)0x06, (uint8_t)0x07, 
                    (uint8_t)0x08, (uint8_t)0x09, (uint8_t)0x0a, (uint8_t)0x0b, (uint8_t)0x0c, (uint8_t)0x0d, (uint8_t)0x0e, (uint8_t)0x0f };
struct AES_ctx ctx;

//Timer Parameters
uint32_t currentTime;
uint32_t startTime[3] = {0, 0, 0};

//Status Device Parameters
uint8_t giDynamo_status = DYNAMO_OFF;
uint8_t Dynamo_flag = 1;
uint8_t giRelay1_status = 0;
uint8_t giRelay2_status = 0;
unsigned long Relay1_timer = 0;
unsigned long Relay2_timer =0;
int giUpdateWiFiConfig = UPDATE_WIFI_NO_CALL;
volatile uint8_t Disconnected_flag;
volatile unsigned long Disconnected_timer = 0;
uint8_t eeprom_commit_call = 0;
uint8_t wifi_flag = 1;

uint8_t gucOverheat_flag = 0;
uint32_t gulOverheat_timer = 0;
uint8_t gucRelay_Warning[2] = {RELAY_AVAILABLE, RELAY_AVAILABLE};


/* create a hardware timer */
hw_timer_t * timer = NULL;
hw_timer_t * countTimer = NULL;
SPIClass * vspi = NULL;
SPIClass * hspi = NULL;

String status1;
String status2;
String rl_warning1;
String rl_warning2;
String temp_warning;
String dynamo_status;
String wifi_status;

/* Timer interrupt call back */
void IRAM_ATTR onTimer(){
  /* Handle the scheduler time tick */

  if(wifi_status_event == WL_CONNECTED){
  Scheduler_SynTick();
  } 
  else{
    /*no action*/

  }

}

/*******************************************************************************
* Task Function definition
*******************************************************************************/
float A2F(byte start_byte){
  float rx_data;
  uint16_t luwDataDec = spi_data.b[start_byte]*pow(2,8) + spi_data.b[start_byte+1];
  uint16_t luwDataFrac = spi_data.b[start_byte+2]*pow(2,8) + spi_data.b[start_byte+3];

  rx_data = (float)(luwDataDec) + (float)(luwDataFrac)/1000;
  return rx_data;
}

void RelayTask(void){
//  long int counterTime;
//  
//  if(gucRelay_Warning[0] == RELAY_UNAVAILABLE) giRelay1_status = RELAY_OFF;
//  if(gucRelay_Warning[1] == RELAY_UNAVAILABLE) giRelay2_status = RELAY_OFF;  
//  
//  if(Relay1_timer > SWITCH_TIME_CALL) {
//      if(gucRelay_Warning[1] == RELAY_AVAILABLE){ 
//        status1 = "OFF";
//        giRelay1_status = RELAY_OFF;
//        
//        status2 = "ON";
//        giRelay2_status = RELAY_ON;
//      }
//      Relay1_timer = 0;
//    }
//
//
//  if(Relay2_timer > SWITCH_TIME_CALL){
//    if(gucRelay_Warning[0] == RELAY_AVAILABLE){ 
//      status1 = "ON";
//      giRelay1_status = RELAY_ON;
//      
//      status2 = "OFF";
//      giRelay2_status = RELAY_OFF;
//    }
//    Relay2_timer = 0;
//  }
//// xử lý topic được gán ở 2 biến relay1_mqtt và relay2_mqtt để cập nhập giRelay1_status và giRelay2_status 
//    if (relay1_mqtt == "ON"){
//      giRelay1_status = RELAY_ON;
//    }
//    else if(relay1_mqtt =="OFF"){
//      giRelay1_status = RELAY_OFF;
//      }
//     cập nhập dữ liệu từ web và xử lý dưới phần cứng
    if(relay1_mqtt == "ON"){
      status1 = "ON";
      TurnOn_Relay(RELAY1_IO);

  } 
  if(relay1_mqtt == "OFF"){
    status1 = "OFF";
    TurnOff_Relay(RELAY1_IO);
  }

    if(relay2_mqtt == "ON"){
      status2 = "ON";
      TurnOn_Relay(RELAY2_IO);
      
    
  }
  if(relay2_mqtt == "OFF"){
    status2 = "OFF";
    TurnOff_Relay(RELAY2_IO);

  }
  
}

void DynamoTask(void){
  int liDynamo_status;
  if(wifi_status_event == WL_CONNECTED){
    Read_vspi();
    liDynamo_status = spi_data.b[20];
//    dynamo_status = "ACTIVE";
    if(liDynamo_status == DYNAMO_ON){
      dynamo_status = "ACTIVE";
      if(Dynamo_flag == 1 ){
      
      status1 = "OFF";
      TurnOff_Relay(RELAY1_IO);
      Serial.println("ReLay1: OFF");
      
      status2 = "OFF";
      TurnOff_Relay(RELAY2_IO);
      Serial.println("ReLay2: OFF");
      
      Dynamo_flag = 0;
      giRelay1_status = RELAY_OFF;
      giRelay2_status = RELAY_OFF;
    }
  }
  else if(giDynamo_status == DYNAMO_OFF) {
      dynamo_status = "DEACTIVE";
      Dynamo_flag = 1;
    }
  }
}

String temp ;
void TemperatureTask(void){
  float lfTemp;
  if(wifi_status_event == WL_CONNECTED){

    Read_vspi();
    lfTemp = A2F(16);
    temp = String(lfTemp);
    if(gucRelay_Warning[0] == RELAY_UNAVAILABLE && gucRelay_Warning[1] == RELAY_UNAVAILABLE){
      temp_warning = "OVERHEAT WARNING";
      Serial.print("Temperature = "); Serial.println("OVERHEAT WARNING"); 
    }
    else {
      
      Serial.print("Temperature = "); Serial.println(lfTemp);  
  
      if(lfTemp >= TEMPERATURE_OVERHEAT){
        gulOverheat_timer++;
            
      } 
      else if(lfTemp < TEMPERATURE_OVERHEAT){
        gulOverheat_timer = 0;
      }
  
      if(gulOverheat_timer >= OVERHEAT_TIME){
          if(giRelay1_status == RELAY_ON){
            gucRelay_Warning[0] = RELAY_UNAVAILABLE;
            rl_warning1 = "RELAY_UNAVAILABLE";
            giRelay1_status = RELAY_OFF;
            giRelay2_status = RELAY_ON;
            gulOverheat_timer = 0;
          }

          else if(giRelay2_status == RELAY_ON){
            gucRelay_Warning[1] = RELAY_UNAVAILABLE;
            rl_warning2 = "RELAY_UNAVAILABLE";
            giRelay2_status = RELAY_OFF;
            giRelay1_status = RELAY_ON;
            gulOverheat_timer = 0;
          }
      }
  }
  }
}

String cur1,cur2,vol1,vol2;
void CurrentAndVoltageTask(void){
  float lfCurrent1, lfCurrent2;
  float lfVoltage1, lfVoltage2;
  if(wifi_status_event == WL_CONNECTED){

    Read_vspi();
    
    lfVoltage1 = A2F(0);
//    lfVoltage1 = rand() % 50;
    if(lfVoltage1 <= 180){
    
    vol1 = "Voltage Warning";
    Serial.print("Voltage1 = "); Serial.println("Voltage Warning");
  } else{
    vol1  = String(lfVoltage1);
    Serial.print("Voltage1 = "); Serial.println(lfVoltage1);
  }
  
    lfVoltage2 = A2F(4);
//    lfVoltage2 = rand() % 50;
  if(lfVoltage2 <= 180){
    vol2 = "Voltage Warning";
    Serial.print("Voltage2 = "); Serial.println("Voltage Warning");
    } else{
    vol2  = String(lfVoltage2);
    Serial.print("Voltage2 = "); Serial.println(lfVoltage2);
  }
    lfCurrent1 = A2F(8);
//    lfCurrent1 = rand() % 50;
    cur1  = String(lfCurrent1);
    Serial.print("Current1 = "); Serial.println(lfCurrent1);

    lfCurrent2 = A2F(12);
//    lfCurrent2 = rand() % 50;
    cur2  = String(lfCurrent2);
    Serial.print("Current2 = "); Serial.println(lfCurrent2);    
    
  }
}

void CheckingWifiTask(void){
  if(wifi_status_event == WL_CONNECTED){
  if(giUpdateWiFiConfig == 1 && wifi_flag == 0 ){

  int ssid_len = gsWifiSsidUpdate.length();
  int pass_len = gsWifiPasswordUpdate.length();

  Wifi_BkUp();
  
  EEPROM.writeByte(STATUS_REG_ADD, STATUS_REG_ADD | STATUS_WFES | STATUS_WFES0 | STATUS_WFES1 | STATUS_WFES2 | STATUS_UWF0);

  
  EEPROM.writeByte(SSID0_SIZE_ADD, ssid_len);
  eep_writeString(SSID0_DATA_ADD, gsWifiSsidUpdate);
  EEPROM.writeByte(PASS0_LEN_ADD, pass_len);

  for(int i = 0; i < 16 - pass_len; ++i){
    gsWifiPasswordUpdate += '0';
  }
  pass_len = gsWifiPasswordUpdate.length();
  char lcPass[pass_len];
  gsWifiPasswordUpdate.toCharArray(lcPass, pass_len + 1);

  Serial.println("WIFI UPDATE: ");
  Serial.print("SSID: "); Serial.println(gsWifiSsidUpdate);
  Serial.print("Password: "); Serial.println(gsWifiPasswordUpdate);
  
  AES_ECB_encrypt(&ctx, (uint8_t*)&lcPass);

  EEPROM.writeByte(PASS0_SIZE_ADD, pass_len);
  for(int i = 0; i < pass_len; i++){
    EEPROM.writeChar(PASS0_DATA_ADD + i, lcPass[i]);
  
  }

  Serial.println("EEPROM WRITE DONE");
  delay(10);

  timerEnd(timer);
  if(EEPROM.commit() == false) Serial.println("FALSE TO COMMIT"); 
    else Serial.println("COMMIT DONE");
  giUpdateWiFiConfig = 0;

  Serial.print("SSID0 size: ");Serial.println(EEPROM.readByte(SSID0_SIZE_ADD));
  Serial.print("SSID0 data: ");Serial.println(eep_readString(SSID0_DATA_ADD, EEPROM.readByte(SSID0_SIZE_ADD)));
  Serial.print("PASS0 size: ");Serial.println(EEPROM.readByte(PASS0_SIZE_ADD));
  Serial.print("PASS0 len: ");Serial.println(EEPROM.readByte(PASS0_LEN_ADD));
  Serial.print("PASS0 data: ");Serial.println(eep_readString(PASS0_DATA_ADD, EEPROM.readByte(PASS0_SIZE_ADD)));

  Serial.print("SSID1 size: ");Serial.println(EEPROM.readByte(SSID1_SIZE_ADD));
  Serial.print("SSID1 data: ");Serial.println(eep_readString(SSID1_DATA_ADD, EEPROM.readByte(SSID1_SIZE_ADD)));
  Serial.print("PASS1 size: ");Serial.println(EEPROM.readByte(PASS1_SIZE_ADD));
  Serial.print("PASS1 len: ");Serial.println(EEPROM.readByte(PASS1_LEN_ADD));
  Serial.print("PASS1 data: ");Serial.println(eep_readString(PASS1_DATA_ADD, EEPROM.readByte(PASS1_SIZE_ADD)));

  Serial.print("SSID2 size: ");Serial.println(EEPROM.readByte(SSID2_SIZE_ADD));
  Serial.print("SSID2 data: ");Serial.println(eep_readString(SSID2_DATA_ADD, EEPROM.readByte(SSID2_SIZE_ADD)));
  Serial.print("PASS2 size: ");Serial.println(EEPROM.readByte(PASS2_SIZE_ADD));
  Serial.print("PASS2 len: ");Serial.println(EEPROM.readByte(PASS2_LEN_ADD));
  Serial.print("PASS2 data: ");Serial.println(eep_readString(PASS2_DATA_ADD, EEPROM.readByte(PASS2_SIZE_ADD)));
  ESP.restart();
  }
  }
}

/*******************************************************************************
* Task WiFi Handler Function definition
*******************************************************************************/

void Wifi_Handle_Event(WiFiEvent_t event, WiFiEventInfo_t info){
  switch (event) {
        case SYSTEM_EVENT_WIFI_READY: 
            Serial.println("WiFi interface ready");
            break;
        case SYSTEM_EVENT_SCAN_DONE:
            Serial.println("Completed scan for access points");
            break;
        case SYSTEM_EVENT_STA_START:
            Serial.println("WiFi client started");
            break;
        case SYSTEM_EVENT_STA_STOP:
            Serial.println("WiFi clients stopped");
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            Serial.println("WiFi connected");
            Serial.println("IP address: ");
            Serial.println(WiFi.localIP());
            WiFi.enableIpV6();
            connectToMqtt();
            wifi_status_event = WiFiStationGotIP(info);
            break;
        case SYSTEM_EVENT_STA_CONNECTED:
            Serial.println("Connected to access point");
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            Serial.println("Disconnected from WiFi access point");
            xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
            xTimerStart(wifiReconnectTimer, 0);
            wifi_status_event = WiFiStationDisconnected(info);
               if( eeprom_commit_call == 1) {
              EEPROM.commit();
              delay(10);
              ESP.restart();
            }
            break;  
        default: break;
    }
}

wl_status_t WiFiStationGotIP(WiFiEventInfo_t info){
    Serial.print("Channel: "); Serial.println(WiFi.channel());
    Serial.print("IP Addr: "); Serial.println(WiFi.localIP());
    Serial.print("Gateway: "); Serial.println(WiFi.gatewayIP());
    Serial.print("Subnet Mask: "); Serial.println(WiFi.subnetMask());
    Serial.print("DNS: "); Serial.println(WiFi.dnsIP());
    String BSSID_Array; BSSID_Array = WiFi.BSSIDstr();
    Serial.println("BSSID: " + BSSID_Array);
    return WL_CONNECTED;
}

wl_status_t WiFiStationDisconnected(WiFiEventInfo_t info){
  uint32_t counterTime;
  if(Disconnected_flag == 0){
    Disconnected_flag = 1;
    Disconnected_timer = 0;
    startTime[WIFI_DISCONNECTED_TIMEOUT] = millis();
  } 
  else {
    if( millis() < startTime[WIFI_DISCONNECTED_TIMEOUT])
      counterTime = millis() + (MILLIS_LIMIT - startTime[WIFI_DISCONNECTED_TIMEOUT]);
    else counterTime = millis() - startTime[WIFI_DISCONNECTED_TIMEOUT];
    
    if(counterTime < 30000){
      Serial.println(counterTime);
      Serial.println("Disconnected to AP!");
     
        Serial.print("SSID Length: ");
        Serial.println(info.connected.ssid_len);
     
        Serial.print("SSID: ");
        for(int i=0; i<info.connected.ssid_len; i++){
          Serial.print((char) info.connected.ssid[i]);
        }
     
        Serial.print("\nBSSID: ");
        for(int i=0; i<6; i++){
          Serial.printf("%02X", info.connected.bssid[i]);
     
          if(i<5){
            Serial.print(":");
          }
        }
         
        Serial.print("\nChannel: ");
        Serial.println(info.connected.channel);
     
        Serial.print("Auth mode: ");
        Serial.println(info.connected.authmode);  
      } else {
      eeprom_commit_call = 1;
      uint8_t status = EEPROM.readByte(STATUS_REG_ADD) & 0x07;
      uint8_t status_reg_bit = EEPROM.readByte(STATUS_REG_ADD) & 0xF8;
      switch(status){
        case 0x01:
          EEPROM.writeByte(STATUS_REG_ADD, status_reg_bit | 0x02);
        break;
        case 0x02:
          EEPROM.writeByte(STATUS_REG_ADD, status_reg_bit | 0x04);
        break;
        case 0x04:
          EEPROM.writeByte(STATUS_REG_ADD, status_reg_bit | 0x01);
        break;
        default:
        break;
      }
    }
  }
  return WL_DISCONNECTED;
}

/*******************************************************************************
* Wifi Function deployment
*******************************************************************************/

void Wifi_BkUp(){
  String str;
  EEPROM.writeByte(STATUS_REG_ADD, STATUS_REG_ADD | STATUS_WFES | STATUS_WFES0 | STATUS_WFES1 | STATUS_WFES2 | STATUS_UWF0);
  
  EEPROM.writeByte(SSID1_SIZE_ADD, EEPROM.readByte(SSID0_SIZE_ADD));
  str = eep_readString(SSID0_DATA_ADD, EEPROM.readByte(SSID0_SIZE_ADD));
  eep_writeString(SSID1_DATA_ADD, str);

  EEPROM.writeByte(PASS1_SIZE_ADD, EEPROM.readByte(PASS0_SIZE_ADD));

  EEPROM.writeByte(PASS1_LEN_ADD, EEPROM.readByte(PASS0_LEN_ADD));
  str = eep_readString(PASS0_DATA_ADD,EEPROM.readByte(PASS0_SIZE_ADD));
  eep_writeString(PASS1_DATA_ADD, str);
}


int getWiFi_INFO(int ssid_size_add, int pass_size_add){

  if((EEPROM.readByte(STATUS_REG_ADD) & 0xFF) != WIFI_INFO_EMPTY){

  int ssid_size = EEPROM.readByte(ssid_size_add);
  int ssid_data_add = ssid_size_add + 0x01;
  int pass_size = EEPROM.readByte(pass_size_add);
  int pass_len = EEPROM.readByte(pass_size_add + 1);
  int pass_data_add = pass_size_add + 0x02;
  
  String lsWifiEpassword;
    for(int i = 0; i < ssid_size; i++){
        gsWifiEssid += char(EEPROM.read(ssid_data_add + i));
    
    }

    Serial.println(pass_size);
    for(int i = 0; i < pass_size; i++){
        lsWifiEpassword += char(EEPROM.read(pass_data_add +i));

    }

    AES_ECB_decrypt(&ctx, (uint8_t*)lsWifiEpassword.c_str());
    char lcPass[pass_len];
    lsWifiEpassword.toCharArray(lcPass, pass_len + 1);
    
    for(int i = 0; i < pass_len; ++i){
      gsWifiEpassword += lcPass[i];
    }
    Serial.println(gsWifiEpassword);

  } else
  if((EEPROM.readByte(STATUS_REG_ADD) & 0xFF) == WIFI_INFO_EMPTY){

    return WIFI_INFO_EMPTY;
  }

  return STATUS_WFES;
}



void ConnectWifi(String ssid, String password)
{
  unsigned int luiCountSSID = 0;
  uint8_t lucLoopCondition = 0;
  String lsSsid;
  String lsPass;
  /* Checking the wifi is available before */
  lucLoopCondition = 1;
  do
  {
    Serial.println("Scan start!");
    luiCountSSID = WiFi.scanNetworks();
    Serial.print("Scan done! ");
    if (luiCountSSID == 0) 
    {
      Serial.println("no networks found!");
    } 
    else
    {
      Serial.print(luiCountSSID);
      Serial.println(" networks found!");
      for (int index = 0; index < luiCountSSID; index++) 
      {
        if( WiFi.SSID(index) == ssid)
        {
          Serial.println("SSID networks found");
          /* End loop */
          lucLoopCondition = 0;
        }
          lucLoopCondition = 0;
      }
    }
    /* Display LED signal  */
    digitalWrite(LED1_IO, HIGH);
    digitalWrite(LED2_IO, LOW);
    delay(100);
    digitalWrite(LED1_IO, LOW);
    digitalWrite(LED2_IO, HIGH);
    delay(100);
    digitalWrite(LED1_IO, HIGH);
    digitalWrite(LED2_IO, LOW);
    delay(100);
    digitalWrite(LED1_IO, LOW);
    digitalWrite(LED2_IO, HIGH);
    delay(100);
  }
  while(lucLoopCondition == 1);

  /* Connect to wifi */
  uint8_t status = EEPROM.readByte(STATUS_REG_ADD) & 0x3F;
  uint8_t wifi_avail = WIFI_INFO_EMPTY ;
  
  if((status & STATUS_WFES0 == STATUS_WFES0) && (status & STATUS_UWF0 == STATUS_UWF0)){
    wifi_avail = getWiFi_INFO(SSID0_SIZE_ADD, PASS0_SIZE_ADD);
    Serial.println("Connect to WiFi 0");
  } 
  else if((status & STATUS_WFES1 == STATUS_WFES1) && (status & STATUS_UWF1 == STATUS_UWF1)){
    wifi_avail = getWiFi_INFO(SSID1_SIZE_ADD, PASS1_SIZE_ADD);
    Serial.println("Connect to WiFi 1");
  }
  else if((status & STATUS_WFES2 == STATUS_WFES2) && (status & STATUS_UWF2 == STATUS_UWF2)){
    wifi_avail = getWiFi_INFO(SSID2_SIZE_ADD, PASS2_SIZE_ADD);
    Serial.println("Connect to WiFi 2");
  }

   if(wifi_avail == STATUS_WFES){
    Serial.println("WIFI IS EXIST");
    Serial.print("Connecting to ");
    Serial.println(gsWifiEssid);
    WiFi.begin(gsWifiEssid.c_str(), gsWifiEpassword.c_str());

    gsWifiSsid_connected = gsWifiEssid;
  } else
  if(wifi_avail == WIFI_INFO_EMPTY){
    Serial.println("WIFI IS EMPTY");
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid.c_str(), password.c_str());

    gsWifiSsid_connected = ssid;
  }

/************************************************************/

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    /* Display LED signal */
    digitalWrite(LED1_IO, !digitalRead(LED1_IO));
    digitalWrite(LED2_IO, !digitalRead(LED2_IO));
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  
  if(EEPROM.readByte(STATUS_REG_ADD) == WIFI_INFO_EMPTY){
    EEPROM.writeByte(STATUS_REG_ADD, STATUS_REG_ADD | STATUS_WFES | STATUS_WFES0 | STATUS_WFES2 | STATUS_UWF0);

    EEPROM.writeByte(SSID0_SIZE_ADD, ssid.length());
    eep_writeString(SSID0_DATA_ADD, ssid);
    
    EEPROM.writeByte(SSID2_SIZE_ADD, ssid.length());
    eep_writeString(SSID2_DATA_ADD, ssid);

    EEPROM.commit();
    
    int len = password.length();
    EEPROM.writeByte(PASS0_LEN_ADD, len);
    EEPROM.writeByte(PASS2_LEN_ADD, len);
    char lcPass[len];
    password.toCharArray(lcPass, len+1);
    for(int i = strlen(lcPass) ; i < 16; ++i){
        lcPass[i] += (char)'0';
        ++len;
    }
    AES_ECB_encrypt(&ctx, (uint8_t*)&lcPass);


    EEPROM.writeByte(PASS0_SIZE_ADD, len);
    EEPROM.writeByte(PASS2_SIZE_ADD, len);
    
    for(int i = 0; i < len; i++){
      EEPROM.writeChar(PASS0_DATA_ADD + i, lcPass[i]);
      EEPROM.writeChar(PASS2_DATA_ADD + i, lcPass[i]);
    
    }
    EEPROM.commit();
    
  }
}

/*******************************************************************************
* MCU Function deployment (SPI, Flash Memory, GPIO)
*******************************************************************************/
byte transferAndWait(byte c){
  byte a = vspi->transfer(c);
  delay(10);

  return a;
}

float Read_vspi() {
  float MasterReceive = 0x00;
  //use it as you would the regular arduino SPI API
  vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE3));
  digitalWrite(VSPI_SSL_IO, LOW); //pull SS slow to prep other end for transfer
  #if(TEST_MODULE == STD_ON)
    transferAndWait(0x01);
  #endif
  transferAndWait(0x04);

  for(int i = 0; i < 21; i++){
    spi_data.b[i] = transferAndWait(i);
  }

  digitalWrite(VSPI_SSL_IO, HIGH); //pull ss high to signify end of data transfer
  vspi->endTransaction();

  return MasterReceive;
}

String eep_readString(uint16_t start_add, uint16_t size_add){
  String str;

    for(int i = 0; i < size_add; i++){
      str += char(EEPROM.readByte(start_add + i));

    }
  
  return str;
}

int eep_writeString(uint16_t start_add, String str){
  int len = str.length();
  char lcChar[len];
  
  str.toCharArray(lcChar, len + 1);

  for(int i = 0 ; i < len; i++){  
    EEPROM.writeChar(start_add + i, lcChar[i]);

  }

}

void TurnOn_Relay(uint8_t RelayOn){
  digitalWrite(RelayOn, RELAY_ACTIVE);
  digitalWrite(WE_IO, LOW);
  delay(1);
  digitalWrite(WE_IO, HIGH);
  delay(1);
  digitalWrite(WE_IO, LOW);
  if(RelayOn == RELAY1_IO){
    status1 = "ON";
    Serial.println("ReLay1: ON");
  } else 
  if(RelayOn == RELAY2_IO) {
    status2 = "ON";
    Serial.println("Relay2: ON");
  }
}
void TurnOff_Relay(uint8_t RelayOff){
  digitalWrite(RelayOff, RELAY_DEACTIVE);
  digitalWrite(WE_IO, LOW);
  delay(1);
  digitalWrite(WE_IO, HIGH);
  delay(1);
  digitalWrite(WE_IO, LOW);
  if(RelayOff == RELAY1_IO){
    status1 = "OFF";
    Serial.println("ReLay1: OFF");  
  } else
  if(RelayOff == RELAY2_IO){
    status2 = "OFF";
    Serial.println("ReLay2: OFF");
  }
}