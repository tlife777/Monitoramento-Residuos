#include "sleep.h"

void espSleep(unsigned int secondsSleep)
{
  esp_sleep_enable_timer_wakeup(secondsSleep * uS_TO_S_FACTOR);
  //esp_sleep_enable_ext0_wakeup(GPIO_NUM_34, 1); //1 = High, 0 = Low
  Serial.println("Dormindo...");
  esp_deep_sleep_start();
}