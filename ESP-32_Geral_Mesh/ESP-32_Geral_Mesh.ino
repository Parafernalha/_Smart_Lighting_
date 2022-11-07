#include "painlessMesh.h" //biblioteca "painlessmesh"
#include "DHT.h"
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <BH1750.h>
#include <EEPROM.h>
BH1750 lightMeter;


#define   MESH_PREFIX     "ESPMESH2022"  //Rede MESH
#define   MESH_PASSWORD   "51525354"  //Senha da rede
#define   MESH_PORT       5555 //Porta de comunicação da rede

#define DHT_SENSOR_PIN  27 //Sensor DHT Pino
#define DHT_SENSOR_TYPE DHT22 //Modelo do DHT

#define pinLed 32 //Led Pino
#define pinLDR 33 //Sensor LDR Pino
#define pinCAM 26 //CAM Pino
//Luminária
int Luminaria = 1;

//Variaveis
float lux = 0;
float valorAjuste = 1;
float floatValue = 0;
float output = 0;
float outputPwm = 0;
float humi = 0;
float tempC = 0;
int x = 0;
int lumens = 0;
int cam = 0;
int contCam = 10000;
String mensagemEnviada = "";
String mensagemRecebida = "";

//Gets do Firebase
int getTxRx = 0;
int getLuminaria = 0;
int getModo = 1;
int getModoLumens = 1;
int getModoCam = 0;
int getIntensidade = 0;
int getTempoTransicao = 1;
int getAjusteMin = 0;
int getAjusteMax = 255;
int getAjusteLumens = 255;
int getTempoMovimento = 5;

Scheduler userScheduler;
painlessMesh  mesh;

void sendMessage() ;

Task taskSendMessage( TASK_SECOND * 3 , TASK_FOREVER, &sendMessage );

DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

void sendMessage() {
  String msg = mensagemEnviada;
  mesh.sendBroadcast( msg );
  taskSendMessage.setInterval(TASK_SECOND * 3);
}

void receivedCallback( uint32_t from, String &msg ) {
  mensagemRecebida = msg.c_str();
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

void funcaoConcatenaMensagem() {
  mensagemEnviada = ("0.");
  mensagemEnviada += (Luminaria);
  mensagemEnviada += (".");
  mensagemEnviada += (int(tempC));
  mensagemEnviada += (".");
  mensagemEnviada += (int(humi));
  mensagemEnviada += (".");
  mensagemEnviada += (int(lux));
  mensagemEnviada += (".");
  mensagemEnviada += (cam);
  mensagemEnviada += (".");
}

void setup() {
  Serial.begin(115200);

  dht_sensor.begin();
  Wire.begin();
  lightMeter.begin();

  pinModePwm(pinLed, 0);  // pin Led
  pinMode(pinLDR, INPUT); // pin LDR
  pinMode(pinCAM, INPUT); // pin CAM

  mesh.setDebugMsgTypes( ERROR | STARTUP );

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();

  mensagemRecebida = EEPROM.read(0);
}

void loop() {

  mesh.update();

  lux = lightMeter.readLightLevel();
  lumens = map(lux, 0 , 1000, getAjusteLumens, 0);
  cam = digitalRead(pinCAM);
  humi  = dht_sensor.readHumidity();
  tempC = dht_sensor.readTemperature();

  if (cam == 1) {
    contCam = 0;
  }

  if (contCam < getTempoMovimento * 100) {
    cam = 1;
    contCam++;
  }

  funcaoConcatenaMensagem();

  int y = 0;
  int cont = 1;
  for (int x = 0; x < mensagemRecebida.length(); x++) {
    if (mensagemRecebida.substring(x, x + 1) == ".") {
      if (cont == 1) {
        getTxRx = (mensagemRecebida.substring(y, x)).toInt();
      }
      if (getTxRx == 1) {
        if (cont == 2) {
          getLuminaria = (mensagemRecebida.substring(y + 1, x)).toInt();
        }
        if (Luminaria == getLuminaria) {
          if (cont == 3) {
            getModo = (mensagemRecebida.substring(y + 1, x)).toInt();
          }
          if (cont == 4) {
            getModoLumens = (mensagemRecebida.substring(y + 1, x)).toInt();
          }
          if (cont == 5) {
            getModoCam = (mensagemRecebida.substring(y + 1, x)).toInt();
          }
          if (cont == 6) {
            getIntensidade = (mensagemRecebida.substring(y + 1, x)).toInt();
          }
          if (cont == 7) {
            getTempoTransicao = (mensagemRecebida.substring(y + 1, x)).toInt();
          }
          if (cont == 8) {
            getAjusteMin = (mensagemRecebida.substring(y + 1, x)).toInt();
          }
          if (cont == 9) {
            getAjusteMax = (mensagemRecebida.substring(y + 1, x)).toInt();
          }
          if (cont == 10) {
            getAjusteLumens = (mensagemRecebida.substring(y + 1, x)).toInt();
          }
          if (cont == 11) {
            getTempoMovimento = (mensagemRecebida.substring(y + 1, x)).toInt();
          }
        }
      }
      cont++;
      y = x;
    }
  }

  if (getModo == 0) {
    outputPwm = pwmWriteSoft(getIntensidade, outputPwm, getTempoTransicao);
  } else {
    if ((getModoLumens == 1 && outputPwm >= lumens && cam == 0) || (getModoLumens == 0 && getModoCam == 1 && cam == 0)) {
      outputPwm = outputPwm - 255.0000 / ((getTempoTransicao * 1000) / 10);
    }
    if (getModoLumens == 1 && outputPwm <= lumens || getModoCam == 1 && cam == 1) {
      outputPwm = outputPwm + 255.0000 / ((getTempoTransicao * 1000) / 10);
    }
    if (outputPwm < getAjusteMin) {
      outputPwm = getAjusteMin;
    }
    if (outputPwm > getAjusteMax) {
      outputPwm = getAjusteMax;
    }
  }

  ledcWrite(0, outputPwm);

  if (mensagemRecebida == "" || mensagemRecebida == "null") {

  } else {
    Serial.println(mensagemRecebida);
    EEPROM.write(0, mensagemRecebida);
    mensagemRecebida = "";
  }

  if (Serial.available() > 0) {
    mensagemRecebida = Serial.readString();
    Serial.println(mensagemEnviada);
  }

  Serial.print("TEMPERATURA: ");
  Serial.println(tempC);
  Serial.print("HUMIDADE: ");
  Serial.println(humi);
  Serial.print("LUZ: ");
  Serial.println(lux);
  Serial.print("CAM: ");
  Serial.println(cam);

  delay(10);
}
