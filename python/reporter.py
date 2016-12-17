import microgear.client as microgear
import time
import requests

appid = "PUDZAHydro"
gearkey = "vG29a6eadW4O7RY"
gearsecret =  "EIQAyK2ti42tS3iMfQicqZx8n"

event = "pudza_report"
url = "https://maker.ifttt.com/trigger/%s/with/key/dJaghatn9VW74F57cTVwdj" % event

urlThingspeak = "https://api.thingspeak.com/update.json"
api_key = "MHTC88ZRMETT82P9"

lux=""
temp=""

microgear.create(gearkey,gearsecret,appid,{'debugmode': True})

def connection():
  print "Now I am connected with netpie"
  payload = {'value1': 'Now Pudza BOT is connected.'}
  r = requests.post(url, data=payload, verify=False)

def subscription(topic,message):
  print topic+" "+message
  if topic == "/PUDZAHydro/nodemcu" :
    data = message.split(',');
    lux=data[0]
    temp=data[1]

  y,m,d,h,mi,s,wd,wy,isd=time.gmtime()  
  if s%15 == 0:
    payload = {'api_key': api_key, 'field1' : str(message)}
    r = requests.post(urlThingspeak,params=payload, verify=False)
    print r.text
  
  if mi == 56 and s == 0 :
    print "@%s send to line" % mi
    payload = {'value1': 'Report<br>Light = '+ lux + ' lux<br>'+'Temp = '+ temp + ' C'}
    r = requests.post(url, data=payload)

def disconnect():
  print "disconnect is work"

microgear.setalias("reporter")
microgear.on_connect = connection
microgear.on_message = subscription
microgear.on_disconnect = disconnect
microgear.subscribe("/nodemcu")
microgear.connect(True)


  
