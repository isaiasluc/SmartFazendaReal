#include <ESP8266WiFi.h> //Conexão WiFi do ESP8266
#include <FirebaseArduino.h> //Contém todas as funções que utilizaremos do Firebase
#include <Ticker.h> //Biblioteca para usar os timers
#include <NTPClient.h> //Biblioteca necessária para obter data e hora
#include <WiFiUdp.h> //Utiliza em conjunto com a NTPClient.h

//Incluindo bibliotecas necessárias para o sensor laser VL53l1x
#include <ComponentObject.h>
#include <RangeSensor.h>
#include <SparkFun_VL53L1X.h>
#include <vl53l1x_class.h>
#include <vl53l1_error_codes.h>
#include <Wire.h>
#include "SparkFun_VL53L1X.h"

//Iniciando uma instância do NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "a.st1.ntp.br");

//Configuraçõs do Firebase
#define FIREBASE_HOST "smart-fazenda-real.firebaseio.com"
#define FIREBASE_AUTH "aQxXYLGyDzSobw2QDiw4xXB8p10UNPA8sTSKuQn1"
#define TABLE_NAME "Dados tanque 2 (Poco)"

//Configuraçõs do WiFi
#define WIFI_SSID "William" //Nome da Wifi
#define WIFI_PASSWORD "camaleao" //Senha da Wifi



// CRIANDO OBJETO JSON PARA ENVIAR DADOS AO FIREBASE
// -------------------------------------------
StaticJsonBuffer<200> jsonBuffer;
JsonObject &root = jsonBuffer.createObject();
// -------------------------------------------

// DEFININDO TICKER DE ATUALIZAÇÃO
// -------------------------------
Ticker ticker;
bool publishNewState = true;

void publish(){
  publishNewState = true;
}
#define PUBLISH_INTERVAL 1000*60*0.166
// -------------------------------
//Iniciando um objeto do tipo SFEVL53L1X
SFEVL53L1X distanceSensor;

// ------------------------------
// FUNÇÃO DISTANCIA

float distancia () {
  distanceSensor.startRanging(); //Write configuration bytes to initiate measurement
  float offset=20;
  //distanceSensor.setOffset(1);
  while (!distanceSensor.checkForDataReady()) {
    delay(1);
  }
  float distance = (distanceSensor.getDistance()+offset)/10; //Get the result of the measurement from the sensor
  distanceSensor.clearInterrupt();
  distanceSensor.stopRanging();
  
  
return distance;
}
// ------------------------------

const int rele = D6;

void setup() {
  //Iniciando comunicação serial
  //Iniciando comunicação serial
  Wire.begin();
  Serial.begin(115200);
  delay(1000);
  if (distanceSensor.begin() == 0) { //Begin returns 0 on a good init
    Serial.println("Sensor online!");
  }
  pinMode(rele, OUTPUT);
  digitalWrite(rele, HIGH);
  //Iniciando conexão WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando com ");
  Serial.println(WIFI_SSID);
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
    }
  Serial.println();
  Serial.print("Conectado a ");
  Serial.println(WIFI_SSID);
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.mode(WIFI_STA);

  timeClient.setTimeOffset(0); //Offset do NTP Client (-10800 para GTM -3:00hrs)
  timeClient.begin(); //Inicia o NTP Client

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); //Inicia uma instância do Firebase
  Firebase.setString("system_power", "Desligado"); //Seta o estado inicial do sistema para Desligado

  //Registrando o Ticker para publicar de tempos em tempos
  ticker.attach_ms(PUBLISH_INTERVAL, publish);                                            // inicializa o sistema desligado
}

void loop() {
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime(); //Retorna o timestamp
  String formattedTime = timeClient.getFormattedTime();

  float alturamedia_poco, tank2_vol;
  int temp_bomba;
  String tank2_level, system_power, tank1Status, status_bomba;

  //Dimensões do poco em cm
  float R=14.5, r=13, h=32;

  //LIGANDO SISTEMA
  system_power=Firebase.getString("system_power");
  
  if (system_power=="Ligado") {
    alturamedia_poco=distancia();
    tank2_vol=(((3.1415*(h-alturamedia_poco))*((R*R)+(R*r)+(r*r))/3)/1000);
    
    tank1Status = Firebase.getString("tank1_level");
    
    // Apenas publique quando passar o tempo determinado
    if  (publishNewState){
      Serial.println("Publish new State");
    // Mandando para o firebase
      if (tank1Status == "LOW") {
        digitalWrite (rele,LOW);
        status_bomba="ON";
        Serial.println("BOMBA LIGADA");
        Serial.println("Nivel do tanque 1: LOW");
      } else if (tank1Status == "FULL") {
        digitalWrite (rele,HIGH);
        status_bomba="OFF";
        Serial.println("BOMBA DESLIGADA");
        Serial.println("Nivel do tanque 1: FULL");
      }
      publishNewState = false;
      
      root["alturamedia_poco"] = alturamedia_poco;
      root["status_bomba"] = status_bomba;
      root["temp_bomba"] = temp_bomba;
      root["tank2_level"] = tank2_level;
      root["tank2_vol"] = tank2_vol;
      root["time"] = epochTime;
      root["system_power"] = system_power;

      Firebase.setString("tank2_level", tank2_level);
      Firebase.setFloat("alturamedia_poco", alturamedia_poco);
      Firebase.push(TABLE_NAME, root);

    } else {
      Serial.println("Erro ao publicar estado");
    }
    //Exibindo informações no Serial Monitor do Arduino IDE
    Serial.print("Distancia em cm: ");
    Serial.println(alturamedia_poco);
    Serial.print("Hora: ");
    Serial.println(formattedTime);
    delay(10000);
    } else if (system_power=="Desligado") {
      Serial.println("Sistema desligado");
      delay(2000);
    }
}
