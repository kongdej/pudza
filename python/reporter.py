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

microgear.create(gearkey,gearsecret,appid,{'debugmode': True})

def connection():
  print "Now I am connected with netpie"
  payload = {'value1': 'Now Pudza BOT is connected.'}
  r = requests.post(url, data=payload)

def subscription(topic,message):
  print topic+" "+message
  lux = message
  y,m,d,h,mi,s,wd,wy,isd=time.gmtime()  
  if mi == 12 and s == 0:
    payload = {'value1': 'Light = '+ lux + ' lux'}
    r = requests.post(url, data=payload)
  if s%15 == 0:
    payload = {'api_key': api_key, 'field1' : str(message)}
    r = requests.post(urlThingspeak,params=payload)
    print r.text

def disconnect():
  print "disconnect is work"

microgear.setalias("reporter")
microgear.on_connect = connection
microgear.on_message = subscription
microgear.on_disconnect = disconnect
microgear.subscribe("/nodemcu/lux")
microgear.connect(True)
