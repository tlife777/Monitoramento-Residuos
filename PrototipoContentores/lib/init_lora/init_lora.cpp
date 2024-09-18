#include <LoRa.h> 
#include "init_lora.h"
#include "utils.h"

void init_lora() {
    // Inicializa o LoRa SPI
    LoRa.setPins(RADIO_NSS_PORT, RADIO_RESET_PORT, RADIO_DIO0_PORT);

    if (!LoRa.begin(915E6)) {
        Serial.println("Erro ao iniciar o LoRa");
        while (true);
    }

    // Configurações do LoRa
    LoRa.setCodingRate4(5);         // Coding rate 4/5
    LoRa.setSpreadingFactor(7);     // Spreading factor 7
    LoRa.setSignalBandwidth(125E3); // Bandwidth de 125 kHz
    
    Serial.println("LoRa inicializado com sucesso");
}

// Função para enviar dados via LoRa
void enviarDadosLoRa(const char* dados) {
    LoRa.beginPacket();     // Inicia o pacote LoRa
    LoRa.print(dados);       // Adiciona os dados ao pacote
    LoRa.endPacket();       // Envia o pacote
    Serial.println("Dados enviados com sucesso.");
}
