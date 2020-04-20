* Using Heltec LoRa32 V2 (ESP32) board

#Functionalities :
* BIdirectional network
* msgID syncronization
* timesync between nodes in network
* Check imsg is for that slave node or not
* Getting data from GNSS coordinates from NEOM8N module by uart communication on ESP32
* Getting temperature data from MAX6675 sensor by SPI interface

#Flow:
*  Default in listen mode
*  When receives data packet from master it actuates on that
*  send telemetry packet in reply to master node
* typedef struct struct_message {   
  int nodeID;
  int RXnodeID;
  byte msgType;
  int msgID;
  byte iCommType;
  unsigned long timeStamp;
  float tempAir;
  float latitudeGPS;
  float longitudeGPS;
  bool ledStatus;
} struct_message;
