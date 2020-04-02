//#include <SoftwareSerial.h>

//SoftwareSerial ss(4, 3); // GPS Module’s TX to D4 & RX to D3
HardwareSerial ss(2);
void setup(){

Serial.begin(9600);

    ss.begin(9600, SERIAL_8N1, 16, 17);  

}


void loop(){

while (ss.available() > 0){

byte gpsData = ss.read();

Serial.write(gpsData);

}
Serial.println("/n");
}
