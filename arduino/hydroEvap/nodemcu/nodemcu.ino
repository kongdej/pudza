#include <AuthClient.h>
#include <MicroGear.h>
#include <MQTTClient.h>
#include <SHA1.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "DHT.h"
#include "ThingSpeak.h"

#define DHTPIN 2  
#define DHTTYPE DHT11   // DHT 11,22,21
DHT dht(DHTPIN, DHTTYPE);

//const char* ssid     = "BAZ";
//const char* password = "gearman1";
const char* ssid     = "ZAB";
const char* password = "Gearman1";

#define APPID       "HTLog"
#define GEARKEY     "SmGmzQkXgnl7aeg"
#define GEARSECRET  "Y3dLjAo8QJl7icpIe7ydmvRCu"
#define SCOPE       ""
#define ALIAS       "nodeMcu"


unsigned long myChannelNumber = 156939;
const char * myWriteAPIKey = "ZTX7R3Q0IBPBQYT3";


WiFiClient client;
AuthClient *authclient;
MicroGear microgear(client);


void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
  if (String(topic) == "/PUDZAHydro/mist"){
  } 
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
  microgear.setName("nodemcu");
  microgear.setAlias(ALIAS);
  microgear.subscribe("/nodemcu/temp");
}


void setup(){
  Serial.begin(9600);
  
  ThingSpeak.begin(client);
  
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
  dht.begin();
}

void loop() {
  delay(2000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Serial.println(t);
  Serial.println(h);  
  ThingSpeak.setField(1,t);
  ThingSpeak.setField(2,h);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
   
  if (microgear.connected()) {
    microgear.loop();                                                                                                                                                               
    microgear.publish ("/nodemcu/temp", String(t));
    microgear.publish ("/nodemcu/hum", String(h));
          
  }else {
    Serial.println("connection lost, reconnect...");
    microgear.connect(APPID);
    delay(3000);
  }
  
}
