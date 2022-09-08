#include "eloquent.h" //Bibliotecas utilizadas
#include "eloquent/vision/motion/naive.h"
#include "eloquent/vision/camera/esp32/aithinker/gray/qqvga.h" // Resolução da camera para ESP32_CAM (vga = mais alta, qqvga = mais baixa)    
#define largura_Cam 100 //Resolução de operação (Aumentar implica em melhor detecção e menor velocidade de operação)
#define altura_Cam 80

Eloquent::Vision::Motion::Naive<largura_Cam, altura_Cam> detector;

float sensibilidade_AjusteCam = 0.005;
int contagem_AjusteCam = 1;
int contagemMax_AjusteCam = 150;

void setup() {
  delay(4000);
  Serial.begin(115200);
  pinMode(4, OUTPUT);

  camera.setHighFreq(); // Ative a alta frequência para velocidade de transmissão rápida

  if (!camera.begin()) // Conexão com camera
    eloquent::abort(Serial, "Camera init error");
  Serial.println("Camera init OK");

  detector.startSinceFrameNumber(10); // Aguarde pelo menos X quadros serem processados ​​antes de começar a detectar
  detector.debounceMotionTriggerEvery(10); // Quando o movimento for detectado, não acione nos próximos X quadros
  detector.throttle(10); // Em uma chamada
}

void loop() {
  detector.setPixelChangesThreshold(sensibilidade_AjusteCam); // Acionar o movimento quando pelo menos 10% dos pixels mudam de intensidade
  detector.setIntensityChangeThreshold(10); // Sensibilidade considerando pelo menos 10 de 255 por pixel
  
  if (!camera.capture()) { // Confere se a camera está funcionando
    Serial.println(camera.getErrorMessage());
    delay(1000);
    return;
  }

  digitalWrite(4, LOW);

  camera.image.resize<largura_Cam, altura_Cam>(); // Realizar detecção de movimento em imagem redimensionada para detecção rápida
  detector.update(camera.image);
  
  if (contagem_AjusteCam < contagemMax_AjusteCam) {
    Serial.println(contagem_AjusteCam);
    contagem_AjusteCam++;
  }
  else if (contagem_AjusteCam < contagemMax_AjusteCam + 1) {
    contagem_AjusteCam++;
    sensibilidade_AjusteCam = sensibilidade_AjusteCam + 0.005;
  }

  if (detector.isMotionDetected()) {   // Se for detectado movimento, imprima as coordenadas para serial no formato JSON
    if (contagem_AjusteCam < contagemMax_AjusteCam) {
      sensibilidade_AjusteCam = sensibilidade_AjusteCam + 0.005;
    }
    detector.printAsJsonTo(Serial);
    Serial.println(sensibilidade_AjusteCam);
    digitalWrite(4, HIGH);
  }

  camera.free(); // liberar memória
}
