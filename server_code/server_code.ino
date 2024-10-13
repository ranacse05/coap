#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <coap-simple.h>

// WiFi credentials
const char* ssid = "RANA";
const char* password = "1122334455";

// CoAP server endpoint
IPAddress ip(192, 168, 1, 100);
int port = 5683;

// CoAP server
WiFiUDP udp;
Coap coap(udp);

// Sensor data (simulated)
int sensor_data = 0;

// CoAP resource callback
void callback_sensor_data(CoapPacket &packet, IPAddress ip, int port) {
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = '\0';

  String message(p);

  if (packet.type == COAP_PUT) {
    sensor_data = message.toInt();
    Serial.print("Sensor data updated to: ");
    Serial.println(sensor_data);
    coap.sendResponse(ip, port, packet.messageid, "Updated");
  } else {
    Serial.print("Sending sensor data: ");
    Serial.println(sensor_data);
    coap.sendResponse(ip, port, packet.messageid, String(sensor_data).c_str());
  }
}

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Set up CoAP server
  WiFi.config(ip, IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
  coap.server(callback_sensor_data, "sensor_data");
  
  // Start CoAP server
  coap.start();
  Serial.println("CoAP server started");
}

void loop() {
  coap.loop();
  delay(1000);
  // Simulate changing sensor data
  sensor_data = random(0, 100);
}
