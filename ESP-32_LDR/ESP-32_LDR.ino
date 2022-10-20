int valorConvertido = 0;
float outputPwm = 0;
float valorAjuste = 0.2;

int pinLed = 26;

TaskHandle_t Task1;
TaskHandle_t Task2;

void setup() {
  pinMode(12, INPUT);
  Serial.begin(115200);

  ledcSetup(0, 5000, 8);
  ledcAttachPin(pinLed, 0);
}


void loop() {
  valorConvertido = map(analogRead(12), 0 , 4095, 255, 0);

  if (valorConvertido > outputPwm) {
    outputPwm = outputPwm + valorAjuste;
  }

  if (valorConvertido < outputPwm) {
    outputPwm = outputPwm - valorAjuste;
  }

  ledcWrite(0, int(outputPwm));

  Serial.print(valorConvertido);
  Serial.print("---");
  Serial.println(int(outputPwm));
  delay(10);
}
