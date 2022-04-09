#include "BLEDevice.h"

#include "Adafruit_VL53L0X.h"
#define HAS_VL53L0X 1

String serverUUID = "ba:03:21:2e:1b:09";
static const BLEUUID g_serviceUUID(uint16_t(0xffe5));
static const BLEUUID g_characteristicUUID(uint16_t(0xffe9));

BLEClient* g_client = BLEDevice::createClient();
const BLEAddress g_address(serverUUID.c_str());
BLERemoteService* g_remoteService = nullptr;
BLERemoteCharacteristic* g_remoteCharacteristic = nullptr;

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup() {
  Serial.begin(115200);
  Serial.println("Hello");

#if 0
  BLEDevice::init("");
  Serial.println(BLEDevice::getInitialized() ? F("BLE Initialized") : F("BLE not initialized"));

  for (uint i=0; i < 5; i++)
  {
    Serial.printf("%s #%d\n", F("Attempting to connect... Attempt"), i);
    if (g_client->connect(g_address))
    {
      break;
    }
    delay(500);
  }

  if(!g_client->isConnected())
  {
    Serial.println(F("Failed to connect"));
    Serial.flush();
    exit(1);
  } else {
    Serial.printf("%s %s\n", F("Connected to"), g_client->getPeerAddress().toString().c_str());
  }

  g_remoteService = g_client->getService(g_serviceUUID);
  if (g_remoteService == nullptr)
  {
    Serial.printf("%s 0x%04x", F("Failed to access service"), g_serviceUUID);
    Serial.flush();
    exit(1);
  }

  g_remoteCharacteristic = g_remoteService->getCharacteristic(g_characteristicUUID);
  if (g_remoteCharacteristic == nullptr || !g_remoteCharacteristic->canWrite())
  {
    Serial.printf("%s 0x%04x", F("Failed to access service characteristic"), g_characteristicUUID);
    Serial.flush();
    exit(1);
  }

#if 0
  {
    uint8_t command[] = { 0xAA, 0x04, 0x01, 0x05 };
    g_remoteCharacteristic->writeValue(&command[0], size_t(sizeof(command)), false);
    Serial.printf("sent 0x%02X 0x%02X 0x%02X 0x%02X\n", command[0], command[1], command[2], command[3]);
  }
  delay(5000);
  {
    uint8_t command[] = { 0xAA, 0x04, 0x0E, 0x12 };
    g_remoteCharacteristic->writeValue(&command[0], size_t(sizeof(command)), false);
    Serial.printf("sent 0x%02X 0x%02X 0x%02X 0x%02X\n", command[0], command[1], command[2], command[3]);
  }
  delay(10000);
#endif
  {
    uint8_t command[] = { 0xAA, 0x04, 0x01, 0x05 };
    g_remoteCharacteristic->writeValue(&command[0], size_t(sizeof(command)), false);
    Serial.printf("sent 0x%02X 0x%02X 0x%02X 0x%02X\n", command[0], command[1], command[2], command[3]);
  }

  //Format:
  //0xAA 0x04 {speed-4} {speed}, where speed is 4-100

  //Min:
  //0xAA 0x04 0x07 0x0B = 11 = no movement
  //0xAA 0x04 0x00 0x04 = 0 = off

  //Max:
  //0xAA 0x04 0x60 0x64 = 100
#endif

#if defined(HAS_VL53L0X)
Serial.println("Adafruit VL53L0X test");
  static TwoWire wire = Wire;
  wire.setPins(GPIO_NUM_23, GPIO_NUM_22);
  if (!lox.begin(VL53L0X_I2C_ADDR, false, &wire, Adafruit_VL53L0X::VL53L0X_SENSE_DEFAULT)) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
  // power 
  Serial.println(F("VL53L0X API Simple Ranging example\n\n")); 
#endif
}

void loop() {
  #if defined(HAS_VL53L0X)
  VL53L0X_RangingMeasurementData_t measure;
  Serial.print("Reading a measurement... ");
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter);
  } else {
    Serial.println(" out of range ");
  }
  #endif

  if (Serial.available() > 0)
  {
    auto str = Serial.readStringUntil(0x00);
    
    uint32_t command32[4] = {0x00, 0x00, 0x00, 0x00 };
    auto result = sscanf(str.c_str(), "0x%02X 0x%02X 0x%02X 0x%02X", &command32[0], &command32[1], &command32[2], &command32[3]);
    if(result == 4)
    {
      uint8_t command8[4] = {command32[0], command32[1], command32[2], command32[3]};
      g_remoteCharacteristic->writeValue(&command8[0], size_t(sizeof(command8)), false);
      Serial.printf("sent 0x%02X 0x%02X 0x%02X 0x%02X\n", command8[0], command8[1], command8[2], command8[3]);
    }
    else
    {
      Serial.print(F("invalid command, "));
      Serial.println(str.c_str());
    }   
  }
  delay(10);
}
