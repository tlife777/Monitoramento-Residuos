#ifndef DEFINES_H
#define DEFINES_H

#define BAUDRATE_SERIAL_DEBUG   115200

/* Definições do rádio LoRa (SX1276) */
#define GANHO_LORA_DBM          20 //dBm

#define RADIO_RESET_PORT 12
#define RADIO_MOSI_PORT 23
#define RADIO_MISO_PORT 19
#define RADIO_SCLK_PORT 18
#define RADIO_NSS_PORT 5
#define RADIO_DIO0_PORT 27
#define RADIO_DIO1_PORT 26
#define RADIO_DIO2_PORT 25

#define uS_TO_S_FACTOR 1000000

#define SLEEP_TIME 30 //minutes

#endif