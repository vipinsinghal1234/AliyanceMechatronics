#Functionalities : 
* Now network is bidirectional (Topology change- master node is sending packet which can be packet request or command packet 
   to one slave node at a time and in reply that slave is sending it's telemetry packet)
* msgID synchronization
* time sync between all nodes of network
* Route msg from server to slave if it has new msgID (check)

#Flow : 
*  Master node is  requesting data  from MySQL command table and check if is it new packet (With new msgID). If no then ignores.
*  If yes then sends that packet to slave node. Node Id is mentioned in packet . If no then master creates it's own request packet and 
   send it to slave
*  In reply master getting telemetry packet from slave and master will send it to MySQL database.
*  typedef struct struct_message { 
  int nodeID;
  int RXnodeID;
  byte msgType;
  int msgID;
  byte iCommType;
  unsigned long timeStamp;
  float tempAir;
  float latitudeGPS;
  float longitudeGPS;
  int ledStatus;
} struct_message;
