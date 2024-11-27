#include <Wire.h>
#include <ESP8266WiFi.h>
#include "DHT.h"

DHT dht(D3, DHT11); // Initialize DHT11 sensor
WiFiClient client;

String apiKey = "ICRJMFA4WFTKDZRG"; // Add your ThingSpeak API key
const char *ssid =  "Shubham"; // Add your WiFi SSID (network name)
const char *pass =  "shubhamjha"; // Add your WiFi password
const char* server = "api.thingspeak.com";

void setup() {
  Serial.begin(115200);
  delay(10);
  
  dht.begin(); // Start DHT sensor
  
  // Connect to WiFi
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop() {
  // DHT11 sensor
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Rain sensor
  int r = analogRead(A0);
  r = map(r, 0, 1024, 0, 100); // Scale value to percentage

  // Send data to ThingSpeak
  if (client.connect(server, 80)) {
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(t); // Temperature
    postStr += "&field2=";
    postStr += String(h); // Humidity
    postStr += "&field3=";
    postStr += String(r); // Rain sensor value
    postStr += "\r\n\r\n\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n\n\n");
    client.print(postStr);

    Serial.print(t);
Serial.print(",");
Serial.print(h);
Serial.print(",");
Serial.println(r);

  }

  client.stop();
  delay(1000); // Wait for 1 second before next update
}