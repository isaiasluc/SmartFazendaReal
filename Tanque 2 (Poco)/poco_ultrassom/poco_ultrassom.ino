#include <ESP8266WiFi.h> //Conexão WiFi do ESP8266
#include <FirebaseArduino.h> //Contém todas as funções que utilizaremos do Firebase
#include <NTPClient.h> //Biblioteca necessária para obter data e hora
#include <WiFiUdp.h> //Utiliza em conjunto com a NTPClient.h

//Iniciando uma instância do NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "a.st1.ntp.br");

//Configuraçõs do Firebase
#define FIREBASE_HOST "smart-fazenda-real.firebaseio.com"
#define FIREBASE_AUTH "aQxXYLGyDzSobw2QDiw4xXB8p10UNPA8sTSKuQn1"
#define TABLE_NAME "Dados tanque 2 (Poco)"

//Configuraçõs do WiFi
#define WIFI_SSID "ESTABULO COZINHA" //Nome da Wifi
#define WIFI_PASSWORD "A1b2c3d4e5" //Senha da Wifi

//Definindo pinos para trigger e echo do sensor HCSR04 (Ultrassom)
#define trigPin D4
#define echoPin D5

//Definindo pinos dos leds
#define ledGreen D2
#define ledRed D3

// CRIANDO OBJETO JSON PARA ENVIAR DADOS AO FIREBASE
// -------------------------------------------
StaticJsonBuffer<200> jsonBuffer;
JsonObject &root = jsonBuffer.createObject();
// -------------------------------------------


// ------------------------------
// FUNÇÃO DISTANCIA E FUNÇÃO QUE FILTRA A DISTÂNCIA

#define n 5 //Número de pontos

float real, filtrado;
float numbers[n];

float moving_average() {
  for (int i=n-1;i>0;i--) numbers[i]=numbers[i-1];

  numbers[0] = real;

  float acc = 0;

  for (int i=0;i<n;i++) acc += numbers[i];

  return (acc/n);
}

float distancia () { //*10 para retornar o valor em mm
  float distance;
  long duration;
  // Clears the trigPin
digitalWrite(trigPin, LOW);
delayMicroseconds(2);

// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);

// Calculating the distance
distance= (duration*0.034)/2;

return (distance);
}

// ------------------------------

const int rele = D6;

// ------------------------------
void espInit() {
  //Iniciando comunicação serial
  Serial.begin(115200);
  delay(1000);
  pinMode(trigPin, OUTPUT); //Seta o trigPin como Output (saída)
  pinMode(echoPin, INPUT); //Seta o echoPin como Input (entrada)
  pinMode(rele, OUTPUT);
  digitalWrite(rele, HIGH);
  pinMode(ledGreen, OUTPUT); //Led verde para sinalizar ON
  pinMode(ledRed, OUTPUT); //Led vermelho para sinalizar OFF
  
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

void enviaDados() {
  float pocoVol, pocoAlturaAgua;
  int timestamp, pocoLevel, temp_bomba, pumpStatus, caixaStatus;
  String system_power;

  real = distancia();
  filtrado = moving_average();

  //Dimensões do poco em cm
  float R=14, r=13, h=145;

  //LIGANDO SISTEMA
  system_power=Firebase.getString("system_power");

  if (system_power == "Desligado") {
    Serial.println("Sistema desligado");
    digitalWrite (rele,HIGH);
    digitalWrite(ledRed, HIGH); //Led vermelho sinalizando sistema OFF
    digitalWrite(ledGreen, LOW);
    
  } else if (system_power == "Ligado") {
    digitalWrite(ledRed, LOW);
    digitalWrite(ledGreen, HIGH);

    timestamp = timeUpdate();
    pocoAlturaAgua = h-filtrado;
    pocoVol = (((3.1415*(pocoAlturaAgua))*((R*R)+(R*r)+(r*r))/3)/1000);


      if (pocoAlturaAgua >= 110) {
        pocoLevel = 2; //High
      } else if (pocoAlturaAgua >= 70 && pocoAlturaAgua < 110) {
        pocoLevel = 1; //Ok
      } else if (pocoAlturaAgua < 70) {
        pocoLevel = 0; //Low
      }

    //Pegando o status da caixa d'agua

    caixaStatus = Firebase.getInt("caixaLevel");

    // Mandando para o firebase
      if (caixaStatus == 0 && pocoLevel != 0) { //  || caixaStatus == ----- So liga a bomba quando o nível de água está abaixo de 10cm
        pumpStatus=1; //LIGA A BOMBA COM 1
        digitalWrite (rele,LOW); //LIGA A BOMBA CASO O NIVEL DA CAIXA ESTEJA BAIXO
        Serial.println("BOMBA LIGADA");
      } else if (caixaStatus == 2) {
        pumpStatus=0; //DESLIGA A BOMBA COM 0
        digitalWrite (rele,HIGH); //DESLIGA A BOMBA CASO O NIVEL DA CAIXA ESTEJA ALTO
        Serial.println("BOMBA DESLIGADA");
      }


    root["pocoAlturaAgua"] = pocoAlturaAgua;
    root["pumpStatus"] = pumpStatus;
    root["temp_bomba"] = temp_bomba;
    root["pocoLevel"] = pocoLevel;
    root["pocoVol"] = pocoVol;
    root["timestamp"] = timestamp;
    root["system_power"] = system_power;

    Firebase.setInt("pocoLevel", pocoLevel);
    Firebase.setFloat("pocoAlturaAgua", pocoAlturaAgua);

    Firebase.push(TABLE_NAME, root);

    if (Firebase.failed()){
        Serial.println("Setting Number Failed...");
        Serial.println(Firebase.error());
    }

    Serial.println("Dados enviados com sucesso!");
    Serial.print("pocoAlturaAgua = ");
    Serial.println(pocoAlturaAgua);
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
