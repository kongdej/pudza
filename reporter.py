import microgear.client as microgear
import time
import requests

appid = "PUDZAHydro"
gearkey = "vG29a6eadW4O7RY"
gearsecret =  "EIQAyK2ti42tS3iMfQicqZx8n"

# IFTTT
event = "pudza_report"
url = "https://maker.ifttt.com/trigger/%s/with/key/dJaghatn9VW74F57cTVwdj" % event

# Thingspeak
urlThingspeak = "https://api.thingspeak.com/update.json"
api_key = "MHTC88ZRMETT82P9"

lux=""
temp=""
amptemp="0"
amphum=""
wtrtemp=""
soilhum=""
rain=""
flow=""
ec=""

microgear.create(gearkey,gearsecret,appid,{'debugmode': True})

def connection():
  print "Now I am connected with netpie"
  payload = {'value1': 'Now Pudza BOT is connected.'}
  r = requests.post(url, data=payload,verify=False)

def subscription(topic,message):
  global amptemp,amphum,wtrtemp,soilhum,rain,flow,ec

  print topic+" "+message
  if topic == "/PUDZAHydro/nodemcu/lux" :
    lux = message
  if topic == "/PUDZAHydro/nodemcu/temp" :
    temp = message
  y,m,d,h,mi,s,wd,wy,isd=time.gmtime()  
#  if s%15 == 0:
  if topic == "/uno/amptemp" :
    amptemp = message
  if topic == "/uno/amphum" :
    amphum = message
  if topic == "/uno/wtrtemp" :
    wtrtemp = message
  if topic == "/uno/soilhum" :
    soilhum = message
  if topic == "/uno/rain" :
    rain = message
  if topic == "/uno/flow" :
    flow = message
  if topic == "/uno/ec" :
    ec = message

  if s % 15 == 0:
    payload = {'api_key': api_key, 'field1' : amptemp,'field2' : amphum,'field3' : str(wtrtemp),'field4' : str(soilhum),'field5' : str(rain),'field6' : str(flow),'field7' : str(ec)}
    r = requests.post(urlThingspeak,params=payload,verify=False)
    print r.text

  
#  if mi == 9 and s == 0:
#    print "@%s send to line" % mi
#    payload = {'value1': 'Light = '+ lux + ' lux<br>'+'Temp = '+ temp + ' C'}
#    r = requests.post(url, data=payload)

def disconnect():
  print "disconnect is work"

microgear.setalias("reporter")
microgear.on_connect = connection
microgear.on_message = subscription
microgear.on_disconnect = disconnect
microgear.subscribe("/nodemcu/lux")
microgear.subscribe("/nodemcu/temp")
microgear.subscribe("/uno/amptemp");
microgear.subscribe("/uno/amphum");
microgear.subscribe("/uno/wtrtemp");
microgear.subscribe("/uno/flow");
microgear.subscribe("/uno/soilhum");
microgear.subscribe("/uno/rain");
microgear.subscribe("/uno/ec");
microgear.connect(True)


  
