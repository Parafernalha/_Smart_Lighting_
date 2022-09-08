#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "........" // Rede
#define WIFI_PASSWORD ".........." // Senha
#define API_KEY "............" // Firebase Key
#define DATABASE_URL "..........." // Firebase URL

const int ledPin = 26; 

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
float floatValue;
bool signupOK = false;

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  dht.begin();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  ledcSetup(0, 5000, 8);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(ledPin, 0);

  pinMode(34,INPUT);

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  Serial.println("-----------------------------");
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    if (Firebase.RTDB.getInt(&fbdo, "/test/number")) {
      if (fbdo.dataType() == "string") {
        Serial.print("Valor recebido: ");
        ledcWrite(0, fbdo.stringData().toInt()); 
        Serial.println(fbdo.stringData().toInt());
      }
    }
    floatValue = 0.01 + random(0, 100);
    if (Firebase.RTDB.setFloat(&fbdo, "test/float", floatValue)) {
        Serial.print("Valor enviado: ");
        Serial.println(floatValue);
    }
  }
  //Serial.println(analogRead(34));
  //Serial.println(map(analogRead(34),0,4095,0,100));
  delay(100);
}
