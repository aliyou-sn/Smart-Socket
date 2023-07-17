#ifndef setup_h
#define setup_h
#include <Arduino.h>
#define SWITCH_OFF 0
#define CHECKING 1
#define SWITCH_ON 2
#define RELAY_OFF 0
#define RELAY_ON 1

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13

const char* ssid = "NITDA-ICT-HUB";                             
const char* password = "6666.2524#";                           
const char* mqtt_server = "172.16.122.37";                        
const char* mqttUser = "nithub";                              
const char* mqttPsw = "0000";                             
int mqttPort = 1883;                                     
const char* mqtt_DeviceName = "Isocket_1";                          
const char* deviceModel = "Model A";                            
const char* Version = "1.0";                                   
const char*	manufacturer = "NITHUB";


const int Switch = 4;
const int RELAY = 12;
const int I_Pin = 35;
bool relay = false;
unsigned long TimeInputs = 0;
unsigned long TimeCurrent = 0;
unsigned long TimeVolt = 0;
unsigned long TimeWatt = 0;
unsigned long TimeAmph = 0;
// float ProbeCurrent = 0.0;
// float ProbeVolt = 0.0;
// float ProbeWatt = 0.0;

int mqttCounterConn = 0;
bool InitSystem = true;
int canPublish = 0;
bool outlet = false;



long milisec = millis(); // calculate time in milliseconds
long Time=milisec/1000; // convert milliseconds to seconds
unsigned long msec=0;
int starttime;
int activetime;
int prevoustime = 0;
float sensitivity = 0.1;  // Set sensitivity for ACS712. 0.185 for 5A, 0.100 for 20A, 0.066 for 30A
int r1 = 1000;
int r2 = 10000;

unsigned long startMillis;  
unsigned long endMillis;  
unsigned long currentMillis;
const unsigned long period = 1000;

int hours = 0;
int mins = 0;
int secs = 0;
int sample=0;

float totalCharge = 0.0;
float averageAmps = 0.0;
float ampSeconds = 0.0;
float ampHours = 0.0;
float wattHours = 0.0;
float totalVolts = 0.0;
float averageVolts = 0.0;
int miliAmpHours = 0;
float amps = 0.00;
float volts = 0.00;
float watts = 0.00;
float amph = 0.00;



#endif
