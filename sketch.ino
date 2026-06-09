#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>          
#include <Wire.h>         
#include <Adafruit_BMP085.h> 

// ============ CONFIGURAÇÕES ============
const char* ssid = "Wokwi-GUEST"; 
const char* password = "";
const char* mqtt_server = "broker.emqx.io"; 
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// Definição dos Pinos
#define DHT_PIN 5
#define DHT_TYPE DHT22   
#define POT_PIN 34
#define LED_ALERTA_PIN 2  

// Inicialização dos Sensores
DHT dht(DHT_PIN, DHT_TYPE);
Adafruit_BMP085 bmp;

// Variáveis Globais de Telemetria
float temp_celsius = 25.0;
float pressure_kpa = 101.3;
float velocity_kmh = 7800.0;
unsigned long lastMsg = 0;
bool anomaliaDetectada = false;

void setup() {
  Serial.begin(115200);
  delay(100);
  
  pinMode(POT_PIN, INPUT);
  pinMode(LED_ALERTA_PIN, OUTPUT); 
  
  Serial.println("\n\n=== DRAGON TELEMETRY SYSTEM ===");
  
  // Inicializa o DHT22
  dht.begin();
  
  // Inicializa o BMP180 no barramento I2C padrão
  if (!bmp.begin()) {
    Serial.println("⚠ Sensor BMP180 não respondeu. Certifique-se de ligar o pino GND!");
  } else {
    Serial.println("✓ Sensor BMP180 conectado com sucesso.");
  }
  
  connectWiFi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  unsigned long now = millis();
  
  // LÓGICA DO LED PISCANTE (Independente do tempo do MQTT)
  if (anomaliaDetectada) {
    digitalWrite(LED_ALERTA_PIN, (now / 200) % 2); 
  } else {
    digitalWrite(LED_ALERTA_PIN, LOW); 
  }

  // Leitura e envio a cada 2 segundos
  if (now - lastMsg > 2000) {
    lastMsg = now;
    
    readSensors();
    checkAnomalies(); 
    publishTelemetry();
  }
}

void connectWiFi() {
  Serial.print("Conectando ao WiFi: ");
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✓ WiFi conectado!");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT... ");
    String clientId = "DragonCapsuleClient-";
    clientId += String(random(0, 5000));
    
    if (client.connect(clientId.c_str())) {
      Serial.println("✓ Conectado!");
    } else {
      delay(5000);
    }
  }
}

void readSensors() {
  // 1. Lendo a temperatura real do DHT22
  float t = dht.readTemperature();
  if (!isnan(t)) {
    temp_celsius = t;
  }
  
  // 2. Lendo a pressão real do BMP180 (Convertendo Pa para kPa)
  float p = bmp.readPressure();
  if (p > 0) {
    pressure_kpa = p / 1000.0; 
  }
  
  // 3. Lendo o Potenciômetro Real
  int potValue = analogRead(POT_PIN);
  velocity_kmh = map(potValue, 0, 4095, 0, 29000); 
}

void checkAnomalies() {
  
  if (temp_celsius >= 80.0 || pressure_kpa < 90.0 || velocity_kmh > 8000) {
    anomaliaDetectada = true;
  } else {
    anomaliaDetectada = false;
  }
}

void publishTelemetry() {
  String payload = "{";
  payload += "\"temperature\":" + String(temp_celsius, 2) + ",";
  payload += "\"pressure\":" + String(pressure_kpa, 2) + ",";
  payload += "\"velocity\":" + String(velocity_kmh, 0) + ",";
  payload += "\"status\":\"" + String(anomaliaDetectada ? "CRITICAL" : "OK") + "\","; 
  payload += "\"timestamp\":" + String(millis());
  payload += "}";
  
  client.publish("dragon/telemetry/full", payload.c_str());
  
  Serial.print("📡 Transmissão -> Temp: "); Serial.print(temp_celsius, 1);
  Serial.print("°C | Pres: "); Serial.print(pressure_kpa, 1);
  Serial.print("kPa | Vel: "); Serial.print(velocity_kmh, 0);
  Serial.println(anomaliaDetectada ? " KM/H [🔴 ALERTA DE CABINE]" : " KM/H [🟢 OPERAÇÃO NOMINAL]");
}
