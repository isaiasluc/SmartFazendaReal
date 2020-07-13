#include <ESP8266WiFi.h> //Conexão WiFi do ESP8266
#include <FirebaseArduino.h> //Contém todas as funções que utilizaremos do Firebase
#include <Ticker.h> //Biblioteca para usar os timers
#include <NTPClient.h> //Biblioteca necessária para obter data e hora
#include <WiFiUdp.h> //Utiliza em conjunto com a NTPClient.h

//Incluindo bibliotecas necessárias para o sensor laser VL53l1x
#include <Wire.h>
#include <VL53L1X.h>

//Iniciando uma instância do NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "a.st1.ntp.br");

//Iniciando objeto ticker
Ticker timer;

//Configuraçõs do Firebase
#define FIREBASE_HOST "smart-fazenda-real.firebaseio.com"
#define FIREBASE_AUTH "aQxXYLGyDzSobw2QDiw4xXB8p10UNPA8sTSKuQn1"
#define TABLE_NAME "Dados tanque 1 (Caixa d'agua)"

//Configuraçõs do WiFi
#define WIFI_SSID "William_2.4GHZ" //Nome da Wifi
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

//Iniciando um objeto VL53L1X chamado sensor
VL53L1X sensor;

// ------------------------------
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

// ------------------------------

void setup() {
  //Iniciando comunicação serial
  Wire.begin();
  Serial.begin(115200);
  Wire.setClock(400000); // 400 kHz I2C communication
  delay(1000);
  
  sensor.setTimeout(500);
  if (!sensor.init()) {
    Serial.println("Falha ao detectar e inicializar o sensor!");
    while (1);
  }
  sensor.setDistanceMode(VL53L1X::Long);
  sensor.setMeasurementTimingBudget(50000);
  sensor.startContinuous(500);
  
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
  ticker.attach_ms(PUBLISH_INTERVAL, publish);
}

void loop() {
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime(); //Retorna o timestamp
  String formattedTime = timeClient.getFormattedTime();
  
  real = sensor.read();
  filtrado = moving_average();
  
  float tank1_vol;
  int alturamedia_caixa, alturaagua;
  String tank1_level, system_power;
  
  //Dimensões do reservatório em mm
  int R=145, r=130, h=337;

  //LIGANDO O SISTEMA
  system_power=Firebase.getString("system_power");

  if (system_power=="Ligado") {
    alturamedia_caixa=filtrado;
    alturaagua=h-alturamedia_caixa;
    tank1_vol=(((3.1415*(alturaagua))*((R*R)+(R*r)+(r*r))/3)/1000);
    
      //Mandando os dados coletados para o Firebase
      if (alturamedia_caixa > 250) {
        Serial.println("Nivel do tanque 1: LOW");
        tank1_level = "LOW";
      } else if (alturamedia_caixa > 100 && alturamedia_caixa < 250) {
        Serial.println("Nivel do tanque 1: OK");
        tank1_level = "OK";
      } else if (alturamedia_caixa < 100) {
        Serial.println("Nivel do tanque 1: FULL");
        tank1_level = "FULL";
      }

      root["alturamedia_caixa"] = alturamedia_caixa;
      root["tank1_level"] = tank1_level;
      root["tank1_vol"] = tank1_vol;
      root["time"] = epochTime;
      root["system_power"] = system_power;

      Firebase.setString("tank1_level", tank1_level);
      Firebase.setFloat("alturamedia_caixa", alturamedia_caixa);
      Firebase.push(TABLE_NAME, root);
       
      
    //Exibindo informações no Serial Monitor do Arduino IDE
    Serial.print("Altura em mm: ");
    Serial.println(alturaagua);
    delay(5000);
  } else if (system_power=="Desligado") {
    Serial.println("Sistema desligado");
    delay(2000);
  }
}
