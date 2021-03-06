#!/usr/bin/python

import microgear.client as microgear
import time
import requests

appid = "PUDZAHydro"
gearkey = "1N8TA8K6Nf1C63H"
gearsecret =  "4LrQ3MZQKCDEv2GnEbACZE72o"

# IFTTT
event = "pudza_report"
url = "https://maker.ifttt.com/trigger/%s/with/key/dsFC-2tGEmWDjeXYVEx5m6" % event

# Thingspeak
urlThingspeak = "https://api.thingspeak.com/update.json"
api_key = "MHTC88ZRMETT82P9"

lux="0"
temp="0"
amptemp="0"
amphum="0"
wtrtemp="0"
soilhum="0"
rain="0"
flow="0"
ec="0"
mistStatus="0"

microgear.create(gearkey,gearsecret,appid,{'debugmode': False})

def connection():
  print "Now I am connected with netpie"
  payload = {'value1': 'PUDZA BOT is connected.'}
  r = requests.post(url, data=payload,verify=False)

def subscription(topic,message):
  global amptemp,amphum,wtrtemp,soilhum,rain,flow,ec,lux,temp,mistStatus

#  print topic+" "+message

  if topic == "/PUDZAHydro/nodemcu" :
    lux,temp,mistStatus = message.split(",");
  if topic == "/PUDZAHydro/uno/amptemp" :
    amptemp = message
  if topic == "/PUDZAHydro/uno/amphum" :
    amphum = message
  if topic == "/PUDZAHydro/uno/wtrtemp" :
    wtrtemp = message
  if topic == "/PUDZAHydro/uno/soilhum" :
    soilhum = message
  if topic == "/PUDZAHydro/uno/rain" :
    rain = message
  if topic == "/PUDZAHydro/uno/flow" :
    flow = message
  if topic == "/PUDZAHydro/uno/ec" :
    ec = message
  if topic == "/PUDZAHydro/sptemp" :
    with open("sptemp.txt", "w") as f:
      f.write(message) 

  if topic == "/PUDZAHydro/tempauto" :
    print "get auto"
    with open("tempauto.txt", "w") as f:
      f.write(message) 

def disconnect():
  print "disconnect is work"

microgear.setalias("reporter")
microgear.on_connect = connection
microgear.on_message = subscription
microgear.on_disconnect = disconnect
microgear.subscribe("/nodemcu")
microgear.subscribe("/uno/amptemp");
microgear.subscribe("/uno/amphum");
microgear.subscribe("/uno/wtrtemp");
microgear.subscribe("/uno/flow");
microgear.subscribe("/uno/soilhum");
microgear.subscribe("/uno/rain");
microgear.subscribe("/uno/ec");
microgear.subscribe("/reporter");
microgear.subscribe("/sptemp");
microgear.subscribe("/sptempf");
microgear.subscribe("/tempauto");

microgear.connect(False)

alrain = 1
almistSta = 1
while True:
  y,m,d,h,mi,s,wd,wy,isd = time.localtime() 

  if s % 15 == 0:
    microgear.publish("/reporter",'O')
    payload = {'api_key': api_key, 'field1' : amptemp,'field2' : amphum,'field3' : str(wtrtemp),'field4' : str(soilhum),'field5' : str(rain),'field6' : str(flow),'field7' : str(ec),'field8' : str(lux)}
    r = requests.post(urlThingspeak,params=payload,verify=False)

#    print r.text

  if float(rain) > 40.0 and alrain == 1:
    v1 = 'Alert Rain!!!!'
    v2 = 'Time = '+str(d)+'-'+str(m)+'-'+str(y)+' @ '+str(h)+':'+str(mi)
    v3 = 'Rain  = '+ rain +' %<br>'    
    payload = {'value1': v1, 'value2': v2, 'value3': v3}
    r = requests.post(url, data=payload,verify=False)
#    print r.text
    alrain = 0

  if float(rain) < 30 and alrain == 0:
    v1 = 'Rain STOP!'
    v2 = 'Time = '+str(d)+'-'+str(m)+'-'+str(y)+' @ '+str(h)+':'+str(mi)
    v3 = 'Rain  = '+ rain +' %<br>'    
    payload = {'value1': v1, 'value2': v2, 'value3': v3}
    r = requests.post(url, data=payload,verify=False)
#    print r.text
    alrain = 1
  
  if mistStatus == '1' and almistSta == 1:
    v1 = 'Mist Pump Start'
    v2 = 'Time = '+str(d)+'-'+str(m)+'-'+str(y)+' @ '+str(h)+':'+str(mi)
    v3 = 'Status  = '+ mistStatus +'<br>'    
    payload = {'value1': v1, 'value2': v2, 'value3': v3}
    r = requests.post(url, data=payload,verify=False)
    almistSta = 0

  if mistStatus == '0' and almistSta == 0:
    v1 = 'Mist Pump Stop'
    v2 = 'Time = '+str(d)+'-'+str(m)+'-'+str(y)+' @ '+str(h)+':'+str(mi)
    v3 = 'Status  = '+ mistStatus +'<br>'    
    payload = {'value1': v1, 'value2': v2, 'value3': v3}
    r = requests.post(url, data=payload,verify=False)
    almistSta = 1

    
  with open("sptemp.txt", "r") as f:
    sptemp = f.read() 

  with open("tempauto.txt", "r") as f:
    tempauto = f.read() 

  microgear.publish("/sptempf",sptemp)
    
  if float(amptemp) > 0  and float(wtrtemp) > 0 and float(temp) > 0 and int(tempauto):
    avgtemp =  (float(amptemp) + float(wtrtemp) + float(temp)) / 3.0
  
    
#    print "%f-%f" %(avgtemp,float(sptemp))

    if avgtemp >= float(sptemp) and mistStatus == "0":
      microgear.publish("/mist","1")
    elif avgtemp < float(sptemp) and mistStatus == "1":
      microgear.publish("/mist","0")



  
    


#  if s % 59 == 0:
  if mi == 0 and s == 0:
#    print "@%s send to line" % mi
    v1 = 'PUDZA Report'
    v2 = 'Time = '+str(d)+'-'+str(m)+'-'+str(y)+' @ '+str(h)+':'+str(mi)
    v3 = 'EC = '+ ec + ' mS/cm<br>'
    v3+= 'Flow = '+ flow +' L/min<br>'
    v3+= 'Humidity = '+ amphum  +' %<br>'
    v3+= 'WTR Temp = '+ wtrtemp +' C<br>'
    v3+= 'Tray Temp = '+ temp +' C<br>'
    v3+= 'Air Temp = '+ amptemp +' C<br>'
    v3+= 'Light = '   + lux +' lux<br>'
    v3+= 'Rain  = '+ rain +' %<br>'
    v3+= 'soil  = '+ soilhum +' %<br>'

    payload = {'value1': v1, 'value2': v2, 'value3': v3}
    r = requests.post(url, data=payload,verify=False)
#    print r.text
  time.sleep(1)
