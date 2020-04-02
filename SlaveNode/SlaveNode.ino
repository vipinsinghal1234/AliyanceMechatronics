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

byte localAddress = 0xEE  ;     // address of this device
byte destination = 0xAA   ;      // destination to send to

int msgCount = 0;

byte COMTYPE_LoRa = 0x01;
byte COMTYPE_WiFi = 0x02;
byte COMTYPE_LTE = 0x03;
byte COMTYPE_ZigBee = 0x04;

typedef struct struct_message {     //Structure example to send telemtryPacket    //Must match the receiver structure
  int nodeID;
  int msgID;
  byte iCommType;
  unsigned long timeStamp;
  float tempAir;
  float latitudeGPS;
  float longitudeGPS;
} struct_message;

struct_message telemtryPacket;        // Create a struct_message
//////////////////////////////////////////
#include <TinyGPS++.h>

HardwareSerial ss(2);
bool data= false;

double lat0 = 18.431590;
double lon0 =73.809572;
const float pi = 3.14159; // To however many digits you want.
int c=1;
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
  Serial.begin(115200); 

  ss.begin(9600, SERIAL_8N1, 16, 17);    // int8_t rxPin, int8_t txPin
  
  Serial.println("setup");
  
}

void loop() {
Sensor_Reading();
send(); 
delay(2000);
}

void send()
{
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(255);        // add payload length
  LoRa.write((uint8_t*)&telemtryPacket, sizeof(telemtryPacket));
  LoRa.endPacket(); // finish packet and send it
  Serial.println("packet sent:");
  Serial.println(msgCount);
   
}

void Sensor_Reading()
{
  //TinyGPSPlus gps = GPS();
  float tempAir = tempMAX6678();

  ////////////////////////////////////
//  int randNumber = random(1, 5);
//  float tempAir = 32+(0.25*randNumber);
  //////////////////////////////////////////
  
  //////////////////////////////////////////////////
  double lat1 = lat0 + (180/pi)*(50*(sin(c/pi))/6378137);
  double lon1 = lon0 + (180/pi)*(50*(cos(c/pi)/6378137))/cos(lat0);
  Serial.println(float(lat1),6);
  Serial.println(float(lon1),6);
  Serial.println();
  c++;
//  /////////////////////////////////////////////////////
  
  telemtryPacket.nodeID = 8;
  telemtryPacket.msgID = msgCount;
  telemtryPacket.iCommType = COMTYPE_LoRa;
  telemtryPacket.timeStamp = millis();
  telemtryPacket.tempAir = tempAir;
//  telemtryPacket.latitudeGPS = (gps.location.lat());
//  telemtryPacket.longitudeGPS = (gps.location.lng());
  
  ///////////////////////////////////////
  telemtryPacket.latitudeGPS = (lat1);
  telemtryPacket.longitudeGPS = (lon1);
  /////////////////////////////////////

  msgCount++; 

}

TinyGPSPlus GPS(){
  TinyGPSPlus gps;
  while (ss.available()>0){
    gps.encode(ss.read());
    }

  if (gps.location.isValid()){
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
  Serial.println(thermocouple.readCelsius());
  return(thermocouple.readCelsius());
}
