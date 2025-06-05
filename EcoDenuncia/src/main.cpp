#include <Arduino.h>

// Pinos dos sensores (potenciômetros)
const int pinResiduo = 32;     // Sensor de resíduo
const int pinChuva = 33;       // Sensor de chuva
const int pinNivelAgua = 34;   // Sensor de nível de água

// LED de alerta
const int pinLED = 25;

// Limiares (valores podem ser ajustados conforme testes)
const int limiteResiduo = 2000;   // valor do potenciômetro indicando presença de resíduo
const int limiteChuva = 2000;     // intensidade da chuva
const int limiteAgua = 2500;      // nível da água considerado crítico

void setup() {
  Serial.begin(115200);
  pinMode(pinLED, OUTPUT);
}

void loop() {
  int leituraResiduo = analogRead(pinResiduo);
  int leituraChuva = analogRead(pinChuva);
  int leituraAgua = analogRead(pinNivelAgua);

  Serial.println("----------");
  Serial.printf("Resíduo: %d\n", leituraResiduo);
  Serial.printf("Chuva: %d\n", leituraChuva);
  Serial.printf("Nível de Água: %d\n", leituraAgua);

  // Regra de risco: se houver resíduo + chuva forte + água subindo
  if (leituraResiduo > limiteResiduo && leituraChuva > limiteChuva && leituraAgua > limiteAgua) {
    digitalWrite(pinLED, HIGH); // alerta
    Serial.println("Risco de alagamento detectado!");
  } else {
    digitalWrite(pinLED, LOW);
  }

  delay(1000);
}
