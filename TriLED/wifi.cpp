#include "WString.h"
#include "wifi.h"

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "lamp.h"

String ssid = "####";
String password = "########";

ESP8266WebServer server(80);

void handleIndex() {
  server.send(200, "text/plain", "TriLED API\n");
}
void handleHue1() {
  hue1 = server.arg("val").toInt();
  server.send(200, "text/plain", "OK\n");
}
void handleHue2() {
  hue2 = server.arg("val").toInt();
  server.send(200, "text/plain", "OK\n");
}
void handleBrightness() {
  brightness = server.arg("val").toFloat() * 0.01f;
  server.send(200, "text/plain", "OK\n");
}
void handleNotFound() {
  server.send(404, "text/plain", "404: Not found\n");
}

void setup_wifi() {
  Serial.begin(115200);
  delay(10);
  Serial.println('\n');

  WiFi.begin(ssid, password);
  Serial.printf("Connecting to %s\n", ssid);

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.printf("Connecting to %s: %ds\n", ssid, ++i);
  }

  Serial.printf("WiFi connected. IP address: %s\n", WiFi.localIP().toString());

  server.on("/", handleIndex);
  server.on("/hue1", handleHue1);
  server.on("/hue2", handleHue2);
  server.on("/brightness", handleBrightness);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("Http server started");
}

void loop_wifi() {
  server.handleClient();
}