#include <WiFi.h>

WiFiServer server(80);

void setup() {
  WiFi.softAP("Resonix","12345678");
  Serial.begin(115200);
  delay(10);

  // Connect to Wi-Fi
  // WiFi.begin(ssid, password);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(1000);
  //   Serial.println("Connecting to WiFi...");
  // }
  // Serial.println("Connected to WiFi");
  // Serial.println(WiFi.localIP());

  // Start the server
  server.begin();
}

void loop() {
  
  }
