#include <WiFi.h>
#include <PubSubClient.h>

// === Wi-Fi Wokwi Simulado ===
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// === Configuração do MQTT ===
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char* mqtt_topic = "enchente/nivel";

// === Pinos ===
#define TRIG 5
#define ECHO 18
#define LED 19
#define BUZZER 21

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.print("Ligando ao WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

// Mede distância em cm
long lerDistancia() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long duracao = pulseIn(ECHO, HIGH);
  long distancia = duracao * 0.034 / 2;
  return distancia;
}

void loop() {
  // Garante conexão MQTT
  if (!client.connected()) {
    while (!client.connect("ESP32Enchente")) {
      delay(1000);
    }
  }

  long nivel = lerDistancia();
  Serial.print("Nível da água: ");
  Serial.print(nivel);
  Serial.println(" cm");

  // Envia MQTT
  String mensagem = String(nivel);
  client.publish(mqtt_topic, mensagem.c_str());

  // Alerta se passar do limite de 350 cm
  if (nivel >= 350) {
    digitalWrite(LED, HIGH);
    digitalWrite(BUZZER, HIGH);
  } else {
    digitalWrite(LED, LOW);
    digitalWrite(BUZZER, LOW);
  }

  // Espera 10 segundos
  delay(10000);
}
