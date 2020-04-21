#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>                                              // firebase library
#include <Ticker.h>


//Firebase settings
#define FIREBASE_HOST "smart-fazenda-real.firebaseio.com"
#define FIREBASE_AUTH "aQxXYLGyDzSobw2QDiw4xXB8p10UNPA8sTSKuQn1"

//Wi-Fi settings
#define WIFI_SSID "William_2.4GHZ"                                          // home or public wifi name 
#define WIFI_PASSWORD "camaleao"                                          // wifi password

//Define os pinos para o trigger e echo
#define trigPin D1
#define echoPin D2

#define TABLE_NAME "Dados tanque 1 (Caixa d'agua)"

// CRIANDO OBJETO JSON PARA ENVIAR DADOS AO FIREBASE
// -------------------------------------------
StaticJsonBuffer<200> jsonBuffer;
JsonObject &root = jsonBuffer.createObject();
// -------------------------------------------

#define PUBLISH_INTERVAL 1000*60*0.166

// DEFININDO TICKER DE ATUALIZAÇÃO
// -------------------------------
Ticker ticker;
bool publishNewState = true;

void publish(){
  publishNewState = true;
}
// -------------------------------

// DECLARANDO FUNÇÕES A SEREM UTILIZADAS
// -------------------------------
// FUNÇÃO CONCAT

int concat(int a, int b) { 
    char s1[20];
    char s2[20];
    // Convert both the integers to string
    sprintf(s1, "%d", a);
    sprintf(s2, "%d", b);
    // Concatenate both strings 
    strcat(s1, s2);  
    // Convert the concatenated string 
    // to integer 
    int c = atoi(s1); 
    // return the formed integer 
    return c; 
}

// ------------------------------
// FUNÇÃO DISTANCIA

float distancia () {
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
distance= duration*0.034/2;

return distance;
}

int dataAtual () {
////GRAVA HORA ATUAL
StaticJsonBuffer<50> Buffer;
JsonObject& timeStampObject = Buffer.createObject();
timeStampObject[".sv"] = "timestamp" ;
Firebase.set("time", timeStampObject);

////PEGA A HORA ATUAL GRAVADA
StaticJsonBuffer<50> Buffer1;
FirebaseObject object = Firebase.get("time"); // Retrive th JSON from firebase
JsonObject& timeStampObject1= Buffer1.createObject();// create a new Json object to store the json value
timeStampObject1["time"]= object.getJsonVariant(); // object.getJsonVariant("config");
String agora3 = timeStampObject1["time"];// store it into a variable
int agora4 = agora3.substring(0,10).toInt();// store it into a variable

  //CONVERTENDO TIMESTAMP PARA DATA
  agora4=agora4-3600*3  ; // (GMT -3)
    
  int ano=1970+(agora4/31556926);
  int mes=1+(agora4 % 31556926) / 2629743;
  int dia=1+(agora4 % 2629743) / 86400;
  int mesEano=concat(mes,ano);
  int data=concat(dia,mesEano);
  return data;
}

int horaAtual (){

////GRAVA HORA ATUAL
StaticJsonBuffer<50> Buffer;
JsonObject& timeStampObject = Buffer.createObject();
timeStampObject[".sv"] = "timestamp" ;
Firebase.set("time", timeStampObject);

////PEGA A HORA ATUAL GRAVADA
StaticJsonBuffer<50> Buffer1;
FirebaseObject object = Firebase.get("time"); // Retrive th JSON from firebase
JsonObject& timeStampObject1= Buffer1.createObject();// create a new Json object to store the json value
timeStampObject1["time"]= object.getJsonVariant(); // object.getJsonVariant("config");
String agora3 = timeStampObject1["time"];// store it into a variable
int agora4 = agora3.substring(0,10).toInt();// store it into a variable

////PASSA TIMESTAMP PARA HORA
agora4=agora4-3600*3  ; // (GMT -3) 

int hr=(agora4  % 86400L) / 3600;
int minuto=(agora4 % 3600) / 60;
int agora = (hr*100)+ minuto; //
return agora;
}

void setup() {
  // Iniciando a comunicação serial
  Serial.begin(115200);
  delay(1000);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  // Iniciando a conexão WIFI
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                      
  Serial.print("Conectando com ");
  Serial.print(WIFI_SSID);
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
 
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);                                       // conexão com o firebase
  
  Firebase.setString("Status do Sistema", "0");                                            // inicializa o sistema desligado
     
  // Registra o ticker para publicar de tempos em tempos
  ticker.attach_ms(PUBLISH_INTERVAL, publish);
}

void loop() {
  float alt1, alt2, alt3, alt4, alt5, altura, tank1_volume, R=14.5, r=13;
  String tank_level, system_power;


  
  //LIGANDO SISTEMA
  //system_power=Firebase.getString("Status do Sistema").toInt();
  //system_power=Firebase.getInt("Status do Sistema");
    system_power=Firebase.getString("Status do Sistema");
  
  if (system_power=="1") {
    alt1=distancia();
    delay(1000);
    alt2=distancia();
    delay(1000);
    alt3=distancia();
    delay(1000);
    alt4=distancia();
    delay(1000);
    alt5=distancia();
    delay(1000);
    altura=(alt1+alt2+alt3+alt4+alt5)/5;
    tank1_volume=(((3.1415*(32-altura))*((R*R)+(R*r)+(r*r)))/3)/1000;
    // Apenas publique quando passar o tempo determinado
    if(publishNewState){
    Serial.println("Publish new State");
    // Mandando para o firebase
      if (altura > 25) {
        Serial.println("Nivel do tanque 1: LOW");
        tank_level = "LOW";
        Firebase.setString("Nivel do tanque 1", "LOW");
        Firebase.setFloat("Distancia em cm do tanque 1", altura);
        Firebase.setFloat("Volume do tanque 1", tank1_volume);
        //status_bomba = "ON";
      } else if (altura < 25) {
        Serial.println("Nivel do tanque 1: FULL");
        tank_level = "FULL";
        Firebase.setString("Nivel do tanque 1", "FULL");
        Firebase.setFloat("Distancia em cm do tanque 1", altura);
        Firebase.setFloat("Volume do tanque 1", tank1_volume);
        //status_bomba = "OFF";
      }
    publishNewState = false;
    root["1- Distancia em cm"] = altura;
    root["2- Nivel do tanque"] = tank_level;
    root["3- Volume do tanque"] = tank1_volume;
    root["4- Data"] = dataAtual();
    root["5- Hora"] = horaAtual();
    Firebase.push(TABLE_NAME, root);
    } else {
      Serial.println("Error Publishing");
    }

  //Exibe informacoes no serial monitor
  // Prints the distance on the Serial Monitor
  Serial.print("Distancia em cm: ");
  Serial.print(altura);
  Serial.println("cm");
  Serial.println("Data:");
  Serial.println(dataAtual());
  Serial.println("Hora:");
  Serial.println(horaAtual());
  delay(10000); // realiza nova leitura a cada 10 segundos
  
  } else if (system_power=="0") {
    Serial.println("Sistema desligado");
  }
}
