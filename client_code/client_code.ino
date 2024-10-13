#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <coap-simple.h>

// WiFi credentials
const char* ssid = "RANA";
const char* password = "1122334455";

// CoAP server details
IPAddress server_ip(192, 168, 1, 100);
int server_port = 5683;
const int ledPin = D4;

// CoAP client
WiFiUDP udp;
Coap coap(udp);

// Client IP
IPAddress client_ip(192, 168, 1, 110);

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

  // Set up CoAP client
  WiFi.config(client_ip, IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
  coap.response(callback_response);
  coap.start();
}

void loop() {
  // Perform GET request
  unsigned long start_time = millis();
  int msgid = coap.get(server_ip, server_port, "sensor_data");
  
  while (!coap.loop()) {
    delay(10);
  }
  
  unsigned long get_delay = millis() - start_time;
  Serial.print("GET delay: ");
  Serial.print(get_delay);
  Serial.println(" ms");

  delay(5000);

  // Perform PUT request (instead of POST)
  start_time = millis();
  msgid = coap.put(server_ip, server_port, "sensor_data", "42", strlen("42"));
  
  while (!coap.loop()) {
    delay(10);
  }
  
  unsigned long put_delay = millis() - start_time;
  Serial.print("PUT delay: ");
  Serial.print(put_delay);
  Serial.println(" ms");

  delay(5000);
}

// CoAP response callback
void callback_response(CoapPacket &packet, IPAddress ip, int port) {
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = '\0';

  if(p){
    digitalWrite(LED_BUILTIN, HIGH);  // LED on
    Serial.println("LED is ON");
    }
  

  Serial.print("CoAP Response: ");
  Serial.println(p);
}
