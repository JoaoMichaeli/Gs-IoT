#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

const int pinResiduo = 32;
const int pinChuva = 33;
const int pinAgua = 34;
const int pinAlerta = 25;

const int limiteResiduo = 2000;
const int limiteChuva = 2000;
const int limiteAgua = 2000;

void setup_wifi() {
  Serial.begin(115200);
  Serial.print("Conectando ao WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando MQTT...");
    if (client.connect("ESP32Client_Alagamento")) {
      Serial.println("Conectado!");
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5s...");
      delay(5000);
    }
  }
}

void setup() {
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

  bool alerta = (leituraResiduo > limiteResiduo || 
                leituraChuva > limiteChuva || 
                leituraAgua > limiteAgua);
  
  const char* risco;
  if (alerta) {
    risco = "ALTO";
    digitalWrite(pinAlerta, HIGH);
  } else {
    risco = "BAIXO";
    digitalWrite(pinAlerta, LOW);
  }

  DynamicJsonDocument doc(256);
  doc["residuo"] = leituraResiduo;
  doc["chuva"] = leituraChuva;
  doc["agua"] = leituraAgua;
  doc["risco"] = risco;

  char payload[256];
  size_t n = serializeJson(doc, payload);
  
  Serial.print("JSON enviado: ");
  Serial.println(payload);

  if (client.publish("alagamento/sensores", payload)) {
    Serial.println("Publicado com sucesso!");
  } else {
    Serial.println("Falha ao publicar!");
  }

  delay(10000);
}