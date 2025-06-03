#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);

const int potPin = 34;

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {
  while (!client.connected()) {
    client.connect("HoneyDropSensor");
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int leitura = analogRead(potPin);
  int peso = map(leitura, 0, 4095, 0, 100); // simula 0–100g

  Serial.print("Enviando peso (g): ");
  Serial.println(peso);

  String payload = String(peso);
  client.publish("/honeydrop/peso", payload.c_str());

  delay(3000);
}
