/*
NodeMcu
BH1750
Connection:
 VCC-5v
 GND-GND
 SCL-SCL(analog pin 5)
 SDA-SDA(analog pin 4)
 ADD-NC or GND

*/

#include <Wire.h>
#include <BH1750.h>
#include <AuthClient.h>
#include <MicroGear.h>
#include <MQTTClient.h>
#include <SHA1.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>

const char* ssid     = "BAZ";
const char* password = "gearman1";
//const char* ssid     = "ZAB";
//const char* password = "Gearman1";

#define APPID       "PUDZAHydro"
#define GEARKEY     "CU0a1cfd4wTW8JI"
#define GEARSECRET  "unlyHeW8FW5RYY2lvwWuawGgY"
#define SCOPE       ""
#define ALIAS       "nodemcu"

WiFiClient client;
AuthClient *authclient;
MicroGear microgear(client);

BH1750 lightMeter;

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
  Serial.print("Incoming message --> ");
  Serial.print(topic);
  Serial.print(" : ");
  char strState[msglen];
  for (int i = 0; i < msglen; i++) {
    strState[i] = (char)msg[i];
    Serial.print((char)msg[i]);
  }
  Serial.println();
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
  microgear.setName("nodemcu");
  microgear.setAlias(ALIAS);
  microgear.subscribe("/nodemcu/lux");
}

void setup(){
  Serial.begin(9600);
  lightMeter.begin();
  
  microgear.on(MESSAGE,onMsghandler);
  microgear.on(CONNECTED,onConnected);

  if (WiFi.begin(ssid, password)) {

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    //uncomment the line below if you want to reset token -->
    microgear.resetToken();
    microgear.init(GEARKEY, GEARSECRET, SCOPE);
    microgear.connect(APPID);
  }
}

void loop() {
  if (microgear.connected()) {
    microgear.loop();
    Serial.println("connect...");
    uint16_t lux = lightMeter.readLightLevel();
    Serial.print("Light: ");Serial.print(lux);Serial.println(" lx");  
//    microgear.chat("htmlgear", String(lux));
    microgear.publish ("/nodemcu/lux", String(lux));
  } else {
    Serial.println("connection lost, reconnect...");
    microgear.connect(APPID);
  }
  delay(1000);
}
