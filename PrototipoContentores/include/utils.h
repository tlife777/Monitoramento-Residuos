#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

// Definições gerais
#define BAUDRATE_SERIAL_DEBUG 115200

/* Definições do rádio LoRa (SX1276) */
#define GANHO_LORA_DBM 20 // dBm

// Definições de pinos para o módulo LoRa
#define RADIO_RESET_PORT 12
#define RADIO_MOSI_PORT 23
#define RADIO_MISO_PORT 19
#define RADIO_SCLK_PORT 18
#define RADIO_NSS_PORT 5
#define RADIO_DIO0_PORT 27  
#define RADIO_DIO1_PORT 26
#define RADIO_DIO2_PORT 25
// #define FREQUENCY 915E6

// Definições de pinos para sensores
#define BATTERY_PIN 35

#endif 
