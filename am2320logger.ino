#include <Wire.h>
#include <ESP8266WiFi.h>
#include "am2320.h"
#include "config.h"

class LoggerServer {
  String host;
  uint16_t port;
public:
  LoggerServer(String host, uint16_t port)
    : host(host), port(port) {
  }
  void Update(uint16_t humidity, short temperature) {
    String humidityStr = String(humidity/10, DEC) + "." + String(humidity%10, DEC);
    String temperatureStr = String(temperature/10, DEC) + "." + String(abs(temperature)%10, DEC);
    WiFiClient client;
    if (client.connect(host.c_str(), port)) {
      client.println("POST /update?temperature=" + temperatureStr + "&humidity=" + humidityStr + " HTTP/1.1");
      client.println("Host: " + host);
      client.println("Connection: close");
      client.println("Content-Length: 0");
      client.println();

      // Wait for response
      while (!client.available()) {
        delay(100);
      }

      String response;
      while (client.available()) {
        response += client.read();
      }
      client.stop();
    }
  }
};

void setup() {
  Serial.println("Start setup");
  Serial.begin(115200);
  delay(1000);

  Serial.print("Connect to AP");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("connected!");

  AM2320 dev;
  struct AM2320Result result;
  for (int i = 0; i < RETRY_MAX; i++) {
    if (dev.ReadTemperatureHumidity(&result)) {
      String humidityStr = String(result.humidity/10, DEC) + "." + String(result.humidity%10, DEC);
      String temperatureStr = String(result.temperature/10, DEC) + "." + String(abs(result.temperature)%10, DEC);
      Serial.print("Humidity:" + humidityStr);
      Serial.print(", Temperature:" + temperatureStr);
      Serial.println(", crc: " + String(result.crc, HEX) + " / " + result.crcOk ? "ok" : "error");
      if (result.crcOk) break;
    }
    delay(RETRY_INTERVAL);
  }

  LoggerServer server(SERVER_HOST, SERVER_PORT);
  server.Update(result.humidity, result.temperature);

  Serial.println("Start sleeping");
  ESP.deepSleep(INTERVAL, WAKE_RF_DEFAULT);
}

void loop() {}

