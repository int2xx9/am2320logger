#pragma once

struct AM2320Result {
  uint16_t humidity;
  short temperature;
  uint16_t crc;
  bool crcOk;
};

struct AM2320DeviceInfo {
  uint16_t model;
  byte version;
  uint32_t id;
  uint16_t crc;
  bool crcOk;
};

class AM2320 {
  static const byte CMD_READ_TEMPERATURE_HUMIDITY[];
  static const byte CMD_READ_DEVICE_INFORMATION[];
  static const int ADDRESS = 0xb8>>1;

  static constexpr byte getCommandLength(const byte cmd[]) {
    return sizeof(cmd)/sizeof(cmd[0]);
  }

  uint16_t crc16(byte* data, unsigned int len);
  byte writeCommand(const byte cmd[]);
  void readData(byte data[], int bytes);
  bool callCommand(const byte cmd[], byte result[], int resultBytes);
public:
  AM2320();
  bool ReadTemperatureHumidity(struct AM2320Result* result);
  bool ReadDeviceInfo(struct AM2320DeviceInfo* result);
};

