/* 
  Check the new incoming messages, and print via serialin 115200 baud rate.
  
  by Aaron.Lee from HelTec AutoMation, ChengDu, China
  成都惠利特自动化科技有限公司
  www.heltec.cn
  
  this project also realess in GitHub:
  https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series
*/

#include "heltec.h"

#include "esp_http_client.h"
#include <WiFi.h>
#include "Arduino.h"

const char* ssid = "Aliyance";
const char* password = "mech@1234";

const char *post_url = "http://192.168.0.107:5000/api/insertIntoTbl"; // Location to send POSTed data
//const char *post_data = "id='5555'&node_id='655'&comm_type='wifi'&timestamp='8855'&temperature='67'&gps_coord='122.445566,33.442211'"; 

bool internet_connected = false;

byte localAddress = 0xAA;     // address of this device
byte destination = 0xBB;      // destination to send to

bool receiveflag = false; // software flag for LoRa receiver, received data makes it true.

#define BAND    433E6  //you can set band here directly,e.g. 868E6,915E6

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

void setup() {
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  LoRa.onReceive(onReceive);      //WIFI Kit series V1 not support Vext control
  Serial.begin(115200);
  LoRa.receive();

    if (init_wifi()) { // Connected to WiFi
    internet_connected = true;
    Serial.println("Internet connected");
  }

  Serial.println("setup"); 
}

void loop() {
  if(receiveflag){
      receiveflag= false;
      Serial.println("Data post");
      post_something();
  }
}


void onReceive(int packetSize)//LoRa receiver interrupt service
{
   if (packetSize == 0) return;          // if there's no packet, return

  Serial.print("Received packet '");
  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingLength = LoRa.read();    // incoming msg length
  LoRa.readBytes((uint8_t *)&telemtryPacket, sizeof(telemtryPacket));


  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress && recipient != 0xFF) {
    Serial.println("This message is not for me.");
    return;                             // skip rest of function
  }

  // if message is for this device, or broadcast, print details:
  Serial.println("Received from: 0x" + String(sender, HEX));
  Serial.println("Sent to: 0x" + String(recipient, HEX));
  Serial.println("Message length: " + String(incomingLength)); 
  Serial.println(telemtryPacket.nodeID);
  Serial.println(telemtryPacket.msgID);
  Serial.println(telemtryPacket.iCommType);
  Serial.println(telemtryPacket.timeStamp);
  Serial.println(telemtryPacket.tempAir);
  Serial.println(((float)telemtryPacket.latitudeGPS),6);
  Serial.println(((float)telemtryPacket.longitudeGPS),6);
  Serial.println();
  receiveflag = true;  
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
      Serial.println();
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

static esp_err_t post_something()
{
   //String idConvertToStr= String(1001);
//    String nodeIDConvertToStr= String(telemtryPacket.nodeID);  
//    String longitudeGPSConvertToStr= String(((float)telemtryPacket.longitudeGPS),6); 
//    String timeStampConvertToStr = String(telemtryPacket.timeStamp);
//    String tempAirConvertToStr = String(telemtryPacket.tempAir);
//    String iCommTypeConvertToStr = String(telemtryPacket.iCommType);
//    String latitudeGPSConvertToStr = String(((float)telemtryPacket.latitudeGPS),6);
//    String msgIDStr= String(telemtryPacket.msgID);
    
    //String idString ="id="+String(1001);
    String nodeIDString = "&nodeID="+String(telemtryPacket.nodeID);
    String GPSString = "&GPS="+String(((float)telemtryPacket.latitudeGPS),6) + "," + String(((float)telemtryPacket.longitudeGPS),6);
    String timeStampString = "&timeStamp="+String(telemtryPacket.timeStamp);
    String tempAirString = "&tempAir="+String(telemtryPacket.tempAir);
    String iCommTypeString = "&iCommType="+String(telemtryPacket.iCommType);
    String msgIDString = "&msgID="+String(telemtryPacket.msgID);
    
    String finalStr;
    
    finalStr = nodeIDString + iCommTypeString + timeStampString + tempAirString + GPSString + msgIDString;
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
  
  // esp_http_client_set_header(http_client, "Content-Type", "image/jpg"); // sending a jpg file

  esp_err_t err = esp_http_client_perform(http_client);
  if (err == ESP_OK) {
    Serial.print("esp_http_client_get_status_code: ");
    Serial.println(esp_http_client_get_status_code(http_client));
  }

  esp_http_client_cleanup(http_client);
}
