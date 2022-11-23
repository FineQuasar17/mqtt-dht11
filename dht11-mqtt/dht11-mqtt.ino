/*
  Basic ESP8266 / NodeMCU MQTT example using the pubsub client library
 In this example a DHT11 temperature/humidity sensor is connected to the nodeMCU/ESP8266 board
 The following measurement data will be sent to the following paths:
 temperature [°C] -> dht11/temp_celsius
 temperature [°K] -> dht11/temp_fahrenheit
 humidity [%] -> dht11/humidity

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#define DHT11PIN 4 //on the NodeMCU board it is the D2 PIN! 
//for more information about Nodemcu PINOUT
//https://rundebugrepeat.com/nodemcu/
#define DHTTYPE DHT11
DHT dht(DHT11PIN, DHTTYPE);

// Update these with values suitable for your network.

// WiFi
const char *ssid = "****"; // Enter your WiFi name
const char *password = "****";  // Enter WiFi password

// MQTT Broker
const char *mqtt_server = "****";
const char *mqtt_username = "****";
const char *mqtt_password = "****";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  char message[length];
  for (int i=0; i< length; i++){
    message[i] = (char)payload[i];
  }
  Serial.println(message);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Connected!");
     
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  delay(2000);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }else{   
    /*
     * Publish the measured data:
     * temperature [°C] -> dht11/temp_celsius
     * temperature [°K] -> dht11/temp_fahrenheit
     * humidity [%] -> dht11/humidity
     */
    client.publish("dht11/temp_1_celsius",String(t).c_str());
    client.publish("dht11/temp_1_fahrenheit",String(f).c_str());
    client.publish("dht11/_1_humidity",String(h).c_str());  
  }

}
