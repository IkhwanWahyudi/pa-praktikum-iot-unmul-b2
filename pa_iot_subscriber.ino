/*
 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
const char* ssid = "iPhone";
const char* password = "arman123";
const char* mqtt_server = "broker.emqx.io";  // broker gratisan
int value_sementara;

// Buzzer & LED pin
#define LEDdanger D1
#define LEDwarning D2
#define Buzzer D3
bool on = true;
int btnONOFF = 0;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

void setup() {
  // We start by connecting to a WiFi network
  pinMode(Buzzer, OUTPUT);  // Inisialisasi pin buzzer
  pinMode(LEDwarning, OUTPUT);
  pinMode(LEDdanger, OUTPUT);  // Inisialisasi pin LED
  Serial.begin(115200);
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, 1883);  // setup awal ke server mqtt
  client.setCallback(callback);
}

// Fungsi untuk menerima data
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Pesan diterima [");
  Serial.print(topic);
  Serial.print("] ");
  String data = "";  // variabel untuk menyimpan data yang berbentuk array char
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    data += (char)payload[i];  // menyimpan kumpulan char kedalam string
  }
  Serial.println("");
  int value = data.toInt();
  if (strcmp(topic, "PA/Power") == 0) {
    if (data == "ON") {
      Serial.println("Indikator nyala");
      on = true;
    } else if (data == "OFF") {
      Serial.println("Indikator mati");
      on = false;
    }
  }
  if (strcmp(topic, "PA/KetinggianAir") == 0) {
    if (on) {
      if (value > 450 && value < 500 ) {
        digitalWrite(LEDwarning, HIGH);
        digitalWrite(LEDdanger, LOW);
        noTone(Buzzer);
      } else if (value >= 500) {
        digitalWrite(LEDwarning, LOW);
        digitalWrite(LEDdanger, HIGH);
        tone(Buzzer, 100);
      } else {
        noTone(Buzzer);
        digitalWrite(LEDdanger, LOW);
        digitalWrite(LEDwarning, LOW);
      }
    } else {
      Serial.println("Komponen mati");
      noTone(Buzzer);
      digitalWrite(LEDdanger, LOW);
      digitalWrite(LEDwarning, LOW);
    }
  }
}


// fungsi untuk mengubungkan ke broker
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "mqttx_arman";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.subscribe("PA/KetinggianAir");
      client.subscribe("PA/Power");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}