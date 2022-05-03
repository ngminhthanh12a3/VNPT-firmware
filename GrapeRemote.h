#ifndef _GRAPEREMOTE_H_
#define _GRAPEREMOTE_H_

#include "GrapeRemoteType.h"

/*******************************************************************************
* Feature definition
*******************************************************************************/
#define TEST_FEATURE            STD_OFF
#define USE_ADC_LUT             STD_OFF
#define FIREBASE_PRINT_LOG      STD_OFF
#define FIREBASE_USE_CALLBACK   STD_OFF
#define USE_CONFIG_BOOT         STD_OFF
#define SHOW_CONFIG             STD_OFF
#define WIFI_SMART_CONFIG       STD_OFF
#define TEST_MODULE             STD_OFF
#define UNION_FORMAT            STD_ON

/*******************************************************************************
* Private definition
*******************************************************************************/

#define RELAY_ACTIVE            RELAY_NO
#define RELAY_DEACTIVE          RELAY_NC

#define DYNAMO_ON               0x01
#define DYNAMO_OFF              0x00

#define RELAY_ON                1
#define RELAY_OFF               0

#define CURRENT_SENSOR_ACTIVE   1
#define CURRENT_SENSOR_DEACTIVE 0

#define UPDATE_WIFI_CALL        1
#define UPDATE_WIFI_NO_CALL     0

#define RELAY_AVAILABLE         0
#define RELAY_UNAVAILABLE       1         

#define FILTER_N                30
#define MILLIS_LIMIT            0xFFFFFFFF - 1

/*******************************************************************************
* WiFi and Server definition
*******************************************************************************/
#if(USE_CONFIG_BOOT == STD_OFF) // Config by Code
  
#define WIFI_SSID "KHCN1"
#define WIFI_PASSWORD "deslab2610"
//#define WIFI_SSID "ThanhNguyen"
//#define WIFI_PASSWORD "nguyent1220"

#else //Config by Script file


#endif
/*******************************************************************************
* Scheduler definition
*******************************************************************************/

#define CURRENT_TASK_TIME       2000
#define VOLTAGE_TASK_TIME       2000
#define TEMPERATURE_TASK_TIME   2000
#define CHECKING_WIFI_TASK_TIME 3000
#define RELAY_TASK_TIME         1000
#define DYNAMO_TASK_TIME        500
#define SWITCH_TIME_CALL        10800 //180 mins
#define OVERHEAT_TIME           150   //5 mins

#define WIFI_DISCONNECTED_TIMEOUT 0

#define TEMPERATURE_OVERHEAT    32
/*******************************************************************************
* IO definition
*******************************************************************************/
#if (TEST_MODULE == STD_ON)

#define RESET_IO          16
#define UPDATE_IO         17

#define LED1_IO           26
#define LED2_IO           27

#define RELAY1_IO         21  
#define RELAY2_IO         22
#define WE_IO             25
  
#define VSPI_SSL_IO        5
#define VSPI_CLOCK_IO      18
#define VSPI_MISO_IO       19
#define VSPI_MOSI_IO       23

#else

#define RESET_IO          16
#define UPDATE_IO         17

#define LED1_IO           21
#define LED2_IO           22

#define RELAY1_IO         26  
#define RELAY2_IO         27
#define WE_IO             25
  
#define VSPI_SSL_IO       5
#define VSPI_CLOCK_IO     18
#define VSPI_MISO_IO      19
#define VSPI_MOSI_IO      23

#endif
/*******************************************************************************
* Blynk definition
*******************************************************************************/
/*
#define RELAY1_VIRTUAL_PIN            V1
#define VOL1_VIRTUAL_PIN              V2
#define CUR1_VIRTUAL_PIN              V3
#define RELAY2_VIRTUAL_PIN            V4
#define VOL2_VIRTUAL_PIN              V5
#define CUR2_VIRTUAL_PIN              V6
#define TEMP_VIRTUAL_PIN              V7
#define UPDATEWIFI_VIRTUAL_PIN        V8
#define SSIDWIFI_VIRTUAL_PIN          V9
#define PASSWIFI_VIRTUAL_PIN          V10
#define DYNAMO_VIRTUAL_PIN            V11
#define CONNECTED_SSID_VIRTUAL_PIN    V12   
#define RELAY1_WARNING_VIRTUAL_PIN    V13
#define RELAY2_WARNING_VIRTUAL_PIN    V14 
#define BLYNK_PORT              8080
*/
/*******************************************************************************
* AES and EEPROM definition
*******************************************************************************/
#define STATUS_REG_ADD              0x00

#define STATUS_WFES             0x00 // Wifi is Exist
// UWFx bit: Using Wifi x
#define STATUS_UWF0             0x01 // Using Wifi 0
#define STATUS_UWF1             0x02
#define STATUS_UWF2             0x04
// WFESx bit: Wifi x is Exist (Available)
#define STATUS_WFES0            0x08 
#define STATUS_WFES1            0x10
#define STATUS_WFES2            0x20
//#define STATUS_UWF0             0x40
#define WIFI_INFO_EMPTY         0xFF
#define WIFI_INFO_AVAILABLE     0x01
#define WIFI_INFO_UNAVAILABLE   0x00

#define SSID0_SIZE_ADD          0x0001
#define SSID0_DATA_ADD          0x0002
#define PASS0_SIZE_ADD          0x0041
#define PASS0_LEN_ADD           0x0042
#define PASS0_DATA_ADD          0x0043

#define SSID1_SIZE_ADD          0x0081
#define SSID1_DATA_ADD          0x0082
#define PASS1_SIZE_ADD          0x00C1
#define PASS1_LEN_ADD           0x00C2
#define PASS1_DATA_ADD          0x00C3

#define SSID2_SIZE_ADD          0x0101
#define SSID2_DATA_ADD          0x0102
#define PASS2_SIZE_ADD          0x0141
#define PASS2_LEN_ADD           0x0142
#define PASS2_DATA_ADD          0x0143

#define EEPROM_SIZE             512

#define CBC 1
#define CTR 1
#define ECB 1

#endif /* _GRAPEREMOTE_H_ */
