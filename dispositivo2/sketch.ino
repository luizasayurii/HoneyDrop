#include <ESP32Servo.h>
#include <WiFi.h>
#include <PubSubClient.h>

const int potPin = 34;
const int servoPin = 2;
#define PESO_MINIMO 10

// Wi-Fi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// MQTT
const char* mqtt_server = "broker.hivemq.com";
WiFiClient espClient;
PubSubClient client(espClient);

Servo servoMotor;
bool comedouroAberto = false;

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {
  while (!client.connected()) {
    client.connect("DispositivoPote");
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);
  setup_wifi();
  client.setServer(mqtt_server, 1883);

  servoMotor.attach(servoPin);
  servoMotor.write(0);
  Serial.println("Sistema do Dispositivo 2 iniciado.");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int leitura = analogRead(potPin);
  int peso = map(leitura, 0, 4095, 0, 100);

  Serial.print("Peso no pote: ");
  Serial.print(peso);
  Serial.println(" g");

  // Envia o peso via MQTT para Node-RED
  client.publish("/honeydrop/pote", String(peso).c_str());

  // Lógica do comedouro automático
  if (peso < PESO_MINIMO && !comedouroAberto) {
    Serial.println("Pouca ração! Liberando comida...");
    servoMotor.write(90);
    delay(5000);
    servoMotor.write(0);
    comedouroAberto = true;
  }

  if (peso >= PESO_MINIMO) {
    comedouroAberto = false;
  }

  delay(2000);
}
