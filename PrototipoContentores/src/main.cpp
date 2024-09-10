#include "sensors.h"
#include "init_lora.h"
#include "utils.h"
#include "data_collector.h" 

TinyGPSPlus gps;    // Inicializa o módulo GPS
VL53L0X sensor;     // Inicializa o sensor de distância a laser

// Declaração de funções (protótipos) para que o compilador saiba que elas existem antes de serem chamadas
void init_sensors();
void init_lora();
void coletar_dados(char** dados, uint16_t* tamanhoStr);
void enviarDadosLoRa(const char* dados);

void setup() {
    Serial.begin(BAUDRATE_SERIAL_DEBUG);  // Inicializa o monitor serial

    Serial.println("Iniciando o setup..."); // Adiciona log para depuração

    // Inicializa os sensores
    init_sensors();
    Serial.println("Sensores inicializados."); // Adiciona log para depuração

    // Inicializa o módulo LoRa
    init_lora();
    Serial.println("LoRa inicializado."); // Adiciona log para depuração
}

void loop() {
    char* dados = nullptr;
    uint16_t tamanhoStr = 0;

    // Coleta e processa dados
    coletar_dados(&dados, &tamanhoStr);

    // Verifica se os dados são válidos antes de enviar
    if (dados != nullptr) {
        enviarDadosLoRa(dados);
        free(dados); // Libera a memória alocada
    } else {
        Serial.println("Esperando por dados de GPS válidos...");
    }

    delay(30000); // Aguarda 30 segundos antes de coletar e enviar dados novamente
}
