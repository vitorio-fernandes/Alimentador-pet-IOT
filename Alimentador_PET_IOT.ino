#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#endif
// Essa bibioteca é externa, para o SERVO MOTOR
#include <ESP32_Servo.h>
// Váriaveis e definições
#define TRIG_PIN 26 // Primeiro Sensor         MOTOR 
#define ECHO_PIN 27 // Primeiro Sensor         MOTOR
/////////////////////////////////////
#define TRIG_PINO 25 // Segundo Sensor         LED'S
#define ECHO_PINO 33 // Segundo sensor         LED'S
// Servo Motor
#define SERVO 12 // Pino do Servo motor
Servo s; // Variável Servo
int pos; // Posição Servo

// Potênciometro
int potencia = 34;
int leituraPotencia = 0;
int tempoEspera = 0;

// Led´s
int azul = 14; // B         LETRA QUE VAI OS FIOS
int vermelho = 32;     // R
int verde = 15;    // G

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

// Parâmetros do WIFI, SSID e SENHA
#define WIFI_SSID "teste"
#define WIFI_PASSWORD "teste12345"

// Configuração com os dados do servidor URL e SENHA
#define DATABASE_URL "https://projeto-ac-25ca0-default-rtdb.firebaseio.com/" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app
#define DATABASE_SECRET "7yVNOgYY0j1gVoQskp9IKzLW85QvQ7NWkBEIuIg3"

/* 3. Define the Firebase Data object */
FirebaseData fbdo;

/* 4, Define the FirebaseAuth data for authentication data */
FirebaseAuth auth;

/* Define the FirebaseConfig data for config data */
FirebaseConfig config;

// Váriaveis de controle
unsigned long dataMillis = 0;
int count = 0;

void setup()
{

    Serial.begin(115200);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
    
    config.database_url = DATABASE_URL;
    config.signer.tokens.legacy_token = DATABASE_SECRET;

    Firebase.reconnectWiFi(true);

    /* Initialize the library with the Firebase authen and config */
    Firebase.begin(&config, &auth);

// Configuração dos sensores
// Primeiro Sensor Ultrassônico;
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    
    // Segundo Sensor Ultrassônico;
    pinMode(TRIG_PINO, OUTPUT);
    pinMode(ECHO_PINO, INPUT);
    
    // Servo motor
    s.attach(SERVO);
    s.write(0);
    
    // Led´s
    pinMode(vermelho, OUTPUT);
    pinMode(azul, OUTPUT);
    pinMode(verde, OUTPUT);
}

void loop()
{ // Configuração do potenciometro com DELAY de 10 Segundos;
  leituraPotencia = analogRead(potencia);
  tempoEspera = map(leituraPotencia, 0, 1023,0,1000); 
  Serial.println(tempoEspera);
  Serial.println(" DELAY MOTOR EM MILISEGUNDOS :");
  
   // Primeiro Sensor Ultrassônico;
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(TRIG_PIN, LOW);
  
  long duracao = pulseIn(ECHO_PIN, HIGH);
  int distancia_cm = duracao * 0.034 / 2;
  
  Serial.print("Distancia: ");
  Serial.print(distancia_cm);
  Serial.println(" cm");
  
  delay(100);

  // Segundo Sensor Ultrassônico
  digitalWrite(TRIG_PINO, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PINO, HIGH);
  delayMicroseconds(5);
  digitalWrite(TRIG_PINO, LOW);
  
  long duracao2 = pulseIn(ECHO_PINO, HIGH);
  int teste = duracao2 * 0.034 / 2;
  int distancia_cm2 = teste;
  
  Serial.print("Distancia: ");
  Serial.print(distancia_cm2);
  Serial.println(" cm");
  
  delay(100);


// Configuração dos LED´S de acordo com o sensor 2 e colocando no Servidor FireBase
  if (distancia_cm2 < 10) // LED VERDE e servidor FireBase
  { Serial.printf("Set string... %s\n", Firebase.setString(fbdo, F("test/Quantidade Comida"), "QUANTIDA DE COMIDA, OK !") ? "ok" : fbdo.errorReason().c_str());
  	digitalWrite(verde, HIGH);
    digitalWrite(azul, LOW);
    digitalWrite(vermelho, LOW);
    
  }
  
    if (distancia_cm2 > 10 && distancia_cm2 <20) //  LED AMARELO e servidor FireBase
  {   Serial.printf("Set string... %s\n", Firebase.setString(fbdo, F("test/Quantidade Comida"), "QUANTIDADE DE COMIDA, MEDIANA !") ? "ok" : fbdo.errorReason().c_str());
  	  digitalWrite(verde, HIGH);
      digitalWrite(azul, LOW);
      digitalWrite(vermelho, HIGH);
  }
 
    if (distancia_cm2 > 20) // LED VERMELHO e servidor FireBase
  {   Serial.printf("Set string... %s\n", Firebase.setString(fbdo, F("test/Quantidade Comida"), "ATENÇÃO ! COLOQUE COMIDA !") ? "ok" : fbdo.errorReason().c_str());
  	  digitalWrite(verde, LOW);
      digitalWrite(azul, LOW);
      digitalWrite(vermelho, HIGH);
      
  }
  // Servo motor funcionar usando o primeiro Sensor e colocando no servidor FireBase
  if (distancia_cm < 30)
  { Serial.printf("Set string... %s\n", Firebase.setString(fbdo, F("test/Animal"), "ANIMAL COMENDO !") ? "ok" : fbdo.errorReason().c_str());
    s.write(450); // Angulo de abertura da comida
    delay(tempoEspera); //Delay pelo potenciometro;
    s.write(0);
    delay(1000);
  }
  else
  {
    Serial.printf("Set string... %s\n", Firebase.setString(fbdo, F("test/Animal"), "ANIMAL NÃO ESTÁ AQUI !") ? "ok" : fbdo.errorReason().c_str());
  }

// Ligar o motor atráves da IoT, usando o FireBase;   Ligado = 1 e Desligado = 0 
    if (millis() - dataMillis > 100)
    {  dataMillis = millis();
      int iVal = 0;
      Serial.printf("Get int ref... %s\n", Firebase.getInt(fbdo, F("/test/motor"), &iVal) ? String(iVal).c_str() : fbdo.errorReason().c_str());
      if(iVal == 0)
      {
        s.write(0);
        delay(100);
      }
      if(iVal==1)
      { s.write(450);
        delay(700);
        s.write(0);
        delay(10000);
      }
    
    }
}