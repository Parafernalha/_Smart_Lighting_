#include <Arduino.h>
#include <DHT.h>
#include <Arduino_JSON.h>
#include "painlessMesh.h"

#define   MESH_PREFIX     "Sky" //name for your MESH
#define   MESH_PASSWORD   "51525354" //password for your MESH
#define   MESH_PORT       5555 //default port

#define DHT_SENSOR_PIN  32                                                         // DHT11 sensor pin X
#define DHT_SENSOR_TYPE DHT11

#define pinLed 27
#define pinLDR 33
#define pinCAM 26

int nodeNumber = 1;

String readings;

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

TaskHandle_t Task1;
TaskHandle_t Task2;

DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);                                    //Configurações DHT11

Scheduler userScheduler; 
painlessMesh  mesh;

void sendMessage() ; 
String getReadings(); 

Task taskSendMessage(TASK_SECOND * 5 , TASK_FOREVER, &sendMessage);

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

String getReadings () {
  JSONVar jsonReadings;
  jsonReadings["node"] = nodeNumber;
  jsonReadings["temp"] = 30;
  jsonReadings["hum"] = 31;
  jsonReadings["pres"] = 32;
  readings = JSON.stringify(jsonReadings);
  return readings;
}

void sendMessage () {
  String msg = getReadings();
  mesh.sendBroadcast(msg);
}

void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
  JSONVar myObject = JSON.parse(msg.c_str());
  int node = myObject["node"];
  double temp = myObject["temp"];
  double hum = myObject["hum"];
  double pres = myObject["pres"];
  Serial.print("Node: ");
  Serial.println(node);
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" C");
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.println(" %");
  Serial.print("Pressure: ");
  Serial.print(pres);
  Serial.println(" hpa");
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
  
  /*mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);*/
  
  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();

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

    mesh.update();
    
    delay(2000);
  }
}

void loop() {

}
