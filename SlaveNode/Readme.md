* Using Heltec LoRa32 V2 (ESP32) board
* Getting data from GNSS coordinates from NEOM8N module by uart communication on ESP32
* Getting temperature data from MAX6675 sensor by SPI interface
* sending data packets on 443Mhz band on LoRa communication to master node
* Right now network is unidirectional from slave to master
* Data packet has  structure of
{
  int nodeID;
  int msgID;
  byte iCommType;
  unsigned long timeStamp;
  float tempAir;
  float latitudeGPS;
  float longitudeGPS;
  }
