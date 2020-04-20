/* 
  Basic test program, send date at the BAND you seted.
  
  by Aaron.Lee from HelTec AutoMation, ChengDu, China
  成都惠利特自动化科技有限公司
  www.heltec.cn
  
  this project also realess in GitHub:
  https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series
*/
#include "heltec.h"
#define BAND    433E6  //you can set band here directly,e.g. 868E6,915E6
#define node  8

int msgCount = 1;
long lastSendTime = 0;        // last send time
int interval = 1000;          // interval between sends
long timeSyncErr = 0;         // come from master to sync time

byte COMTYPE_LoRa = 0x01;
byte COMTYPE_WiFi = 0x02;
byte COMTYPE_LTE = 0x03;
byte COMTYPE_ZigBee = 0x04;

byte msgType_CheckStatus = 0x01;
byte msgType_Commmand = 0x02;
byte msgType_Telemtry = 0x03;

typedef struct struct_message {     //Structure example to send telemtryPacket    //Must match the receiver structure
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

struct_message telemtryPacketTX;        // Create a struct_message
struct_message telemtryPacketRX;        // Create a struct_message


bool receiveflag = false;
//////////////////////////////////////////
#include <TinyGPS++.h>

HardwareSerial ss(2);

////////////////////////////////////////
#include "max6675.h"

int thermoDO = 12;
int thermoCS = 13;
int thermoCLK = 15;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
//////////////////////////////////////////////
void setup() {
  
  //WIFI Kit series V1 not support Vext control
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  LoRa.setTxPower(14,RF_PACONFIG_PASELECT_PABOOST);
  LoRa.onReceive(onReceive);
  LoRa.receive();
  Serial.begin(115200); 

  ss.begin(9600, SERIAL_8N1, 16, 17);    // int8_t rxPin, int8_t txPin

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN,1);
  
  Serial.println("setup");
  
}

void loop() {
  if(receiveflag){
      receiveflag= false;
      send();
  }
}

void send()
{
  Sensor_Reading(); 
  
  LoRa.beginPacket();                   // start packet
  LoRa.write((uint8_t*)&telemtryPacketTX, sizeof(telemtryPacketTX));
  LoRa.endPacket(); // finish packet and send it
  LoRa.receive();
  Serial.println("packet sent:");
  Serial.println(msgCount - 1);
  Serial.println();
}


void onReceive(int packetSize)//LoRa receiver interrupt service
{
   if (packetSize == 0) return;          // if there's no packet, return
 
  LoRa.readBytes((uint8_t *)&telemtryPacketRX, sizeof(telemtryPacketRX));
  timeSyncErr = millis() - telemtryPacketRX.timeStamp;
   
   if (telemtryPacketRX.RXnodeID != node) {
      Serial.print("This message is not for me RXnodeID :");
      Serial.println(telemtryPacketRX.RXnodeID);
      return;                             // skip rest of function
      }

   if (telemtryPacketRX.msgType == msgType_CheckStatus) {
      Serial.println("This message is request status.");
      receiveflag = true;
      return;                             // skip rest of function
      }

  Serial.println("Received packet :");
  Serial.println(telemtryPacketRX.nodeID);
  Serial.println(telemtryPacketRX.RXnodeID);
  Serial.print(telemtryPacketRX.msgType);
  Serial.println(telemtryPacketRX.msgID);
  Serial.println(telemtryPacketRX.iCommType);
  Serial.println(telemtryPacketRX.timeStamp);
  Serial.println(telemtryPacketRX.tempAir);
  Serial.println(((float)telemtryPacketRX.latitudeGPS),6);
  Serial.println(((float)telemtryPacketRX.longitudeGPS),6);
  Serial.println(telemtryPacketRX.ledStatus);
  Serial.println();
  receiveflag = true;  

  digitalWrite(LED_BUILTIN, !(telemtryPacketRX.ledStatus));
}

void Sensor_Reading()
{
  //TinyGPSPlus gps = GPS();
  //float tempAir = tempMAX6678();

  //////////////////////////////////////////
  int randNumber = random(1, 5);
  float tempAir = 32+(0.25*randNumber);
  //////////////////////////////////////////
  
  //////////////////////////////////////////////////
  static double lat0 = 18.431590;
  static double lon0 = 73.809572;
  const float pi = 3.14159; // To however many digits you want.
  static int c=1;
  double lat1 = lat0 + (180/pi)*(50*(sin(c/pi))/6378137);
  double lon1 = lon0 + (180/pi)*(50*(cos(c/pi)/6378137))/cos(lat0);
  Serial.print("Manual GNSS data :");
  Serial.print(float(lat1),6);
  Serial.print(F(","));
  Serial.println(float(lon1),6);
  c++;
////  /////////////////////////////////////////////////////
  
  telemtryPacketTX.nodeID = node;
  telemtryPacketTX.RXnodeID = 10;
  telemtryPacketTX.msgType = msgType_Telemtry;
  telemtryPacketTX.msgID = msgCount;
  telemtryPacketTX.iCommType = COMTYPE_LoRa;
  telemtryPacketTX.timeStamp = syncMillis();
  telemtryPacketTX.tempAir = tempAir;
//  telemtryPacketTX.latitudeGPS = (gps.location.lat());
//  telemtryPacketTX.longitudeGPS = (gps.location.lng());
  telemtryPacketTX.ledStatus = telemtryPacketRX.ledStatus;
  
  ///////////////////////////////////////
  telemtryPacketTX.latitudeGPS = (lat1);
  telemtryPacketTX.longitudeGPS = (lon1);
  /////////////////////////////////////

  msgCount++; 
 
}

TinyGPSPlus GPS(){
  TinyGPSPlus gps;
  while (ss.available()>0){
    gps.encode(ss.read());
    }

  if (gps.location.isValid()){
    Serial.print("NEOM8N Fetched data :");
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.println(gps.location.lng(), 6);
    }
  else {
    Serial.println(F("INVALID"));
    }
    
  return gps;
}

float tempMAX6678(){
  Serial.print("MAX7688 Fetched data :");
  Serial.println(thermocouple.readCelsius());
  return(thermocouple.readCelsius());
}

long syncMillis(){
  long syncMillis = millis() - timeSyncErr;
  return syncMillis;
}
