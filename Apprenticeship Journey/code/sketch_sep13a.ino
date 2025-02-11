
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*---------------------------------- ----------********* JORNADA DE APRENDIZAGEM 6P **********---------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  
/*
This code had been written using the Arduino IDE, prior to the use of this code you should install the following libraries:

- BH1750 by Christopher Laws
- Adafruit Unified Sensor by Adafruit
- Adafruit Circuit Playground by Adafruit
- DHT sensor library
*/


  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*--------------------------------------------*********** INCLUSION OF THE LIBRARIES ********----------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/

#include "DHT.h"
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <BH1750.h>
BH1750 lightMeter;


  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*---------------------------------------------****************** DEFINITIONS ****************---------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/

#define DHTPIN 27
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*---------------------------------------------************ VARIABLES DECLARATIONS ***********---------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  
//
float outputPwm = 0;

// Value used to adjust the variation of the LED
float valorAjuste = 1;

// the number of the LED pin
const int ledPin = 16;  // 16 corresponds to GPIO16

// setting PWM properties
const int freq = 2000;
const int ledChannel = 0;
const int resolution = 8;


  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*--------------------------------------------****************** SETUP **********************----------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
 
void setup(){
  
  Serial.begin(9600);

   dht.begin();
   
  // Initialize the I2C bus (BH1750 library doesn't do this automatically)
  Wire.begin();

  lightMeter.begin();

  Serial.println(F("BH1750 Test begin"));
  
  // configure LED PWM functionalitites
  ledcSetup(ledChannel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(ledPin, ledChannel);
}
 
void loop()

  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------*********** LIGHT SENSOR BH1750 & PWM GENERATION *************----------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
{
  float lux = lightMeter.readLightLevel();
  Serial.print("      ");
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.print(" lx");
  Serial.print("      ");
  float valor = map(lux,0,24635,255,0);
  if (valor < 0) {
  valor = 0;
  }
  Serial.println(valor);

  if (valor > outputPwm)
  {
    outputPwm = outputPwm + valorAjuste; 
  }
  if (valor < outputPwm)
  {
    outputPwm = outputPwm - valorAjuste; 
  }
  
  ledcWrite(0, int(outputPwm)); 
  Serial.print("      ");
  Serial.print(valor);
  Serial.print("      ");
  Serial.print(int(outputPwm));

  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*--------------------------------------*********** TEMPERATURE/HUMIDITY SENSOR DHT22 *************----------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/


  // reads the information from the DHT22 sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity(); 

  if(isnan(temperature) || isnan(humidity))
  {
    Serial.println("Failed to read DHT22");
    delay(2000);
  }
  
  //displays the temperature and humidity
  else
  {
    Serial.print("   ");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print(" %\t");
    Serial.print("   ");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" ºC");
  }

  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-------------------------------------------------------*********** FINISH *************--------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------------------------------------------------------------*/
}
