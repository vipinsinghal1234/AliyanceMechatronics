/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-two-way-communication-esp32/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <esp_now.h>
#include <WiFi.h>

//uint8_t broadcastAddress[] = {};              // REPLACE WITH THE MAC Address of your receiver 
//uint8_t broadcastAddress[] = {0x24, 0x6F, 0x28, 0xAF, 0x43, 0x44};
uint8_t broadcastAddress[] = {0x24, 0x6F, 0x28, 0xAF, 0x4D, 0xB8};
uint8_t broadcastAddress1[] = {0x24, 0x6F, 0x28, 0xAF, 0x4D, 0xA8};

char macStr[18];
String success;             // Variable to store if sending data was successful

float temperature;          // Define variables to store BME280 readings to be sent
float humidity;
float pressure;

float incomingTemp;         // Define variables to store incoming readings
float incomingHum;
float incomingPres;

typedef struct struct_message {     //Structure example to send data    //Must match the receiver structure
    float temp;
    float hum;
    float pres;
} struct_message;

struct_message SensorReadings;        // Create a struct_message called BME280Readings to hold sensor readings

struct_message incomingReadings;      // Create a struct_message to hold incoming sensor readings

void OnDataSent(const uint8_t *mac, esp_now_send_status_t status) {       // Callback when data is sent
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
           
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.print(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success to:  " : "Delivery Fail");
  Serial.println(macStr);
  
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {          // Callback when data is received
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
           
  Serial.print("\n");        
  Serial.print("Last Packet Recv from: "); Serial.println(macStr);
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.println("Received Data");
  Serial.print("Temperature: ");
  Serial.print(incomingReadings.temp);
  Serial.println(" ºC");
  Serial.print("Humidity: ");
  Serial.print(incomingReadings.hum);
  Serial.println(" %");
  Serial.print("Pressure: ");
  Serial.print(incomingReadings.pres);
  Serial.println(" hPa");
  Serial.println();

  incomingTemp = incomingReadings.temp;
  incomingHum = incomingReadings.hum;
  incomingPres = incomingReadings.pres;
}
 
void setup() {
  Serial.begin(115200);       // Init Serial Monitor
  WiFi.mode(WIFI_STA);          // Set device as a Wi-Fi Station

 if (esp_now_init() != ESP_OK) {           // Init ESP-NOW
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);       // Once ESPNow is successfully Init, we will register for Send CB to    // get the status of Trasnmitted packet

  esp_now_peer_info_t peerInfo;                 // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  esp_now_peer_info_t peerInfo1;              // Register peer
  memcpy(peerInfo1.peer_addr, broadcastAddress1, 6);
  peerInfo1.channel = 0;  
  peerInfo1.encrypt = false;
          
  if (esp_now_add_peer(&peerInfo) != ESP_OK){     // Add peer
    Serial.println("Failed to add peer");
    return;
  }

  if (esp_now_add_peer(&peerInfo1) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  
  esp_now_register_recv_cb(OnDataRecv);         // Register for a callback function that will be called when data is received
}
 
void loop() {
  getReadings();
 
  // Set values to send
  SensorReadings.temp = temperature;
  SensorReadings.hum = humidity;
  SensorReadings.pres = pressure;

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &SensorReadings, sizeof(SensorReadings));
   
  if (result == ESP_OK) {
    Serial.println("Sent from master");
  }
  else {
    Serial.println("Error sending from master");
  }

  // Send message via ESP-NOW
  esp_err_t result1 = esp_now_send(broadcastAddress1, (uint8_t *) &SensorReadings, sizeof(SensorReadings));
   
  if (result1 == ESP_OK) {
    Serial.println("Sent from master");
  }
  else {
    Serial.println("Error sending from master");
  }
  
  //updateDisplay();
  delay(10000);
}


void getReadings(){
  temperature = 7;
  humidity = 8;
  pressure = 9;
}


void updateDisplay(){
  // Display Readings in Serial Monitor

}
