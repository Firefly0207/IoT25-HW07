// HW07_Server.ino

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID "91bad492-b950-4226-aa2b-4ede9fa42f59"
#define CHARACTERISTIC_UUID "cba1d466-344c-4be3-ab3f-189f80dd7518"

// BLE server object
BLEServer *pServer;
BLECharacteristic *pCharacteristic;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE Server...");

  // Initialize BLE
  BLEDevice::init("ESP32_SERVER");

  // Create BLE server
  pServer = BLEDevice::createServer();

  // Create service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create characteristic (Read-only, Notify enabled)
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  // Set initial data
  pCharacteristic->setValue("Hello from Server");

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // Minimum advertising interval
  pAdvertising->setMaxPreferred(0x12);  // Maximum advertising interval
  BLEDevice::startAdvertising();

  Serial.println("BLE Server is ready.");
}

void loop() {
  delay(1000);
}
