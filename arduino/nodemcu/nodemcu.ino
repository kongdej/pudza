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

float temp=0;

int relayPin[4] = {14,12,13,15};

unsigned long previousMillis = 0;     // will store last time LED was updated
const long interval = 2000;           // interval at which to blink (milliseconds)

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
/*
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
 if (String(topic) == "/PUDZAHydro/uno/amptemp") {
    atemp = stateStr.toFloat();
  }
  if (String(topic) == "/PUDZAHydro/uno/wtrtemp") {
    wtemp = stateStr.toFloat();    
  }
*/

  if (String(topic) == "/PUDZAHydro/mist"){
    if (msg[0] == '1') {
      digitalWrite(relayPin[0],HIGH);
//      Serial.print("ON");
    }
    else if (msg[0] == '0') {
      digitalWrite(relayPin[0],LOW);
//      Serial.print("OFF");
    }
  } 
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
  microgear.setName("nodemcu");
  microgear.setAlias(ALIAS);
  microgear.subscribe("/nodemcu");
  microgear.subscribe("/mist");
  microgear.subscribe("/mistsp");
//  microgear.subscribe("/uno/amptemp");
//  microgear.subscribe("/uno/wtrtemp");  
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
    //microgear.resetToken();
    microgear.init(GEARKEY, GEARSECRET, SCOPE);
    microgear.connect(APPID);
  }

  pinMode(relayPin[0],OUTPUT);
}

void loop() {
  if (microgear.connected()) {
    microgear.loop();                                                                                                                                                               
    uint16_t lux = lightMeter.readLightLevel();
    sensors.requestTemperatures();
    temp = sensors.getTempCByIndex(0); 
    int relayStatus = digitalRead(relayPin[0]);
    
    String msg = String(lux) + ',' + String(temp) + ',' + String(relayStatus);
    
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
       previousMillis = currentMillis;
       microgear.publish ("/nodemcu", msg);
    }    
  }else {
    Serial.println("connection lost, reconnect...");
    microgear.connect(APPID);
    delay(3000);
  }
}
