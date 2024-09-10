#include "sensors.h"
#include "utils.h"

// Inicializa os sensores
void init_sensors() {
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
    long batSoma = 0;
    uint8_t batPorcent;
    for (int i = 0; i < 50; i++) {
        batSoma += analogRead(BATTERY_PIN);
    }
    batPorcent = map(batSoma / 50, 3755, 2555, 100, 0);
    batPorcent = constrain(batPorcent, 0, 100);
    Serial.print("Bateria: ");
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
