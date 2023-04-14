#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <MCUFRIEND_kbv.h> 
#include <SPI.h> 
#include <ArduinoJson.h>
#include <Keypad.h>

//#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(18, 19);
//SoftwareSerial Serial1(10, 11); // RX, TX
//SoftwareSerial Serial2(12, 13); // RX, TX
//#else
#define mySerial Serial1
//#endif
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id;

// Device id here
//String deviceID= "F";
//String wifiStrength="Searching for WiFi";
char menu ='3';
char enter = 'D';
int finger_ID=0;
char state = '0';
char password_entry ='1';
char cancel = 'C';
char delete_database = 'B';
char enroll_key = 'A';

#define SS_PIN 53 //slave select pin on the RFID
#define buzzer 46
MCUFRIEND_kbv tft; // Create MCUFRIEND_kbv instance

///declarations for fingerprint sensor
uint8_t getFingerprintEnroll();
uint8_t deleteFingerprint(uint8_t id);
uint8_t getFingerprintID(void);
void fingerprint_matched(void);
void tft_denied_access_menu(void);
void tft_access_menu (void);
void fingerid_deletion_menu(void);
void enroll_menu(void);
int convert(void);
void database_deleted(void);
void delete_data(void);
unsigned long showWelcomeScreen();
unsigned long showWaitingStatus();
unsigned long showWifiNetwork();
void ENROLL_FAILED(void);
void beep_init(void);  
void beep_succesfull(void);
void beep_failed(void);
void main_menu(void);

void registration_menu(void);
void scan_1();
void image_taken(void);
void scan_2(void);
void succesfull(int ID);
void unsuccesfull(void);
void invalid_ID(void);
int password(void);
int return_finger_id (void);
void WRONG_PASSWORD(void);

#define BLACK 0x0000
#define WHITE 0xFFFF
#define DARKGREEN 0x03E0
#define DARKCYAN 0x03EF
#define MAROON 0x7800
#define PURPLE 0x780F
#define YELLOW 0xFFE0
#define ORANGE 0xFD20
#define GREENYELLOW 0xAFE5
#define PINK 0xF81F
#define RED 0xF800
#define default_password 4736        

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'4','5','6','B'},
  {'1','2','3','A'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {A6, A7, A8, A9}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {A10, A11, A12, A13}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup()
{
  Serial.begin(9600); 
  Serial1.begin(115200);
  Serial2.begin(9600); // used for bluetooth communication
  //Serial2.begin(115200); 
  SPI.begin();
  tft.reset();
  tft.begin(0x9341);
  tft.setRotation(1);
  showWelcomeScreen();
  delay(2000);
  showWaitingStatus();
  beep_init();
  delay(100);
  Serial.println("\n\nAdafruit Fingerprint sensor enrollment");
  // set the data rate for the sensor serial port
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);
}

void loop()
{
 Serial.println("Main loop starts");
 getFingerprintID(); 
 delay(50);
 finger.getTemplateCount();
 Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
 Serial.println("Waiting for valid finger...");
 
  char customKey = customKeypad.getKey();
  
   if(customKey == password_entry)
   {
      registration_menu();
      int password_2 = password();
      Serial.println("Past password function");
      Serial.println(password_2);
   
      if(default_password == password_2)
      {
          main_menu();  
          while(customKey != cancel )
          {
              customKey = customKeypad.getKey();
              // if key A is pressed execute this block
              if (customKey == enroll_key)
              {
                  enroll_menu();
                  delay (1000);
                  break;
              }
      
              // This block gets executed when delete_database key(B) is pressed
              else if (customKey == delete_database)
              {
                 // This takes the user to the option of chosing to clear entire database or finger ID
                 fingerid_deletion_menu ();
                 int finger_id2 = return_finger_id ();
                 Serial.println(finger_id2);
                 //finger.emptyDatabase();
                 deleteFingerprint(finger_id2);
                 database_deleted (finger_id2);
                 delay(1000);          
                 break;   
                 
                  while(customKey != cancel )
                  {
                      customKey = customKeypad.getKey();
                      // This block gets executed when delete_database key(B) is pressed
                      if (customKey == delete_database)
                      {
                            //this block clears the Entire database
                            Serial.println("YES");                  
                            finger.emptyDatabase();
                            database_deleted(); 
                            delay (1000);  
                            showWaitingStatus();     
                            return;
                      }
                      // This block gets executed when delete_database key(C) is pressed
                    
                            Serial.println("NO");
                            // This block deletes the specific finger_ID that the user has input
                            deleteFingerprint(2); 
                            delay(1000);
                          
        
                  }
                              
                }
  
          }
      }
      else
      {
       WRONG_PASSWORD(); 
       delay(3000);
      }
       showWaitingStatus();
    }
     
  //ENROLL END  HERE
   Serial.println("Main loop done");
   Serial.println(" ");
   return;
  }


//This Part gets the user to enter the fingerprint id to delete
int return_finger_id (void)
{
 int finger_ID=0;
 int position_cursor = 120;
 char customKey_1='t';
    while(customKey_1!= enter)
    {
      customKey_1 = customKeypad.getKey();
      if((customKey_1) && (customKey_1!= enter))
      {
   tft.setCursor(position_cursor,170); 
   tft.setTextSize(3);
   tft.setTextColor(WHITE);
   tft.println("*");
   finger_ID = (finger_ID*10)+ (customKey_1 - '0');
    Serial.println(finger_ID);
        position_cursor+=20;
      } 
}
 return finger_ID;
}

/*
Procedure to enroll starts here
We first get the finger print id then we increment
*/

    void enroll_menu(void)
  {
  Serial.println("Ready to enroll a fingerprint!");
  Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
    
  finger.getTemplateCount();
  id = finger.templateCount+1;
  
  if (id == 0) 
  {
      Serial.println("Invalid ID");
  }
  scan_1();
  Serial.println(id);  
  
    // wait for scanner
   while (! getFingerprintEnroll() );
   delay(1000);
    return;
    }
/* This is the Actual enroll function
*/
 uint8_t getFingerprintEnroll()
 {
  int p = -1;\
  
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
/*
 This is a switch case to check whether the image was taken successfully  
*/
  while (p != FINGERPRINT_OK)
  {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
    image_taken(id);
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  
/* 
This is a switch case to check whether the conversion of the image was successful
*/  

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  /*
  The code below is used to ask the user to place finger the second time 
  It also checks whether the image was taken and converted successfully
*/  

  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  scan_2();
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");

/*
 This is a switch case to check whether the image was taken successfully  
*/
  
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
    //place code for succesfull here
   
    Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  /* 
This is a switch case to check whether the conversion of the image was successful
*/ 

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
   
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
  
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #"); Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
    
        succesfull(id);
        delay(3000);
       showWaitingStatus();
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("ENROLL FAILED");
  ENROLL_FAILED();
  delay(1000);
 showWaitingStatus();
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  ENROLL_FAILED();
  }

  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  return true;
}

uint8_t getFingerprintID() 
{
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER: // When no finger is detected, get the wifi strength
     
      Serial.println("Start of fingerprint id");
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    
    beep_succesfull();
    
    tft_access_menu ();
  
    showWaitingStatus(); 
 // CODE CONTINUATION 
  
  }

  else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    tft_denied_access_menu();
     delay(2000);
    showWaitingStatus();
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }
  return finger.fingerID;
  
}
/*
This is the function to delete finger print based on the id 
*/

uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;

  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    Serial.println("Deleted!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
  } else {
    Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
  }

  return p;
}
int password(void)
{
 int finger_ID=0;
 int position_cursor = 120;
 char customKey_1='t';
    while(customKey_1!= enter)
    {
      customKey_1 = customKeypad.getKey();
      if((customKey_1) && (customKey_1!= enter))
      {
   tft.setCursor(position_cursor,170); 
   tft.setTextSize(3);
   tft.setTextColor(WHITE);
   tft.println("*");
   finger_ID = (finger_ID*10)+ (customKey_1 - '0');
    Serial.println(finger_ID);
        position_cursor+=20;
      } 
}
 return finger_ID;
}
///Buzzer functions
  
void beep_init(void)
{
  //set buzzer as high to ring
  pinMode(buzzer, OUTPUT);
}
  
void beep_succesfull(void)
{
  //beep once if succesfull
  digitalWrite(buzzer, HIGH); // turn the LED on (HIGH is the voltage level)   
  delay(100); // wait for a second // wait for a second
  digitalWrite(buzzer, LOW); // turn the LED off by making the voltage LOW
} 

void beep_failed(void)
{
  //beep thrice if not succefull
  digitalWrite(buzzer, HIGH); // turn the LED on (HIGH is the voltage level)   
  delay(100); // wait for a second // wait for a second
  digitalWrite(buzzer, LOW); // turn the LED off by making the voltage LOW
  delay(100); 
  
  //beep once if succesfull
  digitalWrite(buzzer, HIGH); // turn the LED on (HIGH is the voltage level)   
  delay(100); // wait for a second // wait for a second
  digitalWrite(buzzer, LOW); // turn the LED off by making the voltage LOW
  delay(100); 
  
  //beep once if succesfull
  digitalWrite(buzzer, HIGH); // turn the LED on (HIGH is the voltage level)   
  delay(100); // wait for a second // wait for a second
  digitalWrite(buzzer, LOW); // turn the LED off by making the voltage LOW
  delay(100); 
}

// Tft functions
unsigned long showWelcomeScreen(){
  tft.fillScreen(PURPLE);
  tft.setTextSize(3);
  tft.setCursor(40,95);
  tft.setTextColor(WHITE);
  tft.println("W E L C O M E");  
  tft.setCursor(100, 180);
  tft.println("HOSPITAL");
}

unsigned long showWaitingStatus(){
   tft.fillScreen(WHITE); 
   tft.setCursor(50,20);
   tft.setTextSize(2);
   tft.setTextColor(BLACK);
   tft.println("PATIENT ACCESS CONTROL");
   tft.setCursor(50,30);
   tft.println("___________________");
   tft.setCursor(20,110); 
   tft.setTextSize(3);
   tft.setTextColor(BLACK);
   tft.println("SCAN YOUR FINGER ");
}

void registration_menu(void)
{
   tft.fillScreen(PURPLE); 
   tft.setCursor(60,20);
   tft.setTextSize(2);
   tft.setTextColor(WHITE);
   tft.println("ENTER PASSWORD");
   tft.setCursor(60,28);
   tft.println("______________");
   tft.setCursor(120,180); 
   tft.setTextSize(3);
   tft.setTextColor(WHITE);
   tft.println("____");
  
 }

void scan_1(void)
{
   tft.fillScreen(PURPLE); 
   tft.setCursor(20,100); 
   tft.setTextSize(3);
   tft.setTextColor(WHITE);
   tft.println("PLACE YOU FINGER ");
   tft.setCursor(30,130); 
   tft.setTextSize(3);
   tft.setTextColor(WHITE);
   tft.println("ON THE SCANNER");
 }

 
/*
This function Asks the user to choose an option to either delete the whole database or delete a specific finger_ID
*/
 void delete_data(void)
{
   tft.fillScreen(PURPLE); 
   tft.setCursor(30,40); 
   tft.setTextSize(3);
   tft.setTextColor(WHITE);
   tft.println("SELECT OPTION");
   tft.setCursor(60,130); 
   tft.setTextSize(3);
   tft.setTextColor(WHITE);
   tft.println("B = Database");
   tft.setCursor(60,180); 
   tft.setTextSize(3);
   tft.setTextColor(WHITE);
   tft.println("C = Finger ID");
 }

/*
This function prompts the user to Enter the finger_ID to be deleted
*/
void fingerid_deletion_menu (void){

 tft.fillScreen(PURPLE); 
   tft.setCursor(60,20);
   tft.setTextSize(2);
   tft.setTextColor(WHITE);
   tft.println("ENTER FINGER_ID");
   tft.setCursor(60,28);
   tft.println("______________");
   tft.setCursor(120,180); 
   tft.setTextSize(3);
   tft.setTextColor(WHITE);
   tft.println("___");  
}

 
void image_taken(int id)
{
   tft.fillScreen(PURPLE); 
    tft.setCursor(50,80); 
   tft.setTextSize(3);
   tft.setTextColor(WHITE);
   tft.println("IMAGE TAKEN");
   tft.setCursor(40,130); 
   tft.setTextSize(3);
   tft.setTextColor(WHITE);
   tft.println("REMOVE FINGER");

  //  The block prints the Fingerprint ID on the TFT Screen  
   tft.setCursor(150,220); 
   tft.setTextSize(2);
   tft.setTextColor(WHITE);
   tft.println(id);   

  }

  
void scan_2(void)
{
   tft.fillScreen(PURPLE); 
   tft.setCursor(10,80); 
   tft.setTextSize(3);
   tft.setTextColor(WHITE);
   tft.println("PLACE SAME FINGER ");
   tft.setCursor(40,130); 
   tft.setTextSize(3);
   tft.setTextColor(WHITE);
   tft.println("ON THE SCANNER");
   tft.setCursor(110,180); 
   tft.setTextSize(3);
   tft.setTextColor(WHITE);
   tft.println("AGAIN");
  
 }

 void succesfull(int id)
 {
   tft.fillScreen(PURPLE); 
    tft.setCursor(80,60); 
   tft.setTextSize(2);
   tft.setTextColor(WHITE);
   tft.println("IMAGE MATCHED");
   tft.setCursor(40,120); 
   tft.setTextSize(2);
   tft.setTextColor(WHITE);
   tft.println("FINGERPRINT ID STORED");
   tft.setCursor(80,180); 
   tft.setTextSize(2);
   tft.setTextColor(WHITE);
   tft.println("SUCCESSFULLY");
   tft.setCursor(150,220); 
   tft.setTextSize(2);
   tft.setTextColor(WHITE);
   tft.println(id);

 }


 void unsuccesfull(void)
 {
   tft.fillScreen(PURPLE); 
    tft.setCursor(80,60); 
   tft.setTextSize(2);
   tft.setTextColor(WHITE);
   tft.println("UNSUCCESSFUL");
   tft.setCursor(60,120); 
   tft.setTextSize(2);
   tft.setTextColor(WHITE);
   tft.println("FINGERPRINT IMAGES");
   tft.setCursor(80,180); 
   tft.setTextSize(2);
   tft.setTextColor(WHITE);
   tft.println("DID NOT MATCH");
  }

 void fingerprint_matched(void)
 {
   tft.fillScreen(PURPLE); 
    tft.setCursor(30,60); 
   tft.setTextSize(3);
   tft.setTextColor(WHITE);
   tft.println("SCAN SUCCESSFUL");
   tft.setCursor(100,120); 
   tft.setTextSize(3);
   tft.setTextColor(WHITE);
   tft.println("WELCOME");
  }
 void tft_access_menu(void)
 {
   tft.fillScreen(PURPLE); 
   tft.setCursor(30,50); 
   tft.setTextSize(3);
   tft.setTextColor(WHITE);
   tft.println("ACCESS GRANTED");

   tft.setCursor(35,110); 
   tft.setTextSize(2);
   tft.setTextColor(WHITE);
   tft.println("TURN HOLDER TO OPEN");
   
   tft.setCursor(100,190); 
   tft.setTextSize(3);
   tft.setTextColor(WHITE);
   tft.println("WELCOME");

   tft.setCursor(150,220); 
   tft.setTextSize(2);
   tft.setTextColor(WHITE);
   tft.println(finger.fingerID);
   
  }

void tft_denied_access_menu(void)
 {
  beep_failed();
   tft.fillScreen(PURPLE); 
    tft.setCursor(40,60); 
   tft.setTextSize(3);
   tft.setTextColor(WHITE);
   tft.println("ACCESS DENIED");
   tft.setCursor(40,120); 
   tft.setTextSize(2);
   tft.setTextColor(WHITE);
   tft.println("DID NOT FIND A MATCH");
   tft.setCursor(90,170); 
   tft.setTextSize(2);
   tft.setTextColor(WHITE);
   tft.println("TRY AGAIN");
  }

 void ENROLL_FAILED(void)
 {
   tft.fillScreen(PURPLE); 
    tft.setCursor(120,60); 
   tft.setTextSize(3);
   tft.setTextColor(WHITE);
   tft.println("ERROR!");
   tft.setCursor(50,120); 
   tft.setTextSize(3);
   tft.setTextColor(WHITE);
   tft.println("ENROLL FAILED");
   tft.setCursor(80,170); 
   tft.setTextSize(3);
   tft.setTextColor(WHITE);
   tft.println("TRY AGAIN");
  }


 void WRONG_PASSWORD(void)
 {
   tft.fillScreen(PURPLE); 
    tft.setCursor(120,60); 
   tft.setTextSize(3);
   tft.setTextColor(WHITE);
   tft.println("ERROR!");
   tft.setCursor(50,120); 
   tft.setTextSize(3);
   tft.setTextColor(WHITE);
   tft.println("WRONG PASSWORD");
   tft.setCursor(80,170); 
   tft.setTextSize(3);
   tft.setTextColor(WHITE);
   tft.println("TRY AGAIN");
  }

void database_deleted(int fingerid)
{
   tft.fillScreen(WHITE); 
   tft.setCursor(135,40); 
   tft.setTextSize(3);
   tft.setTextColor(BLACK);
   tft.println("ID");
   tft.setTextSize(3);
   tft.setCursor(140,90);
   tft.println(fingerid);
   tft.setCursor(100,140); 
   tft.setTextColor(BLACK);
   tft.println("DELETED");
   tft.setCursor(60,190); 
   tft.setTextSize(3);
   tft.setTextColor(BLACK);
   tft.println("SUCCESSFULLY");
}

void main_menu(void)
{
   tft.fillScreen(PURPLE); 
   tft.setCursor(40,30); 
   tft.setTextSize(3);
   tft.setTextColor(WHITE);
   tft.println("SELECT OPTION");
   tft.setCursor(60,100); 
   tft.setTextSize(2);
   tft.setTextColor(WHITE);
   tft.println("A = ENROLL");
   tft.setCursor(60,150); 
   tft.setTextSize(2);
   tft.setTextColor(WHITE);
   tft.println("B = DELETE DB");
   tft.setCursor(60,200); 
   tft.setTextSize(2);
   tft.setTextColor(WHITE);
   tft.println("C = CANCEL");
 }
