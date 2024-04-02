#include "data_processing.h"


void dataProcessing(char **p_dados, uint16_t *tamanhoStr)
{
  uint8_t tombo = digitalRead(34);
  uint8_t bateria = leituraBat();
  uint16_t distancia = leituraSensor();
  float latitude = 0;
  float longitude = 0;

  getGPS(&latitude, &longitude);

  *tamanhoStr = snprintf(NULL, 0, "%03d%04d%03.6f%03.6f%d", bateria, distancia, latitude, longitude, tombo);
  *p_dados = (char*)malloc((*tamanhoStr + 1) * sizeof(char));
  if(*p_dados == NULL)
  {
    Serial.println("Erro ao alocar memoria");
    delay(1000);
    ESP.restart();
  }
  else
  {
    snprintf(*p_dados, *tamanhoStr + 1, "%03d%04d%03.6f%03.6f%d", bateria, distancia, latitude, longitude, tombo);
    Serial.println(*p_dados);
  }
  
}