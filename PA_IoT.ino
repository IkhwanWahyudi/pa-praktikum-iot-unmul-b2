/*
 * This ESP8266 NodeMCU code was developed by newbiely.com
 *
 * This ESP8266 NodeMCU code is made available for public use without any restriction
 *
 * For comprehensive instructions and wiring diagrams, please visit:
 * https://newbiely.com/tutorials/esp8266/esp8266-water-sensor
 */
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define POWER_PIN D7
#define SIGNAL_PIN A0

WiFiClient espClient;
PubSubClient client(espClient);

// WiFi
const char *ssid = "vivo Y03";  // Enter your WiFi name
const char *password = "11234567";             // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "PA/KetinggianAir";
const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port = 1883;

int Value = 0;  // variable to store the sensor value

void setup() {
  Serial.begin(9600);
  pinMode(POWER_PIN, OUTPUT);    // Configure D7 pin as an OUTPUT
  digitalWrite(POWER_PIN, LOW);  // turn the sensor OFF

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to Wifi...");
  }

  // Connecting to an MQTT broker
  client.setServer(mqtt_broker, mqtt_port);
  // client.setCallback(callback);
  while (!client.connected()) {
    String client_id = "mqttx_ProjectAkhirIoT2024";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Public EMQX MQTT broker connected");
    } else {
      Serial.print("Failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void loop() {
  digitalWrite(POWER_PIN, HIGH);  // turn the sensor ON
  delay(10);                      // wait 10 milliseconds
  Value = analogRead(SIGNAL_PIN);   // read the analog value from sensor
  digitalWrite(POWER_PIN, LOW);   // turn the sensor OFF

  Serial.print("Ketinggian Air : ");
  Serial.println(Value);

  String sensorData = String(Value);

  client.publish(topic, sensorData.c_str());
  client.loop();
  delay(1000);
}
