#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// Pinos dos sensores
const int pinResiduo = 32;
const int pinChuva = 33;
const int pinAgua = 34;
const int pinAlerta = 25;

// Limites
const int limiteResiduo = 2000;
const int limiteChuva = 2000;
const int limiteAgua = 2000;

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32Client_Alagamento")) {
      // conectado
    } else {
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(pinAlerta, OUTPUT);

  setup_wifi();
  client.setServer(mqttServer, mqttPort);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int leituraResiduo = analogRead(pinResiduo);
  int leituraChuva = analogRead(pinChuva);
  int leituraAgua = analogRead(pinAgua);

  bool alerta = (leituraResiduo > limiteResiduo && leituraChuva > limiteChuva && leituraAgua > limiteAgua);
  digitalWrite(pinAlerta, alerta ? HIGH : LOW);

  // JSON manual
  String payload = "{";
  payload += "\"residuo\":" + String(leituraResiduo) + ",";
  payload += "\"chuva\":" + String(leituraChuva) + ",";
  payload += "\"agua\":" + String(leituraAgua) + ",";
  payload += "\"risco\":" + String(alerta ? "true" : "false");
  payload += "}";

  client.publish("alagamento/sensores", payload.c_str());

  delay(2000);
}
