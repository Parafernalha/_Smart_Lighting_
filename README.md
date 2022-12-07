# Smart Lighting

## What it was the challenge?
Develop a proof of concept (POC) of a smart light fixture, which funcionalities would be:

- Sensing: It was mandatory the existence of a luminosity sensor, which would be responsible for vary the amplitude of a PWM signal according to the luminosity read from the environment. There should be implemented another sensors, like temperature sensors, pressure sensors, humidity sensor, among others;
- Remote control: The device should be controlled via smartphone and/or computer, and it was preferable not to use wires;
- Telemetry: The devices should be able to connect to one another;
- Mobile App: there should exist a mobile app;
- Supervisory: There should exist a supervisory system where the user should be able to monitorate the status of the devices as well as the readings from the sensors;
- Compatibility: The lighting device should be able to communicate and control devices made by another manufactures.

## What it was developed?

- Sensing: It was implemented the reading of the luminosity using the Adafruit Sensor BH1750, Temperature and humidity with the DHT22, and a moviment detector with a ESP32 CAM microcontroller;
- Remote control: The lighting devices were successfully remotely controlled using the ESP32 microcontroller and its mesh communication protocol, both via a mobile app and through the supervisory system;
- Telemetry: The ESP Mesh protocol it was used to make the devices communicate properly with one another without using any cable.
- Mobile App: It was implemented using the low code platform MIT Inventor (Ideal for prototyping).
- Supervisory: It was implemented using the low code platform Node Red (Ideal for prototyping).
- Compatibility: The devices were able to control lighting devices made by another manufactures using a 0-10V analog converter.


<img src="https://github.com/Parafernalha/Parafernalha/_Smart_Lighting_/Apprenticeship Journey/images/supervisorio.png" alt="challenge 1" >
<img src="https://raw.githubusercontent.com/Parafernalha/table-challenges/main/images/desafio-dois.png" alt="challenge 1" >
<img src="https://github.com/Parafernalha/table-challenges/blob/main/images/exercicio-escopos-grupo.png" alt="exercicio" >
<img src="https://raw.githubusercontent.com/Parafernalha/table-challenges/main/images/desafio-quatro.png" alt="challenge 2" >
