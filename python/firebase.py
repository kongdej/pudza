from firebase import firebase
firebase = firebase.FirebaseApplication('https://pudzahydro.firebaseio.com', None)
result = firebase.get('/users', None)
#print result