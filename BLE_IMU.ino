
/*
  BLE IMU sketch on projecthub
  -----------------------------
  Arduino MKR 1010 + IMU Shield

  FOR USE WITH WEB APP DEMO AT :
  https://8bitkick.github.io/ArduinoBLE-IMU.html
  
  IMPORTANT - if required update MKR 1010 fw for bluetooth support first 
  See https://forum.arduino.cc/index.php?topic=579306.0
  
*/

#include <ArduinoBLE.h>
#include <MKRIMU.h>

 // BLE Service
BLEService imuService("917649A0-D98E-11E5-9EEC-0002A5D5C51B"); // Custom UUID

// BLE Characteristic
BLECharacteristic imuCharacteristic("917649A1-D98E-11E5-9EEC-0002A5D5C51B", BLERead | BLENotify, 12);

long previousMillis = 0;  // last timechecked, in ms

void setup() {
  Serial.begin(9600);    // initialize serial communication

  pinMode(LED_BUILTIN, OUTPUT); // initialize the built-in LED pin to indicate when a central is connected

// begin initialization
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  Serial.print("Euler Angles sample rate = ");
  Serial.print(IMU.eulerAnglesSampleRate());
  Serial.println(" Hz");
  Serial.println();
  Serial.println("Euler Angles in degrees");
  Serial.println("X\tY\tZ");
  
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
  }

  // Setup bluetooth
  BLE.setLocalName("ArduinoIMU");
  BLE.setAdvertisedService(imuService); 
  imuService.addCharacteristic(imuCharacteristic);
  BLE.addService(imuService); 
  
  // start advertising
  BLE.advertise();
  Serial.println("Bluetooth device active, waiting for connections...");
}


// send IMU data
void sendSensorData() {
float eulers[3];

// read orientation x, y and z eulers
IMU.readEulerAngles(eulers[0], eulers[1], eulers[2]); 

// Send 3x eulers over bluetooth as 1x byte array 
imuCharacteristic.setValue((byte *) &eulers, 12); 

} 

void loop() {
  // wait for a BLE central
  BLEDevice central = BLE.central();

  // if a BLE central is connected to the peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's BT address:
    Serial.println(central.address());
    // turn on the LED to indicate the connection:
    digitalWrite(LED_BUILTIN, HIGH);

    // while the central is connected:
    while (central.connected()) {
      long currentMillis = millis();

      if (currentMillis - previousMillis >= 50) {
          if (IMU.eulerAnglesAvailable()) {
        previousMillis = currentMillis;
        sendSensorData();
          }
      }
    }
    // when the central disconnects, turn off the LED:
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}
