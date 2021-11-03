#include <ESP32Servo.h>

Servo motor;
#define servopin 27

void setup() {
  motor.attach(servopin);   
}

void loop() {
  AbrirPuerta();
  CerrarPuerta();  
}


void AbrirPuerta(){
  motor.write(9);
  delay(500);
  motor.write(20);
  delay(500);
  motor.write(30);
  delay(500);
   motor.write(40);
  delay(500);
    motor.write(50);
  delay(500);
  motor.write(60);
  delay(500);
  motor.write(70);
  delay(500);
  motor.write(80);
  delay(500);
  motor.write(90);
  delay(500);
  motor.write(100);
  delay(500);
    motor.write(110);
  delay(500);
    motor.write(120);
  delay(500);
  motor.write(130);
  delay(500);
}
void CerrarPuerta(){
  motor.write(130);
  delay(500);
  motor.write(120);
  delay(500);
  motor.write(110);
  delay(500);
  motor.write(100);
  delay(500);
  motor.write(90);
  delay(500);
  motor.write(80);
  delay(500);
  motor.write(70);
  delay(500);
  motor.write(60);
  delay(500);
  motor.write(50);
  delay(500);
  motor.write(40);
  delay(500);
  motor.write(30);
  delay(500);
  motor.write(20);
  delay(500);
  motor.write(10);
  delay(500);
}
