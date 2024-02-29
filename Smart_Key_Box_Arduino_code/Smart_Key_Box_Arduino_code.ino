//RFID
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <FirebaseESP32.h>


const char* ssid = "HUAWEI"; //Wifi Name
const char* password = "12345678"; //Wifi Password



#define FIREBASE_HOST "HOST" //firebase connection
#define FIREBASE_AUTH "Auth" //authentication code

 
#define SS_PIN 5
#define RST_PIN 13
#define RELAY 27 //lampu biru


// Pin numbers for the IR sensors
const int sensor1Pin = 14; // Replace with your pin number
const int sensor2Pin = 16; // Replace with your pin number
const int sensor3Pin = 4; // Replace with your pin number
int rfidusernumber=0;
const int BUTTON_PIN = 34;

// Firebase reference object
FirebaseData firebaseData;



MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
 
void setup() 
{
  Serial.begin(230400);   // Initiate a serial communication

   // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to WiFi");

  // Initialize Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  
  // Set pinMode for IR sensors
  pinMode(sensor1Pin, INPUT);
  pinMode(sensor2Pin, INPUT);
  pinMode(sensor3Pin, INPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(RELAY, OUTPUT);
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522

     // Read sensor data
  int sensor1Value = digitalRead(sensor1Pin);
  int sensor2Value = digitalRead(sensor2Pin);
  int sensor3Value = digitalRead(sensor3Pin);
  
   uploadDataToFirebase(sensor1Value, sensor2Value, sensor3Value,rfidusernumber);

  
  
  digitalWrite(RELAY, HIGH);
  Serial.println("Put Card:");
  Serial.println();

}
void loop() 
{

   int buttonState = digitalRead(BUTTON_PIN);
  int sensor1Value = digitalRead(sensor1Pin);
  int sensor2Value = digitalRead(sensor2Pin);
  int sensor3Value = digitalRead(sensor3Pin);
  int rfidusernumber=0;

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("MSG : ");
  content.toUpperCase();
  if (content.substring(1) == "19 29 58 99") 
  {
    Serial.println("Authorized");
    Serial.println();
    rfidusernumber=100;


 
  digitalWrite(RELAY, LOW);

  while(buttonState!=HIGH)
  {
  buttonState = digitalRead(BUTTON_PIN);
   delay(100);
   Serial.print(".");
   }
  delay(200);
  digitalWrite(RELAY, HIGH);
  delay(1500);  
  sensor1Value = digitalRead(sensor1Pin);
  sensor2Value = digitalRead(sensor2Pin);
  sensor3Value = digitalRead(sensor3Pin);
  
  uploadDataToFirebase(sensor1Value, sensor2Value, sensor3Value,rfidusernumber);
     
  }
  
 
 else   {
    Serial.println("Denied");
    digitalWrite(RELAY, HIGH);
    delay(100);
      
    
  }
 

}

void uploadDataToFirebase(int value1, int value2, int value3,int value4) {

  value1 = !value1;
  value2 = !value2;
  value3 = !value3;

  // Prepare data object
  FirebaseJson jsonData;
  String message = " Available=1    Not Avaialable=0";
  jsonData.set("message", message);
  jsonData.set("Key No:-01", value1);
  jsonData.set("Key No:-02", value2);
  jsonData.set("Key No:-03", value3);
  jsonData.set("User ID NUmber:-", value4);

  // Upload data to Firebase
  if (Firebase.setJSON(firebaseData, "/IR sensor Readings", jsonData)) {
    Serial.println("Data sent to Firebase successfully!");
  } else {
    Serial.println("Failed to send data to Firebase.");
    Serial.println("Reason: " + firebaseData.errorReason());
  }
}

