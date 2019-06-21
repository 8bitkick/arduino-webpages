/*
  Stream sensor data to serial in JSON format
  -------------------------------------------
  Arduino MKR 1010 + IMU Shield
  
*/

#include <MKRIMU.h>
#include <Arduino_JSON.h>

long previousMillis = 0;       // last timechecked, in ms
int  samplePeriod = 1000 / 32; // 1000 / sample rate in HZ

void setup() {
  Serial.begin(9600);    // initialize serial communication

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU");
    while (1);
  }
}

// IMU data
void updateSensorData() {
  float ex, ey, ez;
  IMU.readEulerAngles(ex, ey, ez);

  JSONVar sensorData;
  
  sensorData["ms"] = millis();
  sensorData["ex"] = ex;
  sensorData["ey"] = ey;
  sensorData["ez"] = ez;
  
  Serial.println(sensorData);
}

void loop() {
  long currentMillis = millis();
  
  // if time passed output sensor value
  if (currentMillis - previousMillis >= samplePeriod) {
    if (IMU.eulerAnglesAvailable()) {
      previousMillis = currentMillis;
      updateSensorData();
    }
  }

}
