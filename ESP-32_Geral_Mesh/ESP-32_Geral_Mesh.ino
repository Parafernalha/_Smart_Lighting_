#include <DHT.h>
#include "painlessMesh.h"

#define   MESH_PREFIX     "ESPMESH2022"
#define   MESH_PASSWORD   "51525354"
#define   MESH_PORT       5555

#define DHT_SENSOR_PIN  32 // DHT11 sensor pin X
#define DHT_SENSOR_TYPE DHT11

#define pinLed 27
#define pinLDR 33
#define pinCAM 26

int Luminaria = 1;

float floatValue = 0;
float output = 0;
float outputPwm = 0;
float humi = 0;
float tempC = 0;
int x = 0;
int lumens = 0;
int cam = 0;
int contCam = 10000;
int tempoCam = 5;
String mensagemEnviada = "";
String mensagemRecebida = "";

//Gets do Firebase
int getTxRx = 0;
int getLuminaria = 0;
int getModo = 0;
int getModoLumens = 0;
int getModoCam = 0;
int getIntensidade = 0;
int getTempoTransicao = 0;
int getAjusteMin = 0;
int getAjusteMax = 0;
int getAjusteLumens = 0;
int getTempoMovimento = 0;

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
  mensagemEnviada += ("1.");
  mensagemEnviada += (int(tempC));
  mensagemEnviada += (".");
  mensagemEnviada += (int(humi));
  mensagemEnviada += (".");
  mensagemEnviada += (lumens);
  mensagemEnviada += (".");
  mensagemEnviada += (cam);
  mensagemEnviada += (".");
}

void setup() {
  Serial.begin(115200);
  dht_sensor.begin();

  pinModePwm(pinLed, 0);  // pin Led
  pinMode(pinLDR, INPUT); // pin LDR
  pinMode(pinCAM, INPUT); // pin CAM

  mesh.setDebugMsgTypes( ERROR | STARTUP );

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
}

void loop() {

  mesh.update();

  lumens = map(analogRead(33), 0 , 4095, getAjusteLumens, 0);
  cam = digitalRead(pinCAM);
  humi  = dht_sensor.readHumidity();
  tempC = dht_sensor.readTemperature();

  if (cam == 1) {
    contCam = 0;
  }

  if (contCam < tempoCam * 100) {
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
      outputPwm = outputPwm - (getAjusteMax - getAjusteMin) / ((getTempoTransicao * 1000) / 10);
    }
    if (getModoLumens == 1 && outputPwm <= lumens) {
      outputPwm = outputPwm + (getAjusteMax - getAjusteMin) / ((getTempoTransicao * 1000) / 10);
    }
    if (getModoCam == 1 && cam == 1) {
      outputPwm = outputPwm + (getAjusteMax - getAjusteMin) / ((getTempoTransicao * 1000) / 10);
    }
  }

  if (outputPwm < getAjusteMin) {
    outputPwm = getAjusteMin;
  }
  if (outputPwm > getAjusteMax) {
    outputPwm = getAjusteMax;
  }

  ledcWrite(0, outputPwm);

  if (mensagemRecebida == "" || mensagemRecebida == "null") {

  } else {
    Serial.println(mensagemRecebida);
    mensagemRecebida = "";
  }
  
  /*
    Serial.printf("getTxRx = %.1d\n", getTxRx);
    Serial.printf("getLuminaria = %.1d\n", getLuminaria);
    Serial.printf("getModo = %.1d\n", getModo);
    Serial.printf("getModoLumens = %.1d\n", getModoLumens);
    Serial.printf("getModoCam = %.1d\n", getModoCam);
    Serial.printf("getIntensidade = %.1d\n", getIntensidade);
    Serial.printf("getTempoTransicao = %.1d\n", getTempoTransicao);
    Serial.printf("getAjusteMin = %.1d\n", getAjusteMin);
    Serial.printf("getAjusteMax = %.1d\n", getAjusteMax);
    Serial.printf("getAjusteLumens = %.1d\n", getAjusteLumens);
    Serial.printf("getTempoMovimento = %.1d\n", getTempoMovimento);
  */
  //Serial.printf("%.1d.L%.1d.%.1d.%.1d.%.1d.%.1d.%.1d.%.1d.%.1d.%.1d.%.1d\n", getTxRx, getLuminaria, getModo, getModoLumens, getModoCam, getIntensidade, getTempoTransicao, getAjusteMin, getAjusteMax, getAjusteLumens, getTempoMovimento);

  delay(10);
}
