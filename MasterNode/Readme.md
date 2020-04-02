* Using Heltec LoRa32 V2 (ESP32) board
* Receiving data packets on 443Mhz band on LoRa communication to master node
* Uploading whole data packets coming from slaves to sql server
* Right now network is unidirectional from slave to master
* Data packet has  structure of
`{
  int nodeID;
  int msgID;
  byte iCommType;
  unsigned long timeStamp;
  float tempAir;
  float latitudeGPS;
  float longitudeGPS;
  }
