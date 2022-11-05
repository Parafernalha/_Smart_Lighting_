#include "painlessMesh.h"

#define   MESH_PREFIX     "ESPMESH2022"
#define   MESH_PASSWORD   "51525354"
#define   MESH_PORT       5555

String mensagemEnviada = "";
String mensagemRecebida = "";

Scheduler userScheduler;
painlessMesh  mesh;

void sendMessage() ;

Task taskSendMessage( TASK_SECOND * 3 , TASK_FOREVER, &sendMessage );

void sendMessage() {
  String msg = mensagemEnviada; //"1.1.0.0.0.255.1.1.255.255.1."; // 1rx ou tx, 2luminaria, 3modo, 4adaptar, 5movimento, 6intencidadePWM, 7tempoTransição, 8ajusteMin, 9ajusteMax, 10ajusteLumens, 11tempoMovimento
  mesh.sendBroadcast( msg );
  taskSendMessage.setInterval(TASK_SECOND * 3);
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.println(msg.c_str());
}


void setup() {
  Serial.begin(115200);

  mesh.setDebugMsgTypes( ERROR | STARTUP );

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
}

void loop() {
  mesh.update();
  if (Serial.available() > 0) {
    mensagemEnviada = Serial.readString();
    //Serial.println(mensagemEnviada);
  }
  delay(1000);
}
