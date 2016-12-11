#include "am2320.h"

const byte AM2320::CMD_READ_TEMPERATURE_HUMIDITY[] = {0x03, 0x00, 0x04};
const byte AM2320::CMD_READ_DEVICE_INFORMATION[] = {0x03, 0x08, 0x07};

AM2320::AM2320() {
  Wire.begin();
}

uint16_t AM2320::crc16(byte* data, unsigned int len) {
  uint16_t crc = 0xffff;
  while (len--) {
    crc ^= *data++;
    for (byte i = 0; i < 8; i++) {
      if (crc & 1) {
        crc >>= 1;
        crc ^= 0xa001;
      } else {
        crc >>= 1;
      }
    }
  }
  return crc;
}

byte AM2320::writeCommand(const byte cmd[]) {
  // Wake up a sensor
  Wire.beginTransmission(ADDRESS);
  Wire.endTransmission();

  // Send a command
  Wire.beginTransmission(ADDRESS);
  Wire.write(cmd, getCommandLength(cmd));
  return Wire.endTransmission(true);
}

void AM2320::readData(byte data[], int bytes) {
  // Initialize an array
  for (int i = 0; i < bytes; i++) {
    data[i] = 0;
  }

  // Retrive data
  Wire.requestFrom(ADDRESS, bytes);
  for (int i = 0; i < bytes; i++) {
    data[i] = Wire.read();
  }
}

bool AM2320::callCommand(const byte cmd[], byte result[], int resultBytes) {
  if (writeCommand(cmd)) return false;
  delayMicroseconds(1500);
  readData(result, resultBytes);
  return true;
}

bool AM2320::ReadTemperatureHumidity(struct AM2320Result* result) {
  byte data[8];
  if (!callCommand(CMD_READ_TEMPERATURE_HUMIDITY, data, 8)) return false;
  result->humidity = (data[2] << 8) | data[3];
  result->temperature = (data[4] << 8) | data[5];
  result->crc = (data[6] << 8) | data[7];
  result->crcOk = result->crc == crc16(data, 6);
  return true;
}

bool AM2320::ReadDeviceInfo(struct AM2320DeviceInfo* result) {
  byte data[11];
  if (!callCommand(CMD_READ_DEVICE_INFORMATION, data, 11)) return false;
  result->model = (data[2] << 8) | data[3];
  result->version = data[4];
  result->id = (data[5] << 24) | (data[6] << 16) | (data[7] << 8) | data[8];
  result->crc = (data[9] << 8) | data[10];
  result->crcOk = result->crc == crc16(data, 9);
  return true;
}

