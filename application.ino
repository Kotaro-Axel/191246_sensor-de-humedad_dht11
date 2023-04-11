#include <WiFi.h>
#include <WebServer.h>
#include "DHT.h"

const char* ssid = "Corridos belicos";
const char* password = "PRCpesopluma";

DHT dht(5, DHT11);

WebServer server(80);

int LED_BUILTIN  = 2  ;
int ENA = 23;
int IN1 = 21;
int IN2 = 19;

const int frequency = 500;
const int pwm_channel = 0;
const int resolution = 8;

void setup() {

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT); 
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  ledcSetup(pwm_channel, frequency, resolution);
  ledcAttachPin(ENA, pwm_channel);

  Serial.begin(115200);
  dht.begin();
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);

  server.begin();
  Serial.println("Server started");

  pinMode(LED_BUILTIN, OUTPUT);
}

void handleRoot() {
  
  float humidity = dht.readHumidity();
  float humidityp = 50.00;
  float humidityp2 = 70.00;
  String html = "<html><body><h1>ESP32 DHT11 Sensor</h1>";

  const float initial_humidity = humidity;
  Serial.println(initial_humidity);
  const float condicion1 = initial_humidity + (initial_humidity * 0.1);
  const float condicion2 = initial_humidity + (initial_humidity * 0.2);

  float new_humidity = dht.readHumidity();

  if (new_humidity >= condicion1 || new_humidity<condicion2) {

    float temperature = dht.readTemperature();
    html += "<p>Humidity: ";
    html += humidity;

    static unsigned long previousMillis = 0;
    const long interval = 1000;
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);

  }else if (new_humidity >= condicion2) {
    float temperature = dht.readTemperature();
    html += "<p>Humidity: ";
    html += humidity;
    html += " %</p><p>Temperature: ";
    html += temperature;
    html += " &deg;C</p>";
    delay(1000);
    setDirection();
    delay(1000);
    changeSpeed();
    delay(1000); 
  }else {
    html += "<p>Humidity is too low</p>";
    digitalWrite(LED_BUILTIN, LOW);
  }

  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setDirection() {

  ledcWrite(pwm_channel, 255);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  delay(5000);
  
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  delay(5000);
  
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}

void changeSpeed() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  
  for (int i = 0; i < 256; i++) {;
  ledcWrite(pwm_channel,i);
    delay(20);
  }
  
  for (int i = 255; i >= 0; --i) {
  ledcWrite(pwm_channel,i);
    delay(20);
  }
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}

void loop() {
  server.handleClient();
}


