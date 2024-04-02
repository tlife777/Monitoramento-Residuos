#ifndef LORAWAN_H
#define LORAWAN_H

#include <Arduino.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include "defines.h"
#include "sensors_read.h"
#include "data_processing.h"



static osjob_t sendjob;
const unsigned TX_INTERVAL = 1;

static const PROGMEM u1_t NWKSKEY[16] = { 0x51, 0xA1, 0x58, 0x94, 0x25, 0x46, 0x31, 0x42, 0x02, 0x35, 0x15, 0x35, 0x89, 0x46, 0x32, 0x51 }; //coloque aqui sua network session key
static const u1_t PROGMEM APPSKEY[16] = { 0xFA, 0xCE, 0x15, 0x44, 0x41, 0x86, 0x45, 0x31, 0x85, 0x32, 0x64, 0x86, 0x34, 0x51, 0x84, 0x35 }; //coloque aqui sua application session key
static const u4_t DEVADDR = 0xF3E35972;//coloque aqui seu device address

void init_lorawan(void);


void onEvent(ev_t ev);
void do_send(osjob_t* j);


#endif