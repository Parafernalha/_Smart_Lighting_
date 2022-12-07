#include <Arduino.h>
#include <WiFi.h>
#include <DHT.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>                                              //Inclusão de bibliotecas

#define WIFI_SSID "Sky"                                                            // Rede
#define WIFI_PASSWORD "51525354"                                                   // Senha da rede
#define API_KEY "AIzaSyBqr0DXRi5J9T1JkWLteXZrz6uchfOCPXQ"                          // Firebase Key
#define DATABASE_URL "https://jornadasextoperiodo-default-rtdb.firebaseio.com/"    // Firebase URL
#define USER_EMAIL "kalebebm8@gmail.com"
#define USER_PASSWORD "123456"

#define DHT_SENSOR_PIN  32                                                         // DHT11 sensor pin X
#define DHT_SENSOR_TYPE DHT11

#define pinLed 27
#define pinLDR 33
#define pinCAM 26


unsigned long sendDataPrevMillis = 0;                                                                //Variáveis
bool signupOK = false;
float floatValue = 0;
float output = 0;
float outputPwm = 0;
float humi = 0;
float tempC = 0;
int x = 0;
int get1 = 0;
int lumens = 0;
int cam = 0;
int contCam = 501;
int ajustePwm = 1;

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
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;

  fbdo.setResponseSize(2048);

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Firebase.setDoubleDigits(5);

  config.timeout.serverResponse = 10 * 1000;
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
}


String FirebaseGet(String caminho) {                                               // Função Leitura do Firebase
  if (Firebase.RTDB.getString(&fbdo, caminho)) {
    return fbdo.to<const char *>();
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
  if (ajuste == 0) {
    output = input;
  }
  if (input > output) {
    output = output + (255 / ((ajuste * 1000) / 10));
  }
  if (input < output) {
    output = output - (255 / ((ajuste * 1000) / 10));
  }
  if (output <= 0) {
    output = 0;
  }

  if (output >= 255) {
    output = 255;
  }
  return output;
}


void SerialGeral(String text, String var1) {
  Serial.print(text);
  Serial.println(var1);
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

void Task1code( void * pvParameters ) {
  for (;;) {

    lumens = map(analogRead(33), 0 , 4095, 255, 0);
    cam = digitalRead(pinCAM);

    if ( isnan(dht_sensor.readTemperature()) || isnan(dht_sensor.readHumidity())) {

    } else {
      humi  = dht_sensor.readHumidity();
      tempC = dht_sensor.readTemperature();
    }

    if (cam == 1) {
      contCam = 0;
    }

    if (contCam < 500) {
      cam = 1;
      contCam++;
    }

    outputPwm = pwmWriteSoft(get1, outputPwm, ajustePwm);
    ledcWrite(0, outputPwm);

    SerialGeral("Saída do pwm para o led: ", String(outputPwm));
    SerialGeral("Entrada LDR: ", String(lumens));
    SerialGeral("Entrada temp: ", String(tempC));
    SerialGeral("Entrada humi: ", String(humi));
    SerialGeral("Entrada CAM: ", String(cam));

    delay(10);
  }
}

void Task2code( void * pvParameters ) {
  for (;;) {

    FirebaseSet("/L1/temp", String(tempC));
    FirebaseSet("/L1/humi", String(humi));
    FirebaseSet("/L1/lumens", String(lumens));
    FirebaseSet("/L1/motion", String(cam));
    get1 = FirebaseGet("/L1/number").toInt() * 2.55;
    ajustePwm = FirebaseGet("/L1/ajuste").toInt();

    delay(2000);
  }
}

void loop() {

}
