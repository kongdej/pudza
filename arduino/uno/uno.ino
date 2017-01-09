#include "DHT.h"
#define DHTTYPE DHT21   // DHT 22  (AM2302), AM2321
#define DHTPIN 3 
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  float soil_moisture[4] = {0,0,0,0};
  float volt=0.0;
  float h_air = dht.readHumidity();
  float t_air = dht.readTemperature();

  if (isnan(h_air) || isnan(t_air)) {
    //Serial.println("Failed to read from DHT sensor!");
    t_air = -1;
    h_air = -1;
  }

  volt = (analogRead(0)/1023.0)*25.0*0.993;
  Serial.print(volt);
  Serial.print(',');

  for(int i=1; i<5; i++) {
    soil_moisture[i-1] = (1023-analogRead(i))/1023.0*100.0;
    Serial.print(soil_moisture[i-1]);
    Serial.print(',');
  }
  Serial.print(t_air);
  Serial.print(',');
  Serial.println(h_air);

  delay(2000);
}
