#include <WiFi.h>
#include <WebSocketsServer.h>

// Contantes
const char* ssid = "Amilton"; //Rede
const char* password = "contra619"; //Senha

// Globals
WebSocketsServer webSocket = WebSocketsServer(80);

// Chamado ao receber qualquer mensagem WebSocket
void onWebSocketEvent(uint8_t num,
                      WStype_t type,
                      uint8_t * payload,
                      size_t length) {

  // Descobrir o tipo de evento WebSocket
  switch(type) {

    // Client has disconnected
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;

    // New client has connected
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connection from ", num);
        Serial.println(ip.toString());
      }
      break;

    // Ecoar mensagem de texto de volta ao cliente
    case WStype_TEXT:
      Serial.printf("[%u] Text: %s\n", num, payload);
      char bin[16];
      strcpy(bin, (char *)(payload));
      webSocket.sendTXT(num, "O valor recebido foi: " + String(bin));
      break;

    // For everything else: do nothing
    default:
      break;
  }
}

void setup() {

  // Start Serial port
  Serial.begin(115200);

  // Conectar ao ponto de acesso
  Serial.println("Connecting");
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
  }

  // Imprima nosso endereço IP
  Serial.println("Connected!");
  Serial.print("My IP address: ");
  Serial.println(WiFi.localIP());

  // Inicie o servidor WebSocket e atribua o retorno de chamada
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
}

void loop() {

  // Procure e manipule dados do WebSocket
  webSocket.loop();
}
