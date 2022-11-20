/*
 * Author:Jihan Alam
 * ID:16301199
*/
#include <SimpleDHT.h>
#include <SPI.h>
#include <MFRC522.h>
#define RST_PIN         9 
#define SS_PIN          10  
#define NR_KNOWN_KEYS   8
#include <SoftwareSerial.h>
String number="+8801761978754";
SoftwareSerial mySerial(6, 5);
int pinDHT11 = 2;
SimpleDHT11 dht11(pinDHT11);
MFRC522 mfrc522(SS_PIN, RST_PIN);
String foodId[20];
String foodName[20];
int timeLeft[20];
String willExpire;
String times;
int foodCounter=0;
int next=0;
int toRemove=0;
boolean aExist=false;
boolean msgFlag=false;
byte temperature = 0;
byte humidity = 0;
int clockPin = 3;
void setup() {
    Serial.begin(9600);         
    while (!Serial);            
    SPI.begin();                
    mfrc522.PCD_Init();         
    pinMode(clockPin,INPUT);
    Serial.println(F("Scan to Start."));
    mySerial.begin(9600);
//    while (!Serial) {
//    }
}
String retName(String s){
  if(s=="94831CBA"){
    return "Milk";
    }
    else if(s=="41DFC20"){
    return "Chicken Sausage";
    }
    else if(s=="31F27C20"){
    return "Bread";
    }
    else if(s=="316B8320"){
    return "Pasta";
    }
    else if(s=="31DF3920"){
    return "Fish";
    }
    else if(s=="4115EE20"){
    return "Beef Steak";
    }
}
void addFood(String s){
    foodId[next]=s;
    foodName[next]=retName(s);
    timeLeft[next]=6;
    foodCounter++;
    next++;
}
void removeFood(String s){
  if(foodId[toRemove]==s){
    foodId[toRemove]="";
    leftShift();
    aExist=false;
    foodCounter--;
    next--;
  }
}
void leftShift(){
  for(int i=toRemove;i<20;i++){
    foodId[i]=foodId[i+1];
    foodName[i]=foodName[i+1];
    timeLeft[i]=timeLeft[i+1];
  }
  toRemove=0;
}
void checkAddFood(String s){
    if(next<20){    
        if(foodCounter==0){
          addFood(s);
          Serial.println("CAD0");
          }
        else if(foodCounter!=0){
            for(int i=0;i<=foodCounter-1;i++){
              if(foodId[i]==s){
                aExist=true;
                toRemove=i;
              }
            }
            if(aExist==true){
              removeFood(s);
              Serial.println("CADR1");
            }
            else{
              addFood(s);
              Serial.println("CAD2");
            }
        }
    }
    else{
        Serial.println("Fridge is full try again later");
    }    
}
void readFood(byte *buffer, byte bufferSize) {
    String s= "";
    for (byte i = 0; i < bufferSize; i++) {
        s+=String(buffer[i],HEX); 
    }
    s.toUpperCase();
    checkAddFood(s);
    delay(2000);
}
void sendMassage(String msg,String t){
  delay(10000);
  mySerial.println("AT+CMGF=1");
  delay(1000);
  mySerial.println("AT+CMGS=\"+8801761978754\"\r"); //replace x by your number
  delay(1000);
  mySerial.println(msg+"will expire within "+t+"days respectively. Current temprature is "+temperature+" Degree C");
  delay(100);
  mySerial.println((char)26);
  delay(1000);
  Serial.println("Sending Massage.");
}
void checkSendMassage(){
  dht11.read(&temperature, &humidity, NULL);
  for(int i=0;i<foodCounter;i++){
    if(timeLeft[i]!=0){
        timeLeft[i]=timeLeft[i]-1;
      }
  }
  for(int i=0;i<foodCounter;i++){
    Serial.println(foodName[i]+" has "+String(timeLeft[i])+" days left to get expired.");
  }
  for(int i=0;i<foodCounter;i++){
    if(timeLeft[i]<=2){
        msgFlag=true;
        willExpire+=foodName[i]+", ";
        times+=String(timeLeft[i])+ ", ";
        Serial.println("Food going to be expired soon.");
      } 
  }
  if(msgFlag==true){
    sendMassage(willExpire,times);
    willExpire="";
    times="";
    msgFlag=false;
    Serial.println("Sent.");
  }
}
void loop() {
    // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
    if(digitalRead(clockPin)==HIGH){
      Serial.println("A Clock Cycle has been detected!");
      checkSendMassage();
      delay(500);
    }
    if ( ! mfrc522.PICC_IsNewCardPresent())
        return;
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;
        
    readFood(mfrc522.uid.uidByte, mfrc522.uid.size);
    
    for(int i=0;i<foodCounter;i++){
      Serial.println(foodName[i]);
    }
    char buffer[100];
    sprintf(buffer,"Number of food stored %d",foodCounter);
    Serial.println(buffer);
}
