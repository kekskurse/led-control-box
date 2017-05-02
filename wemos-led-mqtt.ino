
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include "RGBdriver.h"

const char* ssid = "SSID";
const char* password = "W-LanPassword";

char* server = "mqtt.handler.cloud";

String led_couch = "000000000";
String led_bed = "000000000";

RGBdriver Driver(D6,D5);


//Callback MSG
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String msg = "";
  for (int i=0;i<length;i++) {
    msg = msg + String((char)payload[i]);
  }
  String topicName = "";
  for (int i=0;i<length;i++) {
    topicName = topicName + String((char)payload[i]);
  }
  Serial.print(msg);
  Serial.println();
  if(String(topic)=="soeren/apartment/light/ledstripe/bed")
  {
    led_bed = msg;
  }
  else if(String(topic)=="soeren/apartment/light/ledstripe/couch")
  {
    led_couch = msg;
  }
  else{
    Serial.println("Unknown Topic");
  }
  doLEDS();
}


String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}

WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);


void connectWiFi(){
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


}
void connectMqqt(){
  Serial.println("Start MQTT Connection");
  if(WiFi.status() != WL_CONNECTED)
  {
    connectWiFi();
  }
  Serial.println("Generate MQTT Username");
  String clientName;
  clientName += "esp8266-";
  uint8_t mac[6];
  WiFi.macAddress(mac);
  clientName += macToStr(mac);
  clientName += "-";
  clientName += String(micros() & 0xff, 16);

  Serial.print("Connecting to ");
  Serial.print(server);
  Serial.print(" as ");
  Serial.println(clientName);

  if (client.connect((char*) clientName.c_str(), "MQTTUsername", "MQTTPassword")) {
    client.subscribe("soeren/apartment/light/ledstripe/bed");
    client.subscribe("soeren/apartment/light/ledstripe/couch");
    Serial.println("Connected to MQTT broker");
  }
  else {
    Serial.println("MQTT connect failed");
    Serial.println("Will reset and try again...");
    abort();
  }
}



void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.println();
  Serial.println("Start");
  connectMqqt();
}

void loop() {
  client.loop();
  if(WiFi.status() != WL_CONNECTED)
  {
    connectWiFi();
  }
  if(client.connected() != true)
  {
    connectMqqt();
  }
}

//LED Controller
void doLEDS()
{
  Driver.begin();
  Driver.SetColor(led_couch.substring(0,3).toInt(), led_couch.substring(3,6).toInt(), led_couch.substring(6,9).toInt());
  Driver.SetColor(led_bed.substring(0,3).toInt(), led_bed.substring(3,6).toInt(), led_bed.substring(6,9).toInt());
  Driver.end();
}
