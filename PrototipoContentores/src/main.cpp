#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <TinyGPSPlus.h>
/* Definições gerais */
#define BAUDRATE_SERIAL_DEBUG   115200

/* Definições do rádio LoRa (SX1276) */
#define GANHO_LORA_DBM          20 //dBm

#define RADIO_RESET_PORT 12
#define RADIO_MOSI_PORT 23
#define RADIO_MISO_PORT 19
#define RADIO_SCLK_PORT 18
#define RADIO_NSS_PORT 5
#define RADIO_DIO0_PORT 27
#define RADIO_DIO1_PORT 26
#define RADIO_DIO2_PORT 25

#define uS_TO_S_FACTOR 1000000

byte leituraBat(void);
unsigned int leituraSensor(void);
void configSensor(void);
void onEvent (ev_t ev);
void do_send(osjob_t* j);
void dadosSensores(char **p_dados, int *tamanhoStr);
void dadosGPS(char **p_dados, int *tamanhoStr);
void getGPS(float *p_lat, float *p_lon);
void espSleep(unsigned int secondsSleep);
float getTemperature(void);
bool checkFall(bool checkSleep);


/* Constantes do LMIC */
const lmic_pinmap lmic_pins = {
    .nss = RADIO_NSS_PORT,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = RADIO_RESET_PORT,
    .dio = {RADIO_DIO0_PORT, RADIO_DIO1_PORT, RADIO_DIO2_PORT},
};

/* Constantes do LoraWAN */
/* - Chaves (network e application keys) */
static const PROGMEM u1_t NWKSKEY[16] = { 0x51, 0xA1, 0x58, 0x94, 0x25, 0x46, 0x31, 0x42, 0x02, 0x35, 0x15, 0x35, 0x89, 0x46, 0x32, 0x51 }; //coloque aqui sua network session key
static const u1_t PROGMEM APPSKEY[16] = { 0xFA, 0xCE, 0x15, 0x44, 0x41, 0x86, 0x45, 0x31, 0x85, 0x32, 0x64, 0x86, 0x34, 0x51, 0x84, 0x35 }; //coloque aqui sua application session key

/* - Device Address */
static const u4_t DEVADDR = 0xF3E35972;

/* - Tempo entre envios de pacotes LoRa */
const unsigned TX_INTERVAL = 1; //1800s = 30 minutos 

/* Variáveis e objetos globais */

static osjob_t sendjob;
VL53L0X sensor;
TinyGPSPlus gps;
bool trocaDados = false;
RTC_DATA_ATTR bool sleepWakeup = false;
RTC_DATA_ATTR uint16_t cont = 0;



void os_getArtEui (u1_t* buf) {}

void os_getDevEui (u1_t* buf) {}

void os_getDevKey (u1_t* buf) {}


void onEvent (ev_t ev) 
{
    Serial.print(os_getTime());
    Serial.print(": ");
    Serial.println(ev);
    
    switch(ev) 
    {
        case EV_SCAN_TIMEOUT:
            Serial.println(F("EV_SCAN_TIMEOUT"));
            break;
        case EV_BEACON_FOUND:
            Serial.println(F("EV_BEACON_FOUND"));
            break;
        case EV_BEACON_MISSED:
            Serial.println(F("EV_BEACON_MISSED"));
            break;
        case EV_BEACON_TRACKED:
            Serial.println(F("EV_BEACON_TRACKED"));
            break;
        case EV_JOINING:
            Serial.println(F("EV_JOINING"));
            break;
        case EV_JOINED:
            Serial.println(F("EV_JOINED"));
            break;
        case EV_JOIN_FAILED:
            Serial.println(F("EV_JOIN_FAILED"));
            break;
        case EV_REJOIN_FAILED:
            Serial.println(F("EV_REJOIN_FAILED"));
            break;
        case EV_TXCOMPLETE:
            Serial.println (millis());
            Serial.println(F("EV_TXCOMPLETE (incluindo espera pelas janelas de recepção)"));

            if (LMIC.txrxFlags & TXRX_ACK)
              Serial.println(F("Ack recebido"));

            if (LMIC.dataLen) 
            {
                Serial.println(F("Recebidos "));
                Serial.println(LMIC.dataLen);
                Serial.println(F(" bytes (payload) do gateway"));
              
                if (LMIC.dataLen == 1) 
                {
                    uint8_t dados_recebidos = LMIC.frame[LMIC.dataBeg + 0];
                    Serial.print(F("Dados recebidos: "));
                    Serial.write(dados_recebidos);
                }

                delay(100);
             
            }
            
            os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);
            if(cont == 1)
            {
              if(trocaDados == false)
                trocaDados = true;
              else{
                trocaDados = false;
                LMIC_shutdown();
                espSleep(60);
              }
            }else
              espSleep(60);
            
            break;

        case EV_LOST_TSYNC:
            Serial.println(F("EV_LOST_TSYNC"));
            break;
        case EV_RESET:
            Serial.println(F("EV_RESET"));
            break;
        case EV_RXCOMPLETE:
            Serial.println(F("EV_RXCOMPLETE"));
            break;
        case EV_LINK_DEAD:
            Serial.println(F("EV_LINK_DEAD"));
            break;
        case EV_LINK_ALIVE:
            Serial.println(F("EV_LINK_ALIVE"));
            break;
        case EV_TXSTART:
            Serial.println(F("EV_TXSTART"));
            Serial.println (millis());
            Serial.println(LMIC.freq);
            break;
        default:
            Serial.print(F("Evento desconhecido: "));
            Serial.println((unsigned) ev);
            break;
    }
}

void do_send(osjob_t* j)
{
    char *p_dados = NULL;
    int tamanhoStr = 0;

    if(trocaDados == false)
      dadosSensores(&p_dados, &tamanhoStr);
    else
      dadosGPS(&p_dados, &tamanhoStr);

    
    if (LMIC.opmode & OP_TXRXPEND) 
    {
        Serial.println(F("OP_TXRXPEND: ha um envio ja pendente, portanto o atual envio nao sera feito"));
    } 
    else 
    {
        LMIC_setTxData2(4, (uint8_t*)p_dados, tamanhoStr, 0);
        Serial.println(F("Pacote LoRaWAN na fila de envio."));       
    }
    tamanhoStr = 0;
    free(p_dados);
}

void setup() 
{
    uint8_t appskey[sizeof(APPSKEY)];
    uint8_t nwkskey[sizeof(NWKSKEY)];
    int b;
    
    Serial.begin(BAUDRATE_SERIAL_DEBUG);
    Serial1.begin(9600, SERIAL_8N1, 16, 17);
    
    pinMode(4, INPUT_PULLDOWN);
    pinMode(32, OUTPUT);
    pinMode(33, OUTPUT);
    pinMode(34, INPUT);
    
    

    while(checkFall(sleepWakeup) == false)

    digitalWrite(33, LOW);
    digitalWrite(32, HIGH);
    
    ++cont;
    Serial.print("Contador: ");
    Serial.println(cont);
    if(cont >= 10)
      cont = 0;


    Wire.begin();
    configSensor();
    digitalWrite(33, HIGH);


    SPI.begin(RADIO_SCLK_PORT, RADIO_MISO_PORT, RADIO_MOSI_PORT);

    os_init();
    LMIC_reset();

    memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
    memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
    LMIC_setSession (0x13, DEVADDR, nwkskey, appskey);

    for (b=0; b<8; ++b) 
        LMIC_disableSubBand(b);

    LMIC_enableChannel(0); // 915.2 MHz
    LMIC_enableChannel(1); // 915.4 MHz
    LMIC_enableChannel(2); // 915.6 MHz
    LMIC_enableChannel(3); // 915.8 MHz
    LMIC_enableChannel(4); // 916.0 MHz
    LMIC_enableChannel(5); // 916.2 MHz
    LMIC_enableChannel(6); // 916.4 MHz
    LMIC_enableChannel(7); // 916.6 MHz

    LMIC_setAdrMode(0);
    LMIC_setLinkCheckMode(0);

    LMIC.dn2Dr = DR_SF12CR;


    LMIC_setDrTxpow(DR_SF12, GANHO_LORA_DBM);

    do_send(&sendjob);
}

void loop() 
{
  bool gpsDataReceived = false;
  if(cont == 1)
    while(!gpsDataReceived)
    {
      while(Serial1.available() > 0)
        if(gps.encode(Serial1.read()))
          if(gps.location.isValid())
            gpsDataReceived = true;
    }
  
  os_runloop_once();    
}

void configSensor(void)
{
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
}

byte leituraBat(void)
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

unsigned int leituraSensor(void)
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

void dadosSensores(char **p_dados, int *tamanhoStr)
{
  uint8_t bateria = leituraBat();
  uint16_t distancia = leituraSensor();
  
  *tamanhoStr = snprintf(NULL, 0, "{\"bat\": %d, \"dist\": %d}", bateria, distancia);
  *p_dados = (char*)malloc((*tamanhoStr + 1) * sizeof(char));
  if(*p_dados == NULL)
  {
    Serial.println("Erro ao alocar memoria");
    delay(1000);
    ESP.restart();
  }
  else
  {
    snprintf(*p_dados, *tamanhoStr + 1, "{\"bat\": %d, \"dist\": %d}", bateria, distancia);
    Serial.println(*p_dados);
  }
}

void dadosGPS(char **p_dados, int *tamanhoStr)
{
  float latitude = 0;
  float longitude = 0;

  
  getGPS(&latitude, &longitude);

  *tamanhoStr = snprintf(NULL, 0, "{\"lat\": %.6f, \"lon\": %.6f}", latitude, longitude);
  *p_dados = (char*)malloc((*tamanhoStr + 1) * sizeof(char));
  if(*p_dados == NULL)
  {
    Serial.println("Erro ao alocar memoria");
    delay(1000);
    ESP.restart();
  }
  else
  {
    snprintf(*p_dados, *tamanhoStr + 1, "{\"lat\": %.6f, \"lon\": %.6f}", latitude, longitude);
    Serial.println(*p_dados);
  }
}

void getGPS(float *p_lat, float *p_lon)
{
  *p_lat = gps.location.lat();
  *p_lon = gps.location.lng();
}

void espSleep(unsigned int secondsSleep)
{
  esp_sleep_enable_timer_wakeup(secondsSleep * uS_TO_S_FACTOR);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_34, 1); //1 = High, 0 = Low
  sleepWakeup = true;
  Serial.println("Dormindo...");
  esp_deep_sleep_start();
}


bool checkFall(bool checkSleep)
{
  unsigned long tempoAtual = millis();
  Serial.println("Checando...");
  Serial.println(digitalRead(34));
  if(checkSleep == true){
    if(tempoAtual - 0 > 500 && digitalRead(34) == HIGH){
      sleepWakeup = false;
      return true;
    }
    else if(tempoAtual - 0 > 1000 && digitalRead(34) == LOW)
    {
      Serial.println("Apenas tremeu");
      Serial.print("Tempo restante: ");
      Serial.println((60000000 -   esp_timer_get_time())/1000000);
      espSleep(60);
      return false;
    }
    return false;
  }
  else
    return true;
  
}