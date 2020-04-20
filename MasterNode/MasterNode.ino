/* 
  Check the new incoming messages, and print via serialin 115200 baud rate.
  
  by Aaron.Lee from HelTec AutoMation, ChengDu, China
  成都惠利特自动化科技有限公司
  www.heltec.cn
  
  this project also realess in GitHub:
  https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series
*/
#include <ArduinoJson.h>
#define node 10
int slaveNode[] = {8,9};
///////////////////////////////////////////////////////////
#include "esp_http_client.h"
#include <WiFi.h>
#include "Arduino.h"

const char* ssid = "Aliyance";
const char* password = "mech@1234";
const char *post_url = "http://192.168.0.103:5000/api/insertIntoTbl"; // Location to send POSTed data
const char *get_url = "http://192.168.0.103:5000/api/getDataFromOnOffTbl";

#include <HTTPClient.h>
///////////////////////////////////////////////////////////
#include "heltec.h"
#define BAND    433E6  //you can set band here directly,e.g. 868E6,915E6

int msgCount = 1;
long lastSendTime = 0;        // last send time
int interval = 1200;          // interval between sends

byte COMTYPE_LoRa = 0x01;
byte COMTYPE_WiFi = 0x02;
byte COMTYPE_LTE = 0x03;
byte COMTYPE_ZigBee = 0x04;

byte msgType_CheckStatus = 0x01;
byte msgType_Command = 0x02;
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
  int ledStatus;
} struct_message;

struct_message telemtryPacketRX;        // Create a struct_message
struct_message telemtryPacketTX;        // Create a struct_message

void setup() {
  Serial.begin(115200);
  
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  LoRa.setTxPower(14,RF_PACONFIG_PASELECT_PABOOST);
  
    if (init_wifi()) { // Connected to WiFi
    Serial.println("Internet connected");
  } 
  
  Serial.println("setup");    
}

void loop() {
      getDataFromServer();
      send();
      lastSendTime = millis();
      while (millis() - lastSendTime < interval){ 
        onreceive();
      }
}


void send()
{  
  LoRa.beginPacket();                   // start packet
  LoRa.write((uint8_t*)&telemtryPacketTX, sizeof(telemtryPacketTX));
  LoRa.endPacket();     // finish packet and send it
  LoRa.receive();
  Serial.print("packet no. sent to slave :");
  Serial.println(msgCount - 1);
  Serial.println();
}


void onreceive()//LoRa receiver interrupt service
{
   int packetSize = LoRa.parsePacket();
   if (packetSize == 0) return;          // if there's no packet, return

  LoRa.readBytes((uint8_t *)&telemtryPacketRX, sizeof(telemtryPacketRX));
    if (telemtryPacketRX.RXnodeID != node) {
      Serial.println("This message is not for me.");
      return;                             // skip rest of function
    }

  Serial.println("Received packet from slave :");
  Serial.print("nodeID :");
  Serial.print(telemtryPacketRX.nodeID);
  Serial.print("  RXnodeID :");
  Serial.print(telemtryPacketRX.RXnodeID);
  Serial.print("  msgType :");
  Serial.print(telemtryPacketRX.msgType);
  Serial.print("  msgID :");
  Serial.print(telemtryPacketRX.msgID);
  Serial.print("  iCommType :");
  Serial.print(telemtryPacketRX.iCommType);
  Serial.print("  timeStamp :");
  Serial.print(telemtryPacketRX.timeStamp);
  Serial.print("  tempAir :");
  Serial.print(telemtryPacketRX.tempAir);
  Serial.print("  latitudeGPS :");
  Serial.print(((float)telemtryPacketRX.latitudeGPS),6);
  Serial.print("  longitudeGPS :");
  Serial.print(((float)telemtryPacketRX.longitudeGPS),6);
  Serial.print("  ledStatus :");
  Serial.println(telemtryPacketRX.ledStatus);
  Serial.println();
  postDataToServer();
}


void pktBuild()
{
  static int i=0;
  telemtryPacketTX.nodeID = node;
  telemtryPacketTX.RXnodeID = slaveNode[i];//random(8,10);
  telemtryPacketTX.msgType = msgType_CheckStatus;
  telemtryPacketTX.msgID = msgCount;
  telemtryPacketTX.iCommType = COMTYPE_LoRa;
  telemtryPacketTX.timeStamp = millis();
  telemtryPacketTX.tempAir = 42.78;
  telemtryPacketTX.latitudeGPS = 19.785667;
  telemtryPacketTX.longitudeGPS = 20.674597;
  telemtryPacketTX.ledStatus = 1;
  msgCount++; 
  i++;
  if(i>=(sizeof(slaveNode)/sizeof(slaveNode[0]))){
    i=i-(sizeof(slaveNode)/sizeof(slaveNode[0]));
  }

}


bool init_wifi()
{
  int connAttempts = 0;
  Serial.println("\r\nConnecting to: " + String(ssid));
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
    if (connAttempts > 10) return false;
    connAttempts++;
  }
  return true;
}

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
  switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
      Serial.println("HTTP_EVENT_ERROR");
      break;
    case HTTP_EVENT_ON_CONNECTED:
      Serial.println("HTTP_EVENT_ON_CONNECTED");
      break;
    case HTTP_EVENT_HEADER_SENT:
      Serial.println("HTTP_EVENT_HEADER_SENT");
      break;
    case HTTP_EVENT_ON_HEADER:
      Serial.printf("HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
      break;
    case HTTP_EVENT_ON_DATA:
      Serial.println();
      Serial.printf("HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
      if (!esp_http_client_is_chunked_response(evt->client)) {
        // Write out data
        // printf("%.*s", evt->data_len, (char*)evt->data);
      }
      break;
    case HTTP_EVENT_ON_FINISH:
      Serial.println("");
      Serial.println("HTTP_EVENT_ON_FINISH");
      break;
    case HTTP_EVENT_DISCONNECTED:
      Serial.println("HTTP_EVENT_DISCONNECTED");
      break;
  }
  return ESP_OK;
}

static esp_err_t postDataToServer()
{
    String nodeIDString = "&nodeID="+String(telemtryPacketRX.nodeID);
    String GPSString = "&GPS="+String(((float)telemtryPacketRX.latitudeGPS),6) + "," + String(((float)telemtryPacketRX.longitudeGPS),6);
    String timeStampString = "&timeStamp="+String(telemtryPacketRX.timeStamp);
    String tempAirString = "&tempAir="+String(telemtryPacketRX.tempAir);
    String iCommTypeString = "&iCommType="+String(telemtryPacketRX.iCommType);
    String msgIDString = "&msgID="+String(telemtryPacketRX.msgID);
    String ledStatusString = "&ledStatus="+String(telemtryPacketRX.ledStatus);
    
    String finalStr;
    
    finalStr = nodeIDString + iCommTypeString + timeStampString + tempAirString + GPSString + msgIDString + ledStatusString;
    char *post_data;
    
   char *c = strcpy(new char[finalStr.length() + 1], finalStr.c_str());
   post_data = c;
   //delete[] c;
////////////////////////////////////////////////////////////////////////////////////////////


  esp_err_t res = ESP_OK;

  esp_http_client_handle_t http_client;
  
  esp_http_client_config_t config_client = {0};
  config_client.url = post_url;
  config_client.event_handler = _http_event_handler;
  config_client.method = HTTP_METHOD_POST;

  http_client = esp_http_client_init(&config_client);
  
  esp_http_client_set_post_field(http_client, post_data, strlen(post_data));
  
  esp_err_t err = esp_http_client_perform(http_client);
  if (err == ESP_OK) {
    Serial.print("esp_http_client_get_status_code: ");
    Serial.println(esp_http_client_get_status_code(http_client));
    Serial.println();
  }

  esp_http_client_cleanup(http_client);
}

void getDataFromServer(){
        static int serverMsgID = 0;
        static int lastServerMsgID = 0;
        
        HTTPClient http;
        String payload;
        
        Serial.print("[HTTP] begin...\n");        // configure traged server and url
        http.begin(get_url); //HTTP

        Serial.print("[HTTP] GET...\n");
        int httpCode = http.GET();      // start connection and send HTTP header

        if(httpCode > 0) {        // httpCode will be negative on error
           Serial.printf("[HTTP] GET... code: %d\n", httpCode);    // HTTP header has been send and Server response header has been handled
           if(httpCode == HTTP_CODE_OK) {            // file found at server
              const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(8) + 120;
              //DynamicJsonDocument doc(capacity);
              DynamicJsonBuffer jsonBuffer(capacity);
              JsonObject& root = jsonBuffer.parseObject(http.getString()); 

              unsigned long timeStamp = root["data"][0]["timeStamp"];
              int serverMsgID = root["data"][0]["msgID"];
              int nodeID = root["data"][0]["nodeID"];
              int iCommType =  root["data"][0]["iCommType"];
              float tempAir = root["data"][0]["tempAir"];
              bool ledStatus = root["data"][0]["ledStatus"];
              String latitudeGPS = root["data"][0]["GPS"];
              
              if(serverMsgID <= lastServerMsgID){
                Serial.print("no new msg from server serverMsgID :");
                Serial.println(serverMsgID);
                Serial.println();
                pktBuild();
                return;
              }
              lastServerMsgID = serverMsgID;
              
                telemtryPacketTX.nodeID = node;
                telemtryPacketTX.RXnodeID = root["data"][0]["nodeID"];
                telemtryPacketTX.msgType = msgType_Command;
                telemtryPacketTX.msgID = msgCount;
                telemtryPacketTX.iCommType = root["data"][0]["iCommType"];
                telemtryPacketTX.timeStamp = millis();
                telemtryPacketTX.tempAir = root["data"][0]["tempAir"];
                telemtryPacketTX.latitudeGPS = 19.785667;
                telemtryPacketTX.longitudeGPS = 20.674597;
                telemtryPacketTX.ledStatus = root["data"][0]["ledStatus"];
                msgCount++; 
                

              Serial.println("Received packet from server :");
              Serial.print("nodeID :");
              Serial.print(nodeID);
              Serial.print("  serverMsgID :");
              Serial.print(serverMsgID);
              Serial.print("  iCommType :");
              Serial.print(iCommType);
              Serial.print("  timeStamp :");
              Serial.print(timeStamp);
              Serial.print("  tempAir :");
              Serial.print(tempAir);
              Serial.print("  GPS :");
              Serial.print(latitudeGPS);
              Serial.print("  ledStatus :");
              Serial.println(ledStatus);
              Serial.println();
           }
        } 
        else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
            pktBuild();
        }
        

        http.end();
}
