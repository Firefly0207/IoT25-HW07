// HW07_Client.ino

#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#define SERVICE_UUID "91bad492-b950-4226-aa2b-4ede9fa42f59"

BLEScan* pBLEScan;

// TxPower: RSSI value at 1 meter distance
const int TxPower = -59;
const float pathLossExponent = 2.0;  // Typical indoor environment

// Function to estimate distance in meters from RSSI
float estimateDistance(int rssi) {
  if (rssi == 0) {
    return -1.0; // Invalid RSSI value
  }
  float ratio_db = TxPower - rssi;
  float ratio_linear = ratio_db / (10.0 * pathLossExponent);
  float distance = pow(10, ratio_linear);
  return distance;
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(BLEUUID(SERVICE_UUID))) {
      int rssi = advertisedDevice.getRSSI();
      float distanceMeters = estimateDistance(rssi);

      Serial.print("Found Device: ");
      Serial.print(advertisedDevice.getName().c_str());
      Serial.print(", RSSI: ");
      Serial.print(rssi);
      Serial.print(" dBm, Estimated distance: ");
      if (distanceMeters < 0) {
        Serial.println("Unknown");
      } else {
        Serial.print(distanceMeters, 2);  // Print distance with 2 decimal places
        Serial.println(" m");
      }
    }
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE Client...");

  // Initialize BLE
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();  // Create BLE scan object
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1000);  // Scan interval (ms)
  pBLEScan->setWindow(500);     // Scan window (ms)
  pBLEScan->setActiveScan(true);  // Enable active scan
}

void loop() {
  Serial.println("Scanning for BLE devices...");
  pBLEScan->start(5, false);  // Scan for 5 seconds
  delay(1000);  // Wait 1 second before scanning again
}
