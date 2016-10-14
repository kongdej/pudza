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

microgear.create(gearkey,gearsecret,appid,{'debugmode': True})

def connection():
  print "Now I am connected with netpie"
  payload = {'Value1': 'text1..', 'Value2': 'text2..', 'Value3': 'text3..'}
  r = requests.post(url, data=payload)

def subscription(topic,message):
  print topic+" "+message
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