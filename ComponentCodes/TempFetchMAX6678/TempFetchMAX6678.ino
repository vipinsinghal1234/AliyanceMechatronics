#include "max6675.h"
 
//int thermoDO = 19;
//int thermoCS = 18;
//int thermoCLK = 5;
////
int thermoDO = 12;
int thermoCS = 13;
int thermoCLK = 15;
// 
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
 
void setup()
{
Serial.begin(9600);
Serial.println("MAX6675 test");
delay(500);
}
 
void loop()
{
// basic readout test, just print the current temp
 
Serial.print("C = ");
Serial.println(thermocouple.readCelsius());
Serial.print("F = ");
Serial.println(thermocouple.readFahrenheit());
 
delay(1000);
}
