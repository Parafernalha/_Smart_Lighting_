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
}

void Task2code( void * pvParameters ) {
  for (;;) {
    delay(1000);
  }
}

void loop() {

}
