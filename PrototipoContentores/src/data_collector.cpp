// data_collector.cpp
#include "data_collector.h"
#include "sensors.h"
#include "init_lora.h"
#include "utils.h"

void coletar_dados(char **p_dados, uint16_t *tamanhoStr) {
    uint8_t batery = leituraBat();
    uint16_t volume = leituraSensor();
    double latitude = -23.470270;
    double longitude = -47.429062;

    // Formatação da string de envio
    *tamanhoStr = snprintf(nullptr, 0, "%03d,%04d,%03.6f,%03.6f", batery, volume, latitude, longitude);
    *p_dados = (char*)malloc((*tamanhoStr + 1) * sizeof(char));
    if (*p_dados == nullptr) {
        Serial.println("Erro ao alocar memória");
        delay(1000);
        ESP.restart();
    } else {
        snprintf(*p_dados, *tamanhoStr + 1, "%03d,%04d,%03.6f,%03.6f", batery, volume, latitude, longitude);
        Serial.println(*p_dados);
    }
}
