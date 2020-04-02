#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();
#include <WiFi.h>
String apiKey = "LKEHC9TT1LHUYFPF";                  //  Enter your Write API key from ThingSpeak
const char *ssid =  "Aliyance";                                    // replace with your wifi ssid and wpa2 key
const char *pass =  "mech@1234";
const char* server = "api.thingspeak.com";
WiFiClient client;
void setup() 
{
    Serial.begin(115200);
    delay(10);
    Serial.println("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
 
}
void loop() 
{
  
int h = millis()/1000;
float t =71.5;

//h = hallRead();
//t = ((temprature_sens_read()-32)/1.8);                //changing temperature parameter to celsius
 if (client.connect(server,80))                                 //   "184.106.153.149" or api.thingspeak.com
    {  
                            
    String postStr = apiKey;
    postStr +="&field1=";
    postStr += String(h);
    postStr += "&field2=";
    postStr += String(t);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
 
    Serial.print("Hall: ");
    Serial.println(h);
    Serial.print("Temperature:");
    Serial.print(t);
    Serial.println(" C");
                             
    Serial.println("%. Send to Thingspeak.");
}
    //client.stop();
    Serial.println("Waiting...");
    delay(25000);
}
