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
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 0
// parameter for temperture sensor
OneWire oneWire(ONE_WIRE_BUS);// Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire);// Pass our oneWire reference to Dallas Temperature.

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

int relayPin[4] = {14,12,13,15};
//const int rainPin = A0;

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

  String stateStr = String(strState).substring(0,msglen);
  if (String(topic) == "/PUDZAHydro/mist"){
    if (stateStr == "ON") {
      digitalWrite(relayPin[0],HIGH);
      Serial.print("ON");
    }
    else if (stateStr == "OFF") {
      digitalWrite(relayPin[0],LOW);
      Serial.print("OFF");
    }
  }
 
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
  microgear.setName("nodemcu");
  microgear.setAlias(ALIAS);
  microgear.subscribe("/nodemcu");
//  microgear.subscribe("/nodemcu/lux");
//  microgear.subscribe("/nodemcu/temp");
  microgear.subscribe("/mist");
}

void setup(){
  Serial.begin(9600);
  lightMeter.begin();
  sensors.begin();
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

  
  pinMode(relayPin[0],OUTPUT);
}

void loop() {
  if (microgear.connected()) {
    microgear.loop();                                                                                                                                                               
    uint16_t lux = lightMeter.readLightLevel();
//    float rain = (1024-analogRead(rainPin))/1024.0*100;
//    microgear.publish ("/nodemcu/lux", String(lux));
    sensors.requestTemperatures();
    float temp = sensors.getTempCByIndex(0); 
    String msg = String(lux) + ',' + String(temp);
    microgear.publish ("/nodemcu", msg);
//    Serial.println(temp);
    //delay(2000);
    //microgear.publish ("/nodemcu/temp", String(temp));



//    microgear.publish ("/nodemcu/rain", String(rain));
//    Serial.println("connect...");
//    Serial.print("Light: ");Serial.print(lux);Serial.println(" lx");  
//    microgear.chat("htmlgear", String(lux));
  } else {
    Serial.println("connection lost, reconnect...");
    microgear.connect(APPID);
  }
  delay(1000);
}
