int valorConvertido = 0;
float outputPwm = 0;
float valorAjuste = 0.2;

void setup() {
  pinMode(34, INPUT);
  Serial.begin(115200);
}

void loop() {
  valorConvertido = map(analogRead(34),0 ,4095, 255, 0);
  
  if (valorConvertido > outputPwm) {
    outputPwm = outputPwm + valorAjuste; 
  }

  if (valorConvertido < outputPwm) {
    outputPwm = outputPwm - valorAjuste; 
  }
  
  Serial.print(valorConvertido);
  Serial.print("---");
  Serial.println(int(outputPwm));
  delay(10);
}
