#include "Global.h"
#include <uni.h>

// ----------------- Main -------------------
void processGamepad(ControllerPtr ctl) {
  bool L1pressed = ctl->l1();
  bool R1pressed = ctl->r1();
  bool L2pressed = ctl->l2();
  bool R2pressed = ctl->r2();
  
  bool Xpressed = ctl->buttons() == 0x0001;
  bool TrianglePressed = ctl->buttons() == 0x0008;
  bool SquarePressed = ctl->buttons() == 0x0004;
  bool CirclePressed = ctl->buttons() == 0x0002;
  
  
  int joystickY_value = ctl->axisY() ;    // joystick kanan atas-bawah → maju/mundur
  int joystickX_value = ctl->axisRX();     // joystick kiri kanan-kiri → belok
  int joystickRY_value = ctl->axisY();    // opsional (jika digunakan untuk fitur lain)

  int speed = 0;
  int turnSpeed = 0;

// Dibalik agar joystick ke atas → maju
// Kecepatan maksimal diturunkan agar lebih mudah dikontrol.
  speed = exponentialMapping(joystickY_value, 120.0);
  turnSpeed = exponentialMapping(joystickX_value, 80.0);
  if (speed == 0) {
    // Kecepatan putar di tempat (pivot) diturunkan agar lebih halus.
    turnSpeed = exponentialMapping(joystickX_value, 80.0) * -1;
  }
  


  if (!L1pressed && !R1pressed) {
    ctl->setColorLED(0, 204, 204);
  }

  // ------------ L2 Pressed ------------------ belok tajam ke satu arah 
  if (L2pressed) {
    if (speed == 0) {
      turnSpeed = 60;
    }

    if (speed != 0 && (speed <= 10 && speed >= -10)) {
      turnSpeed = -40;
      Serial.println("L2 Kecil");
    }

    if ((speed >= 10 && speed <= 80) || (speed <= -10 && speed >= -80)) {
      turnSpeed = -50;
      Serial.println("L2 Gede Aja");
    }

    if (speed > 80 || speed < -80) {
      turnSpeed = -65;
      Serial.println("L2 Gede Bgt");
    }
  }

  // -------------- R2 Pressed -------------- 
  if (R2pressed) {
    if (speed == 0) {
      turnSpeed = -70;
    }
    if (speed != 0 && (speed <= 10 && speed >= -10)) {
      turnSpeed = 40;
      Serial.println("R2 Kecil");
    }

    if ((speed >= 10 && speed <= 80) || (speed <= -10 && speed >= -80)) {
      turnSpeed = 50;
      Serial.println("R2 Gede Aja");
    }

    if (speed > 80 || speed < -80) {
      turnSpeed = 65;
      Serial.println("R2 Gede Bgt");
    }
  }

  if (Xpressed && millis() - delayKicker <= 50) {
    digitalWrite(KICKER, HIGH);
    digitalWrite(KICKER, LOW);
    Serial.print("KICK");
    delayKicker = millis();
  }
  // ------------ SKILL -----------------------
  // boost
  if (speed != 0 && R1pressed) {
    ctl->setColorLED(255, 0, 127);
    if (getarMillis <= 250 && !getar_flag) {
      ctl->playDualRumble(0, 500, 0x80, 0x40);
      getarMillis = millis();
      getar_flag = true;
    } else {
      getarMillis = 0;
      ctl->playDualRumble(0, 0, 0, 0);
    }

    if (speed < 0) {
      speed = -180;  // Boosting
      Serial.println("BOOST BACK");
    } else {
      speed = 180;  // Boosting
      Serial.println("BOOST FWD");
    }
    turnSpeed = exponentialMapping(joystickX_value, 225.0);

  } else {
    getar_flag = false;
  }

  //  reduce
  if (speed != 0 && L1pressed) {
    ctl->setColorLED(255, 255, 0);

    if (speed < 0) {
      speed = -20;  // reduce

    } else {
      speed = 20;
      Serial.println("REDUCE FRWD");
    }
  }

//speed hold sebelumnya 20


  // --------------- Putar 180 ----------------
  if (TrianglePressed && !turnStarted && !isTurning) {
    isTurning = true;
    turnStarted = true;
    turnMillis = millis();
    Serial.println("PUTAR");
  }

  if (isTurning) {
    if (millis() - turnMillis <= 130) {
      turnSpeed = 180;
    } else {
      isTurning = false;
      Serial.println("STOP PUTAR");
    }
  }
  // reset
  if (!TrianglePressed) {
    turnStarted = false;
  }

  // ------------- Putar 90 ---------------------
  if (CirclePressed && !turn90started && !isTurning90) {
    isTurning90 = true;
    turn90started = true;
    turn90Millis = millis();
    Serial.println("PUTAR 90");
  }
  if (isTurning90) {
    if (millis() - turn90Millis <= 80) {
      turnSpeed = -180;
    } else {
      isTurning90 = false;
      Serial.println("STOP PUTAR");
    }
  }
  // reset
  if (!CirclePressed) {
    turn90started = false;
  }

  // // -------------  Putar -90  ---------------------
  if (SquarePressed && !turn90started_reverse && !isTurning90_reverse) {
    isTurning90_reverse = true;
    turn90started_reverse = true;
    turn90Millis_reverse = millis();
    Serial.println("PUTAR -90");
  }
  if (isTurning90_reverse) {
    if (millis() - turn90Millis_reverse <= 80) {
      turnSpeed = 180;
    } else {
      isTurning90_reverse = false;
      Serial.println("STOP PUTAR");
    }
  }
  // reset
  if (!SquarePressed) {
    turn90started_reverse = false;
  }

  Motion(speed, turnSpeed);
  
  // Debugging output  
  Serial.print("Speed : ");
  Serial.print(speed);
  Serial.print("Turn : ");
  Serial.print(turnSpeed);
  Serial.print(" axis Y : ");
  Serial.print(ctl->axisY());
  Serial.print(" axis RY : ");
  Serial.print(ctl->axisRY());
  Serial.print("  Buttons : ");
  Serial.println(ctl->buttons(), HEX);
 


  // dumpGamepad(ctl); // for debugging
}

// ---------------- Which controller do u use? ----------------
void processControllers() {
  for (auto myController : myControllers) {
    if (myController && myController->isConnected() && myController->hasData()) {
      if (myController->isGamepad()) {
        processGamepad(myController);
      } else {
        Serial.println("Unsupported controller");
      }
    }
  }
}


void setup() {
  Serial.begin(115200);
  Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
  const uint8_t* addr = BP32.localBdAddress();
  Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

  // ------- //


  uni_bt_allowlist_init();
    
  bd_addr_t addr2;

  // sscanf_bd_addr(controller1_addr_string, addr1);
  sscanf_bd_addr(controller2_addr_string, addr2);
  // uni_bt_allowlist_add_addr(addr1);
  uni_bt_allowlist_add_addr(addr2);
  uni_bt_allowlist_set_enabled(true);                                                                                                                                                                                                                                  

  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.forgetBluetoothKeys();
  // BP32.enableNewBluetoothConnections(true);
  BP32.enableVirtualDevice(false);
  // ------ //
  

  pinMode(RIGHT_FORWARD, OUTPUT);
  pinMode(RIGHT_BACKWARD, OUTPUT);
  pinMode(LEFT_FORWARD, OUTPUT);
  pinMode(LEFT_BACKWARD, OUTPUT);
  pinMode(KICKER, OUTPUT);

  pinMode(MOTOR_ENABLE, OUTPUT);
  digitalWrite(MOTOR_ENABLE, HIGH);

  startMillis = millis();
}

void loop() {
  bool dataUpdated = BP32.update();
  if (dataUpdated) {
    processControllers();
  }
}
