#include <dht11.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define LED_PORT D1
#define DHT11_PORT D2
// Wi-Fi SSID
const char* ssid = "XxX";
// Wi-Fi PWD
const char* password = "A-b/cFgg~[123]cnm";
const char* mqtt_server = "broker.emqx.io";
const char* TOPIC = "wanghua/my/smart/home/control";
const char* client_id = "clientId-MySmartHome-002";
dht11 sensor_Dht11;
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
int tem, hum;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PORT, OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {
  delay(1000);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  digitalWrite(LED_PORT, !digitalRead(LED_PORT));
  /*if ((char)payload[0] == '1') {
    digitalWrite(LED_PORT, HIGH);
  } else {
    digitalWrite(LED_PORT, LOW);
  }*/
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(client_id)) {
      Serial.println("connected");
      client.subscribe(TOPIC);
    } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        delay(5000);
    }
  }
}

void loop() {
  delay(1000);
  sensor_Dht11.read(DHT11_PORT);
  tem = float(sensor_Dht11.temperature);
  hum = float(sensor_Dht11.humidity);
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    bool led_state = digitalRead(LED_PORT);
    String pub_content = "{\"device_id\":\"";
    pub_content += client_id;
    pub_content += "\",\"temperature\":\"";
    pub_content += tem;
    pub_content += "\",\"humidity\":\"";
    pub_content += hum;
    pub_content += "\",\"led_state\":\"";
    pub_content += led_state;
    pub_content += "\"}";
    client.publish("wanghua/my/smart/home", pub_content.c_str());
  }
}
