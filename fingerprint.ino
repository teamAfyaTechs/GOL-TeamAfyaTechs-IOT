//Remember to assign the correct device id
//Remember to assign the correct device id
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <string>

//Declare the pins used for Serial communication on ESP32
#define txd1 17
#define rxd1 16
String deviceID= "C";

//credentials for WiFi network
const char* ssid = "Houston";
const char* password = "Network3";
String string2="NoFinger";
String string1="Finger";
boolean string_comparison;
String wifiStrength;
String wifiName;
String wifiNameStrength;
unsigned long interval = 5000;
unsigned long previousmillis =0;
unsigned long currentmillis =0;
int PostCode;
char buffer1[400];
int id=0;
int cMode=0;
// Assignment for device number
String finger_and_deviceID;

void setup() {
  Serial.begin(115200); //To allow use of Serial monitor
  Serial2.begin(115200, SERIAL_8N1, rxd1, txd1); //Use of multiple Serial, for communication between the two microcontrollers.
  WiFi.disconnect();  //disconnects wifi from previous network
  WiFi.mode(WIFI_STA); //sets the Wifi module to station mode - devices are connecting to it
  WiFi.begin(ssid, password); //To connect ESP32 to the WiFi Network
  Serial.println("Connecting to wifi");
  
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Attempting to: ");
    delay(500);
    WiFi.begin(ssid, password);
    break;
  }
  
  Serial.println("\n Connected to a wifi network");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP()); //Fetches then shows the IP address of the wifi network
  Serial.print("Wifi strength");
  Serial.println(WiFi.RSSI());
  Serial.println(WiFi.macAddress());
}


void loop()
{
  getWifiDetails(); 
  previousmillis = millis();
  Serial.println("Done Getting WiFi Details");
  reconnectingFunction();
  Serial.println("Waiting for No Finger");
  
  while(Serial2.available()==0)
  {
    currentmillis= millis();
    if (currentmillis - previousmillis >= interval)
         {
        Serial.println("Stuck in the while loop");
        Serial2.available()==1; //artificial
        Serial2.println(wifiNameStrength);
          }   
      return;
    }
        
  Serial2.println("*********************************************************************************************");
        
  String noFingerFromPro=Serial2.readString();
  noFingerFromPro.trim();
  Serial.println(noFingerFromPro);
  
  if(noFingerFromPro.equals(string2)) // string2 is "nofinger" from promini
  { 
     Serial2.println(wifiNameStrength); //actual 
  }
  else if(noFingerFromPro.equals(string1)) // string2 is "finger" from promini
  { 
     Serial2.println("");
     receive_data();
     Serial2.println(" ");
     //noFingerFromPro;
  }
}

void getWifiDetails()
{
  wifiStrength=WiFi.RSSI();
  wifiName=WiFi.SSID();
  wifiNameStrength="Connected to: "+wifiName+" WSS = "+wifiStrength;
  Serial.println(wifiNameStrength);  
}

void reconnectingFunction(){
  while (WiFi.status() != WL_CONNECTED)
  {
     wifiNameStrength="No WiFi Connection";
     Serial.println("Wifi disconnected:");
     Serial.print("Searching for WiFi..");
     WiFi.mode(WIFI_STA);
     WiFi.disconnect();
     WiFi.begin(ssid, password);
     delay (5000);
     //ESP.restart();
    // delay (5000);
     break;
  }
}

void receive_data()
{
  while(Serial2.available()==0){}
  String clockMode=Serial2.readString();
  Serial.println(clockMode);
  if(clockMode=="G")
  {
    Serial.println("clocking cancelled");
    return;
  }
  while(Serial2.available()==0){}
  String fingerPrintID_Pro=Serial2.readString();
 
  Serial.println(clockMode);
  if(clockMode=="A")
  {
    cMode=1; // 1 maps to clocking out
  }
  else
  {
    cMode=0; //0 maps to clocking in
  }
  id=fingerPrintID_Pro.toInt();
  finger_and_deviceID = id + deviceID;
  Serial.print("Clock Mode: ");
  Serial.println(cMode);
  Serial.print("Fingerprint Id: ");
  Serial.println(finger_and_deviceID);
  Serial.println("POSTING DATA STARTED");
  postFingerPrintID(finger_and_deviceID,cMode);
  Serial.println("POSTING DATA ENDED");
  delay(1000);
  //Serial2.println("Posting Success");
 }



void postFingerPrintID(String finger_and_deviceID, int clockMode) 
{
  Serial.println("Get Authentication"); //to know that this function is running
  //New IP
  sprintf(buffer1,"http://41.89.227.241:8002/api/StudentStatusTest/stafflog?fingerId=%s&direction=%d",finger_and_deviceID,clockMode);
  
  Serial.println(buffer1);
  //Check WiFi connection status - pre-check to confirm we are still connected to the WiFi network, before trying to do the request
  if (WiFi.status() == WL_CONNECTED)
  {
     HTTPClient http;
     http.begin(buffer1);
     http.addHeader("Content-Type", "application/json");
     //String staff = "{\"fingerId\": \"" +fingerPrintID+ "\",\"direction\": \"" +clockMode+ "\"}";
     PostCode = http.POST(buffer1);
       
     if(PostCode>0)
     {
     String payload = http.getString();  //Get the response to the request
     Serial.println(payload);           //Print request answer
     }
     else
     {
       Serial.print("Error on sending POST: ");
       Serial.println(PostCode);
     }
  }
  else
  {
    Serial.println("Error in WiFi connection");
  }
}
//End of code
