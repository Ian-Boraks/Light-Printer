//Server Code
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define bleServerName "Light Printer"

BLEServer *pServer;

class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    // raw data packet:
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0)
    {
      Serial.println("*******");
      Serial.print("New Value: ");
      for (int i = 0; i < value.length(); i++)
      {
        Serial.print(value[i]);
      }

      Serial.println();
      Serial.println("*******");
    }
  }
};

class ServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer, esp_ble_gatts_cb_param_t *param)
  {
//    std::map<uint16_t, conn_status_t> clients = pServer->getPeerDevices(true);
//    Serial.println(">> New Connection");
//  
//    for (std::map<uint16_t, conn_status_t>::iterator it = clients.begin(); it != clients.end(); ++it) {
//       Serial.printf("Device: %s %s\n", it->first, it->second);
//     }
    Serial.println("<< New Connection");
  }

  void onDisconnect(BLEServer *pServer)
  {
    Serial.println("<< Disconnected");
  }
};

void setup() {
  Serial.begin(115200);

  BLEDevice::init(bleServerName);
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  pCharacteristic->setCallbacks(new MyCallbacks());

  pCharacteristic->setValue("Hello, World!");
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();

  BLEAdvertisementData advertisementData = BLEAdvertisementData();
  advertisementData.setName(bleServerName);

  pAdvertising->setScanResponseData(advertisementData);

  pAdvertising->start();
}

void loop() {
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  BLEAdvertisementData advertisementData = BLEAdvertisementData();
  advertisementData.setName(bleServerName);

  pAdvertising->setScanResponseData(advertisementData);

  if (pServer->getConnectedCount() == 0)
  {
    pAdvertising->start();
  }

  delay(2000);
}
