#ifndef _GRAPEREMOTETYPE_H_
#define _GRAPEREMOTETYPE_H_

/*******************************************************************************
* Feature status definition
*******************************************************************************/
#define STD_ON         (1)
#define STD_OFF        (0)

#define RELAY_NO       HIGH
#define RELAY_NC       LOW


enum bit_state{
  BIT_DEACTIVE = 0,
  BIT_ACTIVE 
};
static union first_union{
  float f;
  byte b[21];
} spi_data;

struct WIFI_STATUS_REG {
  bit_state WFES;
  bit_state WFES0;
  bit_state WFES1;
  bit_state WFES2;
  bit_state UWF0;
  bit_state UWF1;
  bit_state UWF2;
};
#endif /* _GRAPEREMOTETYPE_H_ */