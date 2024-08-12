#include <ESP32Servo.h>


Servo motor;

void setup() {
  // put your setup code here, to run once:
  motor.attach(27);

  //turn the motor in degrees
  motor.write(90);
  
  Serial.begin(115200);
  Serial.setTimeout(1);
}

void loop() {
  // put your main code here, to run repeatedly:
  //Checking if we received Serial from ros2 
  if(Serial.available()){
    int angle = Serial.readString().toInt();
    motor.write(angle);
  }
  delay(50);

}
