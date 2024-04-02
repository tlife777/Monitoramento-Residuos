#include "sensors_read.h"

void init_sensors(void)
{
  Serial1.begin(9600, SERIAL_8N1, 16, 17);

  pinMode(4, INPUT_PULLDOWN);
  pinMode(32, OUTPUT);
  pinMode(33, OUTPUT);
  pinMode(34, INPUT);
  
  


  digitalWrite(33, LOW);
  digitalWrite(32, HIGH);
  Wire.begin();
  sensor.setTimeout(500);
  if(!sensor.init())
  {
    Serial.println("Falha ao iniciar o sensor");
    delay(1000);
    ESP.restart();
  }

  sensor.setSignalRateLimit(0.1);
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
  sensor.setMeasurementTimingBudget(200000);

  digitalWrite(33, HIGH);
}

uint8_t leituraBat(void)
{
  unsigned int batSoma = 0;
  byte batPorcent = 0;
  for(int i = 0; i < 50; i++)
  {
    batSoma += analogRead(35);
  }
  batPorcent = map(batSoma/50, 3755, 2555, 100, 0);
  if(batPorcent > 100)
  {
    batPorcent = 100;
    Serial.print("ALERTA! Passou de 100% - ");
  }
    
  if(batPorcent < 0)
    batPorcent = 0;
  Serial.print(batPorcent);
  Serial.println("%");
  batSoma = 0;
  return batPorcent;
}

uint16_t leituraSensor(void)
{
  static int valorAnterior = 0;
  unsigned long contSensor = 0;
  for(int i = 0; i < 10; i++)
  {
      contSensor += sensor.readRangeSingleMillimeters();
  }
  Serial.print("Distancia média: ");
  Serial.print(contSensor/10);
  Serial.println("mm");
  return contSensor/10;
}
