#include <ESP8266WiFi.h> //Conexão WiFi do ESP8266
#include <FirebaseArduino.h> //Contém todas as funções que utilizaremos do Firebase
#include <NTPClient.h> //Biblioteca necessária para obter data e hora
#include <WiFiUdp.h> //Utiliza em conjunto com a NTPClient.h
#include <ArduinoJson.h>

//Incluindo bibliotecas necessárias para o sensor laser VL53l1x
#include <Wire.h>
#include <VL53L1X.h>

//Iniciando uma instância do NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "a.st1.ntp.br", -3 * 3600);

//Configuraçõs do Firebase
#define FIREBASE_HOST "smart-fazenda-real.firebaseio.com"
#define FIREBASE_AUTH "aQxXYLGyDzSobw2QDiw4xXB8p10UNPA8sTSKuQn1"
#define TABLE_NAME "Dados tanque 2 (Poço)"

//Configuraçõs do WiFi
#define WIFI_SSID "William" //Nome da Wifi
#define WIFI_PASSWORD "camaleao" //Senha da Wifi

//----------------------------------------------------------------------------------------
// CRIANDO OBJETO JSON PARA ENVIAR DADOS AO FIREBASE

StaticJsonBuffer<200> jsonBuffer;
JsonObject &root = jsonBuffer.createObject();
//----------------------------------------------------------------------------------------

//Iniciando um objeto VL53L1X chamado sensor
VL53L1X sensor;

//Definindo o pino D6 para o Rele
const int rele = D6;

// FILTRO MÉDIA MÓVEL

#define n 5 //Número de pontos
int real, filtrado;
int numbers[n];

long moving_average() {
  for (int i=n-1;i>0;i--) numbers[i]=numbers[i-1];

  numbers[0] = real;

  int acc = 0;

  for (int i=0;i<n;i++) acc += numbers[i];

  return (acc/n);
}

//----------------------------------------------------------------------------------------

// FUNÇÕES PARA INICIAR O SENSOR E COMUNICAÇÃO SERIAL

void espInit() {
  //Iniciando comunicação serial
  Wire.begin();
  Serial.begin(115200);
  Wire.setClock(400000); // 400 kHz I2C communication
  pinMode(rele, OUTPUT);
  digitalWrite(rele, HIGH);
  delay(1000);
  /*
  //Iniciando sensor
  sensor.setTimeout(1000);
  if (!sensor.init()) {
    Serial.println("Falha ao detectar e inicializar o sensor!");
    while (1);
  }
  sensor.setDistanceMode(VL53L1X::Long);
  sensor.setMeasurementTimingBudget(50000);
  sensor.startContinuous(1000);
  */
  //Iniciando instância do NTP timeClient
  //timeClient.setTimeOffset(0); //Offset do NTP Client (-10800 para GTM -3:00hrs)
  timeClient.begin(); //Inicia o NTP Client
}

void wifiInit() {
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
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); //Inicia uma instância do Firebase
  Firebase.setString("system_power", "Desligado"); //Seta o estado inicial do sistema para Desligado
  Serial.println("Iniciada instância do Firebase");
  Serial.println("Setando estado inicial do sistema para Desligado");
}
//----------------------------------------------------------------------------------------

//FUNÇÃO PARA ATUALIZAR DATA E HORA

int timeUpdate() {
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime(); //Retorna o timestamp
  //String formattedTime = timeClient.getFormattedTime(); //Retorna hora formatada
  
  return epochTime;
}
//----------------------------------------------------------------------------------------

//FUNÇÃO QUE CALCULA OS DADOS E ENVIA PARA O FIREBASE

void enviaDados() {
  float pocoVol;
  int pocoAlturaAgua, timestamp, pumpTemp, pumpStatus, caixaLevel, pocoLevel;
  String system_power;
  
  //real = sensor.read();
  //filtrado = moving_average();
  timestamp = timeUpdate();

  //Dimensões do reservatório em mm
  int R=145, r=130, h=337;

  //LIGANDO O SISTEMA
  system_power=Firebase.getString("system_power");

  if (system_power == "Desligado") {
    Serial.println("Sistema desligado");
  } else if (system_power == "Ligado") {
    pocoAlturaAgua=h-filtrado; //Altura da coluna de água
    pocoVol=(((3.1415*(pocoAlturaAgua))*((R*R)+(R*r)+(r*r))/3)/1000); //Volume de água do poço

    caixaLevel = Firebase.getInt("caixaLevel");
    
  //Mandando os dados coletados para o Firebase
      if (caixaLevel == 0) { //caixaLevel Low
        digitalWrite (rele,LOW);
        pumpStatus = 1; //Bomba ligada
      } else if (caixaLevel == 2) { //caixaLevel High
        digitalWrite (rele,HIGH);
        pumpStatus = 0; //Bomba desligada
      }

      if (pocoAlturaAgua >= 110) {
        pocoLevel = 2; //"HIGH";
      } else if (pocoAlturaAgua >= 50 && pocoAlturaAgua < 110) {
        pocoLevel = 1; //"OK";
      } else if (pocoAlturaAgua < 50) {
        pocoLevel = 0; //"LOW";
      }
      
      root["pocoAlturaAgua"] = pocoAlturaAgua;
      root["pocoLevel"] = pocoLevel;
      root["pocoVol"] = pocoVol;
      root["pumpStatus"] = pumpStatus;
      //root["pumpTemp"] = pumpTemp;
      root["timestamp"] = timestamp;
      root["system_power"] = system_power;

      Firebase.setInt("pocoLevel", pocoLevel);
      Firebase.setFloat("pocoAlturaAgua", pocoAlturaAgua);
      Firebase.setInt("pumpStatus", pumpStatus);
      
      Firebase.push(TABLE_NAME, root);

      if (Firebase.failed()){
        Serial.println("Setting Number Failed...");
        Serial.println(Firebase.error());
      }

      Serial.println("Dados enviados com sucesso!");
      }
}

void setup() {
  espInit();
  wifiInit();
}

void loop() {
  enviaDados();
  delay(5000);
}
