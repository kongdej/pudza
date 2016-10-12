#include <Wire.h> 
#include <OneWire.h>
#include <DallasTemperature.h>
#include <avr/eeprom.h>

#include "DHT.h"
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

// GPIO Pin define
byte sensorInterrupt = 0;  // 0 = digital pin 2
byte sensorPin       = 2;
#define DHTPIN 3 
#define ONE_WIRE_BUS 4
int soil_moisturePin = A0;
int rainPin = A1;

// parameter for temperture sensor
OneWire oneWire(ONE_WIRE_BUS);// Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire);// Pass our oneWire reference to Dallas Temperature.

// paramenter for flow sensor
float calibrationFactor = 4.5;
volatile byte pulseCount;  
float flowRate;
unsigned long oldTime;

// parameter for dht21
DHT dht(DHTPIN, DHTTYPE);
//** Parameters for EC sensor **//
float K = 1.66;    // default K for calibreation
int R1 = 670;     // <*************resistance 
float Ra = 0.5;    //Resistance of powering Pins
int ECPin = A4;   // Signal pin
int ECGround = A5; // Ground pin
int ECPower = A3; // Power pin
int ppm =0;
float PPMconversion=0.7;
float TemperatureCoef = 0.019; //this changes depending on what chemical we are measuring
float Temperature=10;
float EC=0;
float EC25 =0;
float raw= 0;
float Vin= 5;
float Vdrop= 0;
float Rc= 0;
float buffer=0;

// EEPROM structur
struct settings_t {
  float K;
} settings;

//** EC Calibration **//
float CalibrationEC=2.5;   // default EC for calibration
float TemperatureStart=0;
float TemperatureFinish=0;
//**//

void setup() {
  Serial.begin(9600);

  sensors.begin();
  dht.begin();
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);

  // initial flow sensor
  pulseCount        = 0;
  flowRate          = 0.0;
  oldTime           = 0;
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);

  // EC setup
  pinMode(ECPin,INPUT);
  pinMode(ECPower,OUTPUT);                //Setting pin for sourcing current
  pinMode(ECGround,OUTPUT);               //setting pin for sinking current
  digitalWrite(ECGround,LOW);             //We can leave the ground connected permanantly

  // load data from EEPROM
  eeprom_read_block((void*)&settings, (void*)0, sizeof(settings));   // read eeprom 
  K=settings.K;
}

void loop()
{
  delay(2000);
  
  sensors.requestTemperatures();
  float t_water = sensors.getTempCByIndex(0); 
  
  float h_air = dht.readHumidity();
  float t_air = dht.readTemperature();

  if (isnan(h_air) || isnan(t_air)) {
    //Serial.println("Failed to read from DHT sensor!");
    t_air = -1;
  }

  float soil_moisture = (1024-analogRead(soil_moisturePin))/1024.0*100.0;
  float rain = (1024-analogRead(rainPin))/1024.0*100.0;

  // send data to raspbery pi
  Serial.print(t_air);
  Serial.print(',');
  Serial.print(h_air);
  Serial.print(',');
  Serial.print(t_water);
  Serial.print(',');
  Serial.print(soil_moisture);
  Serial.print(',');
  Serial.print(rain);
  Serial.print(',');

  // flow sensor
  if((millis() - oldTime) > 1000)    // Only process counters once per second
   {
     detachInterrupt(sensorInterrupt);
     flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
     oldTime = millis();
     unsigned int frac;
     Serial.print(int(flowRate));  // Print the integer part of the variable
     Serial.print(".");             // Print the decimal point
     frac = (flowRate - int(flowRate)) * 10;
     Serial.print(frac, DEC) ;      // Print the fractional part of the variable
     pulseCount = 0;
     attachInterrupt(sensorInterrupt, pulseCounter, FALLING);        
  }

  // EC sensor
  GetEC();                      // Calls Code to Go into GetEC() 
  Serial.print(',');
  Serial.println(EC25);
  
  if (Serial.available())
  {
     String message = Serial.readString();
     String cmd = message.substring(0,1);
     String val = message.substring(1);
     if (cmd == "1") { // EC Calibration
        CalibrationEC = val.toFloat();
        doCalibration();
/*        Serial.print(message);
        Serial.print(" <=> ");
        Serial.println(EC);
        delay(1000);
        Serial.println("text1...");
        delay(1000);
        Serial.println("text2...");
        delay(1000);
        Serial.println("text3...");
*/
      }     
  }
}

void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}

void GetEC(){
  //*********Reading Temperature Of Solution *******************//
  sensors.requestTemperatures();// Send the command to get temperatures
  Temperature=sensors.getTempCByIndex(0); //Stores Value in Variable
   
  //************Estimates Resistance of Liquid ****************//
  digitalWrite(ECPower,HIGH);
  raw= analogRead(ECPin);
  raw= analogRead(ECPin);// This is not a mistake, First reading will be low beause if charged a capacitor
  digitalWrite(ECPower,LOW);
   
  //***************** Converts to EC **************************//
  Vdrop= (Vin*raw)/1024.0;
  Rc=(Vdrop*R1)/(Vin-Vdrop);
  Rc=Rc-Ra; //acounting for Digital Pin Resitance
  EC = 1000/(Rc*K);
   
  //*************Compensating For Temperaure********************//
  EC25  =  EC/ (1+ TemperatureCoef*(Temperature-25.0));
  ppm=(EC25)*(PPMconversion*1000);
}
//************************** End OF EC Function ***************************//

//***===Function: do Calibraion ===============================//
void doCalibration() {
  Serial.print("Cal EC=");
  Serial.println(CalibrationEC);
  while (1) {
      int i=1;
      buffer=0;
   
      sensors.requestTemperatures();                        // Send the command to get temperatures
      TemperatureStart=sensors.getTempCByIndex(0);          //Stores Value in Variable
      Serial.print("Temp Start = ");
      Serial.println(TemperatureStart);
      delay(1000);
      //******Estimates Resistance of Liquid ***************//
      while(i<=10){
        digitalWrite(ECPower,HIGH);
        raw= analogRead(ECPin);
        raw= analogRead(ECPin);                              // This is not a mistake, First reading will be low
        digitalWrite(ECPower,LOW);
        buffer=buffer+raw;
        Serial.print("Raw ");Serial.print(i);
        Serial.print(" = ");Serial.println(raw);        
        i++;
        delay(5000);
      }
      
      raw=(buffer/10);                              // cal average raw
      sensors.requestTemperatures();                // get finish temperatures
      TemperatureFinish=sensors.getTempCByIndex(0); //Stores Value in Variable
      Serial.print("Temp Finish=");Serial.println(TemperatureFinish);
 
      //*************Compensating For Temperaure********************//
      EC =CalibrationEC*(1+(TemperatureCoef*(TemperatureFinish-25.0))) ;
       
      //***************** Calculates R relating to Calibration fluid **************************//
      Vdrop= (((Vin)*(raw))/1024.0);
      Rc=(Vdrop*R1)/(Vin-Vdrop);
      Rc=Rc-Ra;
      K= 1000/(Rc*EC);
      if (TemperatureStart == TemperatureFinish){
        Serial.print("Result is OK -->");
        Serial.print(" K = ");Serial.println(K); 
        settings.K=K;
        eeprom_write_block((const void*)&settings, (void*)0, sizeof(settings)); //save all configuration to eeprom
        delay(1000);
        Serial.println("Save to EEPROM.");
        return;
      }
      else{
        delay(2000);
        Serial.println("Err-Temp not settle, calibrate again.");
      }
  }
}
