import requests
#import urllib2

api_key = "MHTC88ZRMETT82P9"
#urlThingspeak = "https://api.thingspeak.comupdate?api_key=%s" % api_key 
#print urlThingspeak
#lux = "10"
#params = urllib.urlencode({'key': keyThingspeak})
#f = urllib.urlopen(urlThingspeak+"update?field1="+msg, data=params)

#f = urllib2.urlopen(urlThingspeak + "&field1=%s" % (lux))
#print f.read()
x=10
payload = {'api_key': api_key, 'field1' : str(x)}
print "X is: "+str(x)
r = requests.post("https://api.thingspeak.com/update.json",params=payload)
print r.text