#include <Arduino.h>
#include <WiFi.h>
#include <DHT.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"                                                     //Inclusão de bibliotecas

#define WIFI_SSID "Amilton"                                                        // Rede
#define WIFI_PASSWORD "contra619"                                                  // Senha da rede
#define API_KEY "AIzaSyBqr0DXRi5J9T1JkWLteXZrz6uchfOCPXQ"                          // Firebase Key
#define DATABASE_URL "https://jornadasextoperiodo-default-rtdb.firebaseio.com/"    // Firebase URL

#define DHT_SENSOR_PIN  14                                                         // DHT11 sensor pin X
#define DHT_SENSOR_TYPE DHT11

unsigned long sendDataPrevMillis = 0;                                              //Variáveis
unsigned long sendDataPrevMillis2 = 0;
float floatValue;
bool signupOK = false;
int output = 0;
float outputPwm = 0;
int outputPwm2 = 0;
int x = 0;
int get1 = 0;


FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;                                                              //Configurações do Firebase

DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);                                    //Configurações DHT11


void connectWifi() {                                                                // Função de conecção do Wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println(WiFi.localIP());
}


void connectFirebase() {                                                            // Função de conecção do Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}


String FirebaseGet(String caminho) {                                               // Função Leitura do Firebase
  if (Firebase.RTDB.getInt(&fbdo, caminho)) {
    if (fbdo.dataType() == "string") {
      return fbdo.stringData();
    }
  }
}


void FirebaseSet(String caminho, String Value) {                                  // Função Escrita do Firebase
  if (Firebase.RTDB.setString(&fbdo, caminho, Value)) {}
}

void pinModePwm(int Pin, int setPin) {
  ledcSetup(setPin, 5000, 8);
  ledcAttachPin(Pin, setPin);
}

float pwmWriteSoft(float input, float output, float ajuste) {
  if (input > output) {
    output = output + (1.00 / ajuste);
  }

  if (input < output) {
    output = output - (1.00 / ajuste);
  }
  return output;
}

void SerialGeral(String text, float var) {
  Serial.print(text);
  Serial.println(var);
}

void setup() {
  Serial.begin(115200);
  dht_sensor.begin();
  connectWifi();
  connectFirebase();
  pinModePwm(26, 0);  // pin Led
  pinMode(33, INPUT); // pin Cam
  pinMode(34, INPUT); // pin LDR
  pinMode(35, INPUT); // pin CAM
}


void loop() {

  int lumens = map(analogRead(34), 0 , 4095, 255, 0);
  float humi  = dht_sensor.readHumidity();
  float tempC = dht_sensor.readTemperature();

  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 2000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    FirebaseSet("/L1/temp", " ");
    get1 = FirebaseGet("/L1/number").toInt()*2,5;
  }

  outputPwm = pwmWriteSoft(get1, outputPwm, 2);
  ledcWrite(0, outputPwm);

  SerialGeral("Saída do pwm para o led: ", outputPwm);
  SerialGeral("Entrada CAM: ", digitalRead(35));

  delay(10);
}
