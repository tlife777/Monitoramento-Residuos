#include "sensors.h"
#include "utils.h"
#include "Arduino.h"

#define R1 30000.00
#define R2 100000.00

#define MAX_VOLTAGE 4.2
#define MIN_VOLTAGE 3.5

// Inicializa os sensores
void initSensors() {
    Serial1.begin(9600, SERIAL_8N1, 16, 17);  // Inicializa a comunicação serial com o GPS

    pinMode(4, INPUT_PULLDOWN);
    pinMode(32, OUTPUT);
    pinMode(33, OUTPUT);
    pinMode(34, INPUT);

    digitalWrite(33, LOW);
    digitalWrite(32, HIGH);

    // Inicializa o sensor de distância a laser
    Wire.begin();
    sensor.setTimeout(500);
    if (!sensor.init()) {
        Serial.println("Falha ao iniciar o sensor");
        delay(1000);
        ESP.restart();
    }
    sensor.setSignalRateLimit(0.1);
    sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
    sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
    sensor.setMeasurementTimingBudget(200000);
}

// Função para leitura do nível de bateria
uint8_t leituraBat() {
    int batReading = analogReadMilliVolts(BATTERY_PIN);
    int batPorcent;
    int batVoltage;
    float batVoltageBattery;

    batVoltage = (((float)batReading) / 4095) * 3.3;

    batVoltageBattery = batVoltage * (R1 + R2) / R2;

    Serial.printf("Tensão medida: %f V\n", batVoltageBattery);

    batPorcent = (((batVoltageBattery - MIN_VOLTAGE)/ (MAX_VOLTAGE - MIN_VOLTAGE))* 100.0);

    if(batPorcent>100)
        batPorcent = 100;
    if(batPorcent< 0)
        batPorcent = 0;

    Serial.print(batPorcent);
    Serial.println("%");
    return batPorcent;
}

// Função para leitura do sensor de distância
uint16_t leituraSensor() {
    unsigned long contSensor = 0;
    for (int i = 0; i < 10; i++) {
        contSensor += sensor.readRangeSingleMillimeters();
    }
    Serial.print("Distância média: ");
    Serial.print(contSensor / 10);
    Serial.println("mm");
    return contSensor / 10;
}
