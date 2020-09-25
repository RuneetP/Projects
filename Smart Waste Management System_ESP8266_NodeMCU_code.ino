#include<ESP8266WiFi.h>
#include<PubSubClient.h>
#include<SoftwareSerial.h>
SoftwareSerial mySerial(D1, D2);
#define wifi_ssid "Shhh...I'm Batman!"
#define wifi_password "abracadabra#"
#define mqtt_server "192.168.0.106"
#define ultrasonic_topic "ultrasonic"
#define mqttpubQos 2
#define mqttsubQos 1
const int mqttPort = 1883;
const char* mqttUser = "roger";
const char* mqttPassword = "password";

String text;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, mqttPort);
  client.setCallback(callback);
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);
}

void setup_wifi() {
  delay(10);
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED)
    delay(500);
  Serial.print("Wifi connected \n");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("attempting MQTT connection");
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
      client.subscribe("BinOpen1");
      client.subscribe("BinOpen2");
      client.subscribe("bin/threshold/output/+");
      Serial.println("connected");
    }
    else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }

  }
}

void callback(char* topic, byte* payload, unsigned int length) {

  text = topic;


  if ( text == "BinOpen1") {
    Serial.write("BinOpen1");
    digitalWrite(0, LOW);
  }
  delay(2000);
  if ( text == "BinOpen2") {
    Serial.write("BinOpen2");
    digitalWrite(2, LOW);
  }
  delay(2000);
  if ( text == "bin/threshold/output/1") {
    Serial.write("bin/threshold/1");
    digitalWrite(BUILTIN_LED, HIGH);
    digitalWrite(0, LOW);
    delay(1000);
    digitalWrite(BUILTIN_LED, LOW);
    digitalWrite(0, HIGH);
    delay(1000);
  }
  if ( text == "bin/threshold/output/2") {
    Serial.write("bin/threshold/2");
    digitalWrite(BUILTIN_LED, HIGH);
    digitalWrite(2, LOW);
    delay(1000);
    digitalWrite(BUILTIN_LED, LOW);
    digitalWrite(2, HIGH);
    delay(1000);
  }

}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  String msg = mySerial.readStringUntil('\r');
  Serial.print(msg);
  if ((msg.indexOf("1=", 0) > -1) || (msg.indexOf("2=", 0) > -1) || (msg.indexOf("P1=", 0) > -1))
    client.publish(ultrasonic_topic, msg.c_str(), true);

}
