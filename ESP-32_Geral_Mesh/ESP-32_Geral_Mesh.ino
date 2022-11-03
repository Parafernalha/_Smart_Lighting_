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

unsigned long sendDataPrevMillis = 0;                                                                //Variáveis
bool signupOK = false;
float floatValue = 0;
float output = 0;
float outputPwm = 0;
float humi = 0;
float tempC = 0;
int x = 0;
int get1 = 255;
int lumens = 0;
int cam = 0;
int contCam = 501;
int ajustePwm = 1;
String mensagem = "";
int getL1 = 0;
int getHumi = 0;
int getTemp = 0;
int getLumens = 0;
int getEfic = 0;

Scheduler userScheduler;
painlessMesh  mesh;

void sendMessage() ;

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

void sendMessage() {
  String msg = mensagem;
  //msg += mesh.getNodeId();
  mesh.sendBroadcast( msg );
  taskSendMessage.setInterval(TASK_SECOND * 1);
}

void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());

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


  mensagem = ("L1.");
  mensagem += (int(tempC));
  mensagem += (".");
  mensagem += (int(humi));
  mensagem += (".");
  mensagem += (lumens);
  mensagem += (".");
  mensagem += (cam);
  mensagem += (".");

  int y = 0;
  int cont = 0;
  for (int x = 0; x < mensagem.length(); x++) {
    if (mensagem.substring(x, x + 1) == ".") {
      if (cont == 0) {
        getL1 = (mensagem.substring(y + 1, x)).toInt();
      }
      if (cont == 1) {
        getHumi = (mensagem.substring(y + 1, x)).toInt();
      }
      if (cont == 2) {
        getTemp = (mensagem.substring(y + 1, x)).toInt();
      }
      if (cont == 3) {
        getLumens = (mensagem.substring(y + 1, x)).toInt();
      }
      if (cont == 4) {
        getEfic = (mensagem.substring(y + 1, x)).toInt();
      }
      cont++;
      y = x;
    }
  }

  Serial.printf("L = %.1d\n", getL1);
  Serial.printf("Umidade = %.1d\n", getHumi);
  Serial.printf("Temperatura = %.1d\n", getTemp);
  Serial.printf("Lumens = %.1d\n", getLumens);
  Serial.printf("Eficiencia = %.1d\n", getEfic);

  delay(10);
}
