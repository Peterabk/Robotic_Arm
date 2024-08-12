#include <ESP32Servo.h>

#define SERVO_BASE_PIN 27
#define SERVO_SHOULDER_PIN 26
#define SERVO_ELBOW_PIN 25
#define SERVO_GRIPPER_PIN 33  // Changed from 35 to 33 for compatibility

#define BASE_START 90
#define SHOULDER_START 90
#define ELBOW_START 90
#define GRIPPER_START 0

Servo base;
Servo shoulder;
Servo elbow;
Servo gripper;
Servo* currentMotor = nullptr;  // Changed to pointer for safety

uint8_t value_idx = 0;
char value[4] = "000";

void reach_goal(Servo& motor, int goal) {
  int currentPos = motor.read();
  if (goal >= currentPos) {
    for (int pos = currentPos; pos <= goal; pos++) {
      motor.write(pos);
      delay(5);
    }
  } else {
    for (int pos = currentPos; pos >= goal; pos--) {
      motor.write(pos);
      delay(5);
    }
  }
}

void setup() {
  base.attach(SERVO_BASE_PIN);
  shoulder.attach(SERVO_SHOULDER_PIN);
  elbow.attach(SERVO_ELBOW_PIN);
  gripper.attach(SERVO_GRIPPER_PIN);

  base.write(BASE_START);
  shoulder.write(SHOULDER_START);
  elbow.write(ELBOW_START);
  gripper.write(GRIPPER_START);

  Serial.begin(115200);
  Serial.setTimeout(1);
}

void loop() {
  if (Serial.available()) {
    char chr = Serial.read();
    switch (chr) {
      case 'b':
        currentMotor = &base;
        value_idx = 0;
        break;

      case 's':
        currentMotor = &shoulder;
        value_idx = 0;
        break;

      case 'e':
        currentMotor = &elbow;
        value_idx = 0;
        break;

      case 'g':
        currentMotor = &gripper;
        value_idx = 0;
        break;

      case ',':
        if (currentMotor != nullptr) {
          int val = atoi(value);
          reach_goal(*currentMotor, val);

          value[0] = '0';
          value[1] = '0';
          value[2] = '0';
          value[3] = '\0';
        }
        break;

      default:
        if (value_idx < 3) {
          value[value_idx] = chr;
          value_idx++;
        }
        break;
    }
  }
}
