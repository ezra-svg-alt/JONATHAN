#define MOTOR_ENABLE 32

// ----------- V1 ----------
#define RIGHT_FORWARD 26
#define RIGHT_BACKWARD 25
#define LEFT_BACKWARD 14
#define LEFT_FORWARD 27
#define KICKER 33

// static const char * controller1_addr_string = "41:42:78:CF:BD:94";

static const char * controller2_addr_string = "30:0e:d5:85:76:af";
//  X :30:0e:d5:85:76:af
// Y:8c:41:f2:83:74:41
// 2 :8c:41:f2:f9:e3:97
unsigned long delayKicker = 0;
unsigned long startMillis;

// ----------- getar ------------------
unsigned long getarMillis;
bool getar_flag = false;

// ------------- turn 180 -----------
unsigned long turnMillis = 0;
bool isTurning = false;
bool turnStarted = false;

// ----------- turn 90 --------------
unsigned long turn90Millis = 0;
bool isTurning90 = false;
bool turn90started = false;

// ----------- turn -90 ------------
unsigned long turn90Millis_reverse = 0;
bool isTurning90_reverse = false;
bool turn90started_reverse = false;


#include <Bluepad32.h>
#include "Motion.h"


ControllerPtr myControllers[BP32_MAX_GAMEPADS];

void dumpGamepad(ControllerPtr ctl) {
  Serial.printf(
    "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
    "misc: 0x%02x, gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d\n",
    ctl->index(),        // Controller Index
    ctl->dpad(),         // D-pad
    ctl->buttons(),      // bitmask of pressed buttons
    ctl->axisX(),        // (-511 - 512) left X Axis
    ctl->axisY(),        // (-511 - 512) left Y axis
    ctl->axisRX(),       // (-511 - 512) right X axis
    ctl->axisRY(),       // (-511 - 512) right Y axis
    ctl->brake(),        // (0 - 1023): brake button
    ctl->throttle(),     // (0 - 1023): throttle (AKA gas) button
    ctl->miscButtons(),  // bitmask of pressed "misc" buttons
    ctl->gyroX(),        // Gyro X
    ctl->gyroY(),        // Gyro Y
    ctl->gyroZ(),        // Gyro Z
    ctl->accelX(),       // Accelerometer X
    ctl->accelY(),       // Accelerometer Y
    ctl->accelZ()        // Accelerometer Z
  );
}

void onConnectedController(ControllerPtr ctl) {
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
      ControllerProperties properties = ctl->getProperties();
      Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id, properties.product_id);
      myControllers[i] = ctl;
      foundEmptySlot = true;
      break;
    }
  }

  if (!foundEmptySlot) {
    Serial.println("CALLBACK: Controller connected, but could not found empty slot");
  }
}

void onDisconnectedController(ControllerPtr ctl) {
  bool foundController = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
      myControllers[i] = nullptr;
      foundController = true;
      break;
    }
  }

  if (!foundController) {
    Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
  }
}
