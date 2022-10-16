#include <Arduino.h>
#include <WiFi.h>
#include <DHT.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"                                                     //Inclusão de bibliotecas

#define WIFI_SSID "Sky"                                                            // Rede
#define WIFI_PASSWORD "51525354"                                                   // Senha da rede
#define API_KEY "AIzaSyBqr0DXRi5J9T1JkWLteXZrz6uchfOCPXQ"                          // Firebase Key
#define DATABASE_URL "https://jornadasextoperiodo-default-rtdb.firebaseio.com/"    // Firebase URL

#define DHT_SENSOR_PIN  14                                                         // DHT11 sensor pin X
#define DHT_SENSOR_TYPE DHT11

#define pinLed 27
#define pinLDR 12
#define pinCAM 26

                                            
float floatValue;                                                                 //Variáveis
bool signupOK = false;
int output = 0;
float outputPwm = 0;
int outputPwm2 = 0;
int x = 0;
int get1 = 0;
int lumens;
float humi;
float tempC;

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;                                                              //Configurações do Firebase

TaskHandle_t Task1;
TaskHandle_t Task2;

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
  pinModePwm(pinLed, 0);  // pin Led
  pinMode(pinLDR, INPUT); // pin LDR
  pinMode(pinCAM, INPUT); // pin CAM

  xTaskCreatePinnedToCore(
    Task1code,   /* Task function. */
    "Task1",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &Task1,      /* Task handle to keep track of created task */
    0);          /* pin task to core 0 */
  delay(500);

  xTaskCreatePinnedToCore(
    Task2code,   /* Task function. */
    "Task2",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &Task2,      /* Task handle to keep track of created task */
    1);          /* pin task to core 1 */
  delay(500);
}


void Task2code( void * pvParameters ) {
  for (;;) {
    
    FirebaseSet("/L1/Out1", String(outputPwm));
    FirebaseSet("/L1/temp", String(tempC));
    FirebaseSet("/L1/humi", String(humi));
    FirebaseSet("/L1/lumens", String(lumens));
    get1 = FirebaseGet("/L1/number").toInt();
    
    delay(2000);
    
  }
}


void Task1code( void * pvParameters ) {
  for (;;) {

    lumens = map(analogRead(12), 0 , 4095, 255, 0);
    humi  = dht_sensor.readHumidity();
    tempC = dht_sensor.readTemperature();

    outputPwm = pwmWriteSoft(get1, outputPwm, 2);
    ledcWrite(0, outputPwm);

    SerialGeral("Saída do pwm para o led: ", outputPwm);
    SerialGeral("Saída de temp: ", tempC);
    SerialGeral("Entrada CAM: ", digitalRead(12));
    delay(10);
  }
}


void loop() {

}
