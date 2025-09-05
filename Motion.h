#include <cmath>

void Motion(int speed, int turnspeed) {
  speed = constrain(speed, -200, 200);
  turnspeed = constrain(turnspeed, -255, 255);

  if (speed == 0) {
    if (turnspeed < 0) {  // Belok kanan
      analogWrite(LEFT_BACKWARD, -turnspeed);
      analogWrite(RIGHT_BACKWARD, 0);
      analogWrite(LEFT_FORWARD, 0);
      analogWrite(RIGHT_FORWARD, -turnspeed);
    } else {  // Belok kiri
      analogWrite(LEFT_BACKWARD, 0);
      analogWrite(RIGHT_BACKWARD, turnspeed);
      analogWrite(LEFT_FORWARD, turnspeed);
      analogWrite(RIGHT_FORWARD, 0);
    }
  } else {
    bool isMovingBackward = (speed < 0);
    int absoluteSpeed = abs(speed);

    int leftSpeed = absoluteSpeed + turnspeed;
    int rightSpeed = absoluteSpeed - turnspeed;

    leftSpeed = constrain(leftSpeed, -200, 200);
    rightSpeed = constrain(rightSpeed, -200, 200);

    if (isMovingBackward) {  // Mundur
      // float faktorPengurang = 0.65;
      // int finalLeftSpeed = leftSpeed * faktorPengurang;
      // int finalRightSpeed = rightSpeed * faktorPengurang;
      analogWrite(LEFT_BACKWARD, leftSpeed);
      analogWrite(RIGHT_BACKWARD, rightSpeed);
      analogWrite(LEFT_FORWARD, 0);
      analogWrite(RIGHT_FORWARD, 0);
    } else {  // Maju
      analogWrite(LEFT_BACKWARD, 0);
      analogWrite(RIGHT_BACKWARD, 0);
      analogWrite(LEFT_FORWARD, leftSpeed);
      analogWrite(RIGHT_FORWARD, rightSpeed);
    }
  }
}


int exponentialMapping(int rawInput, float maxSpeed, float expWeight = 1) {
  if (rawInput > 512) rawInput = 512;
  if (rawInput < -511) rawInput = -511;

  if (rawInput > 490) rawInput = 512;
  if (rawInput < -490) rawInput = -512;

  // Normalisasi input
  float norm = rawInput / 512.0;

  // Eksponensial Mapping dengan pembobotan
  float expMapped = (1.0 - expWeight) * norm + expWeight * (pow(norm, 2));

  int speed = (int)(expMapped * maxSpeed);

  if (rawInput < 0) {
    return speed * -1;
  } else {
    return speed;
  }
}

int sigmoidMapping(int rawInput, float maxSpeed, float sigmoidWeight = 5){ 
// int sigmoidMapping(int rawInput, float maxSpeed, float sigmoidWeight = 1){
  if (rawInput > 512) rawInput = 512;
  if (rawInput < -511) rawInput = -511;

  if (rawInput > 490) rawInput = 512;
  if (rawInput < -490) rawInput = -512;


  float norm = rawInput / 512.0;

  float sigmoid = (2.0 / (1.0 + exp(- sigmoidWeight * norm))) - 1.0;

  int speed = (int)(sigmoid * maxSpeed);

  return speed;
}


