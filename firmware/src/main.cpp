#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h> 
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ACS712.h"
#include "setup.h"

String deviceName = "socket_1";                          		
String  TopicRelay = "socket_1/relay";  
String  TopicCurrent = "socket_1/curret";  
String  TopicVolt = "socket_1/volt";
String  TopicWatt = "socket_1/watt"; 
String  TopicAmph = "socket_1/amph";    			
String UniqueId;
byte  RelayStatus = RELAY_OFF;
byte Input = 0;
byte state_input = SWITCH_OFF;


void setup_wifi();
void MqttReconnect();
void MqttHomeAssistantDiscovery();
void MqttReceiverCallback(char* , byte*, unsigned int);
void MqttPublishStatus_Relay1();
void MqttPublishCurrent();
void MqttPublishVolt();
void MqttPublishWatt();
void MqttPublishAmph();
float GetCurrent();
float GetVolt();
float GetWatt();

WiFiClient wiFiClient;
PubSubClient mqttPubSub(wiFiClient);
// ACS712 sensor(ACS712_30A, I_Pin);
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

void setup(){
    pinMode(Switch, INPUT_PULLUP);
    pinMode(RELAY, OUTPUT);
    Serial.begin(115200);
    
    Serial.println("");
    Serial.println("----------------------------------------------");
    Serial.print("MODEL: ");
    Serial.println(deviceModel);
    Serial.print("DEVICE: ");
    Serial.println(deviceName);
    Serial.print("Software Rev: ");
    Serial.println(Version);
    Serial.println("----------------------------------------------");
    setup_wifi();
    mqttPubSub.setServer(mqtt_server, mqttPort);
    mqttPubSub.setCallback(MqttReceiverCallback);
    // sensor.calibrate();

    // display.begin(SSD1306_SWITCHCAPVCC);
    // display.clearDisplay();     // Clear the buffer
    // display.display();
    // delay(1000);
    // display.setTextSize(2); 
    // display.setTextColor(SSD1306_WHITE);
    // display.setCursor(0, 25);
    // //display.cp437(true);
    // display.write(" Nithub Unilag "); // text
    // display.display();
    // delay(1000);
    // display.write('\n'); // new line
    // display.setCursor(0, 0);
    // display.setTextSize(1);  
    // display.write("Nithub Isocket");
    // display.display();
    // display.clearDisplay();
    // delay(1000);
    // display.setCursor(0, 0);
    // display.setTextSize(1);  
    // display.write("Nithub Isocket");
    // display.display();  
    // display.clearDisplay();
    // delay(500);
    // display.setCursor(0, 18);
    // display.setTextSize(1.5);  
    // display.write("Voltage :");
    // display.setCursor(55, 18);
    // display.write("00.00 V");
    // display.setCursor(0, 30);
    // display.write("Current :");
    // display.setCursor(55, 30);
    // display.write("00.00 A");
    // display.setCursor(0, 42);
    // display.write("Time    :");
    // display.setCursor(55, 42);
    // display.write("00:00 H");
    // display.setCursor(0, 54);
    // display.write("AmpHour :00000 mAh");
    // display.setCursor(0, 58);
    // display.write("Energy  :00000 Wh");
    // display.display();
    // display.clearDisplay();
    // starttime = millis() / 1000;
}

void loop(){
    activetime = (millis() / 1000) - starttime;
    if (prevoustime < activetime){
        secs++;
        prevoustime = activetime;
    }
    if (secs > 59){
        mins++;
        secs = 0;
    }

    if (mins > 59){
        hours++;
        mins = 0;
    }

    if (hours > 23){
        hours = 0;
    }

    if(WiFi.status() == WL_CONNECTED)
    {
        if(!mqttPubSub.connected())
            MqttReconnect();
        else
            mqttPubSub.loop();
    }
    if(InitSystem)
    {
        delay(1000);
        InitSystem = false;
		    Serial.println("INIT SYSTEM...");
        MqttHomeAssistantDiscovery();
    }
    // Inputs Monitor
    if(millis() - TimeInputs > 200){
        TimeInputs = millis();

        switch(state_input){
            case SWITCH_OFF:
            {
                if(digitalRead(Switch) == 1){
                    state_input = CHECKING;
                }
            } break;

            case CHECKING:
            {
                if(digitalRead(Switch) == 1){
                    state_input = SWITCH_ON;
                    Input = 1;
                    RelayStatus = RELAY_ON;
                    digitalWrite(RELAY, RelayStatus);
                }
                else if(digitalRead(Switch) == 0){                      
                    state_input = SWITCH_OFF;
                    Input = 0;
                    RelayStatus = RELAY_OFF;
                    digitalWrite(RELAY, RelayStatus);
                }
                    MqttPublishStatus_Relay1();

            } break;

            case SWITCH_ON:
            {
                if(digitalRead(Switch) == 0){
                    state_input = CHECKING;
                }
            } break;
        }

    }

    if(millis() - TimeCurrent > 5000){
        TimeCurrent = millis();
        amps = GetCurrent();
        MqttPublishCurrent();
    }

    if(millis() - TimeVolt > 5000){
        TimeVolt = millis();
        volts = GetVolt();
        MqttPublishVolt();
    }

    if(millis() - TimeWatt > 5000){
        TimeWatt = millis();
        watts = GetWatt();
        MqttPublishWatt();
    }

    if(millis() - TimeAmph > 5000){
        TimeAmph = millis();
        amph = miliAmpHours;
        MqttPublishAmph();
    }
    // if(RelayStatus = RELAY_ON)
    // {
    // display.write("OUTLET ON"); // text
    // display.display();
    // display.setCursor(0, 0);
    // display.setTextSize(1.5);  
    // display.write("Nithub Isocket");
    // display.display(); 
    // display.setCursor(0, 16);
    // display.write("Voltage :");
    // display.setCursor(0, 26);
    // display.write("Current :      A");
    // display.setCursor(0, 36);
    // display.write("Time    :      H");
    // display.setCursor(0, 46);
    // display.write("AmpHour :");
    // display.setCursor(0, 56);
    // display.write("Energy  :");
    // display.setCursor(55, 16);
    // if (volts < 10){
    //     display.print("0");
    //     display.print(volts);
    //     display.print(" V");
    //     }
    // else{
    //     display.print(volts);
    //     display.print(" V");
    //     }

    // display.setCursor(55, 26);
    // if (amps < 10){
    //     display.print("0");
    //     display.print(amps);
    //     }
    // else{
    //     display.print(amps);
    //     }

    // display.setCursor(55, 36);
    // if (hours < 10){
    //     display.print("0");
    //     display.print(hours);
    //     display.print(":");
    //     }
    // else{
    //     display.print(hours);
    //     display.print(":");
    //     }
    // if (mins < 10){
    //     display.print("0");
    //     display.print(mins);
    //     }
    // else{
    //     display.print(mins);
    //     }

    // display.setCursor(55, 46);
    // if (miliAmpHours < 100){
    //     display.print("000");
    //     display.print(miliAmpHours);
    //     display.print(" mAh");
    //         }
    // else if (miliAmpHours < 1000){
    //     display.print("00");
    //     display.print(miliAmpHours);
    //     display.print(" mAh");
    //         }
    // else if (miliAmpHours < 10000){
    //     display.print("0");
    //     display.print(miliAmpHours);
    //     display.print(" mAh");
    //         }
    // else{ 
    //     display.print(miliAmpHours);
    //     display.print(" mAh");
    //         }
    // display.setCursor(55,56);
    // if (wattHours < 10){
    //     display.print("0");
    //     display.print(wattHours);
    //     display.print(" Wh");
    //         }
    // else{
    //     display.print(wattHours);
    //     display.print(" Wh");
    //         }

    // display.display();
    // display.clearDisplay();
    // }
    // else{
    //     display.write("OUTLET OFF"); // text
    //     display.display();
    // }


    // delay(1000);
    endMillis = millis();
}

void MqttPublishCurrent() 
{
    String topicMsg;
    String payload;
    if(mqttPubSub.connected())
    {
        payload = amps;
        topicMsg = TopicCurrent;
        mqttPubSub.publish(topicMsg.c_str(), payload.c_str());
    }
}

float GetCurrent(){
  int count1 = 20;
  int a0 = 0;
  for (int i=0; i< count1; i++) a0 += analogRead(I_Pin); 
  a0 = a0 / count1;
  delay(5);
  float voltage = a0 * 5 / 1023.0;
  float amps = (voltage - 2.5) / sensitivity;
  if (amps < 0.1) {
    amps = 0;
  }
  // float I = sensor.getCurrentAC();
  float I = random(2, 5);
//   return amps;
    return I;
}


void MqttPublishVolt() 
{
    String topicMsg;
    String payload;
    if(mqttPubSub.connected())
    {
        payload = volts;
        topicMsg = TopicVolt;
        mqttPubSub.publish(topicMsg.c_str(), payload.c_str());
    }
}

float GetVolt(){
  // float V = 220.00;
  float V = random(218, 220);
  return V;
}

void MqttPublishWatt() 
{
    String topicMsg;
    String payload;
    if(mqttPubSub.connected())
    {
        payload = watts;
        topicMsg = TopicWatt;
        mqttPubSub.publish(topicMsg.c_str(), payload.c_str());
    }
}

float GetWatt(){
  // float V = 220.00;
  // float I = sensor.getCurrentAC();
  //   float V = random(200, 220);
  //   float I = random(2, 10);
  msec = millis();
  sample=sample+1;
  totalCharge = totalCharge + amps;
  averageAmps = totalCharge / sample;
  ampSeconds = averageAmps * Time;
  ampHours = ampSeconds / 3600;
  miliAmpHours = ampHours*1000;
  totalVolts = totalVolts + volts;
  averageVolts = totalVolts / sample;
  wattHours = averageVolts * ampHours;
  Time = (float) msec / 1000.0;
  return wattHours;
}


void MqttPublishAmph() 
{
    String topicMsg;
    String payload;
    // if(mqttPubSub.connected())
    // {
        payload = amph;
        topicMsg = TopicAmph;
        mqttPubSub.publish(topicMsg.c_str(), payload.c_str());
    // }
}

void setup_wifi(){
    int counter = 0;
    byte mac[6];
    delay(10);
    WiFi.begin(ssid, password);
    WiFi.macAddress(mac);
    UniqueId =  String(mac[0],HEX) + String(mac[1],HEX) + String(mac[2],HEX) + String(mac[3],HEX) + String(mac[4],HEX) + String(mac[5],HEX);

    while(WiFi.status() != WL_CONNECTED && counter++ < 5){
        delay(500);
    }

}

void MqttReconnect(){
    while (!mqttPubSub.connected()  && (mqttCounterConn++ < 4))
    {
        Serial.print("Attempting MQTT connection...");
        if (mqttPubSub.connect(mqtt_DeviceName, mqttUser, mqttPsw))
        {
            Serial.println("connected");
            mqttPubSub.subscribe("homeassistant/status");
            mqttPubSub.subscribe((TopicRelay + "/set").c_str());
            delay(500);
        } else
        {
            Serial.print("failed, rc=");
            Serial.print(mqttPubSub.state());
            Serial.println(" try again in 3 seconds");
            delay(3000);
        }
    }
    mqttCounterConn = 0;
}

void MqttHomeAssistantDiscovery(){
    String discoveryTopic;
    String payload;
    String strPayload;
    int uniqueId_increment = 0;
    if(mqttPubSub.connected())
    {
        StaticJsonDocument<600> payload;
        JsonArray modes;
        JsonObject device;
        JsonArray identifiers;

		discoveryTopic = "homeassistant/switch/" + deviceName + "_relay/config"; 
        payload["name"] = deviceName + ".relay";
        payload["uniq_id"] = UniqueId + "_relay";
        payload["stat_t"] = TopicRelay + "/state";
        payload["cmd_t"] = TopicRelay + "/set";
        device = payload.createNestedObject("device");
        device["name"] = deviceName;
        device["model"] = deviceModel;
        device["sw_version"] = Version;
        device["manufacturer"] = manufacturer;
        identifiers = device.createNestedArray("identifiers");
        identifiers.add(UniqueId);

        serializeJsonPretty(payload, Serial);
        Serial.println(" ");
        serializeJson(payload, strPayload);

        mqttPubSub.publish(discoveryTopic.c_str(), strPayload.c_str());
        delay(100);


//Discover Current
        discoveryTopic = "homeassistant/sensor/" + deviceName + "_current/config"; 
        payload.clear();
        modes.clear();
        device.clear();
        identifiers.clear();
        strPayload.clear();

        uniqueId_increment++;
        payload["name"] = deviceName + ".current";
        payload["uniq_id"] = UniqueId + "_" + String(uniqueId_increment);
        payload["stat_t"] = TopicCurrent;
        payload["dev_cla"] = "Current";
        payload["unit_of_meas"] = "Amp";
        device = payload.createNestedObject("device");
        device["name"] = deviceName;
        device["model"] = deviceModel;
        device["sw_version"] = Version;
        device["manufacturer"] = manufacturer;
        identifiers = device.createNestedArray("identifiers");
        identifiers.add(UniqueId);

        serializeJsonPretty(payload, Serial);
        Serial.println(" ");
        serializeJson(payload, strPayload);
        mqttPubSub.publish(discoveryTopic.c_str(), strPayload.c_str());
        delay(100);


//Discover Voltage
        discoveryTopic = "homeassistant/sensor/" + deviceName + "_voltage/config"; 
        payload.clear();
        modes.clear();
        device.clear();
        identifiers.clear();
        strPayload.clear();

        uniqueId_increment++;
        payload["name"] = deviceName + ".volt";
        payload["uniq_id"] = UniqueId + "_" + String(uniqueId_increment);
        payload["stat_t"] = TopicVolt;
        payload["dev_cla"] = "Voltage";
        payload["unit_of_meas"] = "Volt";
        device = payload.createNestedObject("device");
        device["name"] = deviceName;
        device["model"] = deviceModel;
        device["sw_version"] = Version;
        device["manufacturer"] = manufacturer;
        identifiers = device.createNestedArray("identifiers");
        identifiers.add(UniqueId);

        serializeJsonPretty(payload, Serial);
        Serial.println(" ");
        serializeJson(payload, strPayload);
        mqttPubSub.publish(discoveryTopic.c_str(), strPayload.c_str());
        delay(100);


//Discover Watt
        discoveryTopic = "homeassistant/sensor/" + deviceName + "_watt/config"; 
        payload.clear();
        modes.clear();
        device.clear();
        identifiers.clear();
        strPayload.clear();

        uniqueId_increment++;
        payload["name"] = deviceName + ".watt";
        payload["uniq_id"] = UniqueId + "_watt" ;
        payload["stat_t"] = TopicWatt;
        payload["dev_cla"] = "Watt";
        payload["unit_of_meas"] = "W";
        device = payload.createNestedObject("device");
        device["name"] = deviceName;
        device["model"] = deviceModel;
        device["sw_version"] = Version;
        device["manufacturer"] = manufacturer;
        identifiers = device.createNestedArray("identifiers");
        identifiers.add(UniqueId);

        serializeJsonPretty(payload, Serial);
        Serial.println(" ");
        serializeJson(payload, strPayload);
        mqttPubSub.publish(discoveryTopic.c_str(), strPayload.c_str());
        delay(500);
//Discover amph
        discoveryTopic = "homeassistant/sensor/" + deviceName + "_amph/config"; 
        payload.clear();
        modes.clear();
        device.clear();
        identifiers.clear();
        strPayload.clear();

        uniqueId_increment++;
        payload["name"] = deviceName + ".amph";
        payload["uniq_id"] = UniqueId + "_" + String(uniqueId_increment);
        payload["stat_t"] = TopicCurrent;
        payload["dev_cla"] = "Amp Hour";
        payload["unit_of_meas"] = "mAh";
        device = payload.createNestedObject("device");
        device["name"] = deviceName;
        device["model"] = deviceModel;
        device["sw_version"] = Version;
        device["manufacturer"] = manufacturer;
        identifiers = device.createNestedArray("identifiers");
        identifiers.add(UniqueId);

        serializeJsonPretty(payload, Serial);
        Serial.println(" ");
        serializeJson(payload, strPayload);
        mqttPubSub.publish(discoveryTopic.c_str(), strPayload.c_str());
        delay(500);

		// Send Current status of Relays
        MqttPublishStatus_Relay1();
        delay(100);
    }
}

void MqttReceiverCallback(char* topic, byte* inFrame, unsigned int length){
    byte state = 0;
    String payload;
    String topicMsg;
    StaticJsonDocument<256> doc;

    for (int i = 0; i < length; i++)
    {
        payload += (char)inFrame[i];
    }


    if(String(topic) == String("homeassistant/status"))
    {
        if(payload == "online")          
        {
            MqttHomeAssistantDiscovery(); 
        }
    }

	if(String(topic) == String(TopicRelay + "/set")) 
    {
        if(payload == "ON")
            RelayStatus = RELAY_ON;
        else if(payload == "OFF")
            RelayStatus = RELAY_OFF;
        digitalWrite(RELAY, RelayStatus);
        MqttPublishStatus_Relay1();
    }
}

void MqttPublishStatus_Relay1() 
{
    String strPayload;
    if(mqttPubSub.connected())
    {
        if(RelayStatus == 0)
            strPayload = "OFF";
        else
            strPayload = "ON";

        mqttPubSub.publish((TopicRelay + "/state").c_str(), strPayload.c_str());
    }
}



