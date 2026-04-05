#include <WiFi.h>
#include "predict.h"
#include <ArduinoJson.h>
#include <WebServer.h>
#include "helper.h"

WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", html);
}

void handleSend() {
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "Data yoxdur");
    return;
  }

  String body = server.arg("plain");

  StaticJsonDocument<4096> doc;

  DeserializationError error = deserializeJson(doc, body);

  if (error) {
    Serial.println("JSON parse xətası");
    server.send(400, "text/plain", "JSON xətası");
    return;
  }

  JsonArray matrix = doc["matrix"];

  float input[H * W];

  int index = 0;

  for (int y = 0; y < H; y++) {
    JsonArray row = matrix[y];

    for (int x = 0; x < W; x++) {
      input[index++] = row[x];
    }
  }

  // Model texmini
  int result = predict(input);

  Serial.print("Təxmin edilen reqem: ");
  Serial.println(result);

  server.send(200, "text/plain", String(result));
}

void setup() {
  Serial.begin(115200);

  delay(200);

  Serial.println("Sistem baslayir...");

  WiFi.begin(ssid, password);

  Serial.print("WiFi qosulur");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi qosuldu");

  Serial.print("IP adresi: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/send", HTTP_POST, handleSend);

  server.begin();

  Serial.println("Web server hazirdir");
}

void loop() {
  server.handleClient();
}