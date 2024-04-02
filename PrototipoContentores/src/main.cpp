#include "LoRaWan.h"
#include "defines.h"
#include "sensors_read.h"
#include "sleep.h"


/* Constantes do LMIC */
const lmic_pinmap lmic_pins = {
    .nss = RADIO_NSS_PORT,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = RADIO_RESET_PORT,
    .dio = {RADIO_DIO0_PORT, RADIO_DIO1_PORT, RADIO_DIO2_PORT},
};


RTC_DATA_ATTR bool sleepWakeup = false;
RTC_DATA_ATTR uint16_t cont = 0;


void setup() 
{
    init_sensors();
    
    Serial.begin(BAUDRATE_SERIAL_DEBUG);
    
    init_lorawan();
}

void loop() 
{
  os_runloop_once();    
}





