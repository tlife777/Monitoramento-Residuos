#ifndef SENSORS_READ_H
#define SENSORS_READ_H

#include "Arduino.h"
#include "stdint.h"
#include <VL53L0X.h>
#include <TinyGPSPlus.h>

VL53L0X sensor;
TinyGPSPlus gps;

void init_sensors(void);

uint8_t leituraBat(void);
uint16_t leituraSensor(void);
void getGPS(float *p_lat, float *p_lon);



#endif