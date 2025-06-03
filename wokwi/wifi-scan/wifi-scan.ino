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

unsigned long lastTime = 0;
const long interval = 10000;  // 10 segundos

void setup_wifi() {
  Serial.print("Ligando ao WiFi: ");
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
  return duracao * 0.034 / 2;
}

void loop() {
  if (!client.connected()) {
    while (!client.connect("ESP32Enchente")) {
      delay(1000);
    }
  }

  client.loop();  // mantém conexão MQTT

  unsigned long now = millis();
  if (now - lastTime >= interval) {
    lastTime = now;

    long nivel = lerDistancia();
    Serial.print("Nível da água: ");
    Serial.print(nivel);
    Serial.println(" cm");

    String mensagem = String(nivel);
    client.publish(mqtt_topic, mensagem.c_str());

    if (nivel >= 350) {
      digitalWrite(LED, HIGH);
      digitalWrite(BUZZER, HIGH);
    } else {
      digitalWrite(LED, LOW);
      digitalWrite(BUZZER, LOW);
    }
  }
}
