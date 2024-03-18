#include <FirebaseESP32.h>
#include  <WiFi.h>
#include <LiquidCrystal_I2C.h>

#include "DHT.h"

#define FIREBASE_HOST "https://smartsilo-4115a-default-rtdb.firebaseio.com/"
#define WIFI_SSID "AndroidAPD9EE" // Change the name of your WIFI
#define WIFI_PASSWORD "Vedant@1712" // Change the password of your WIFI
#define FIREBASE_Authorization_key "olLWg0GODF6Y5IqQ8FgshqvknrkMUyR2B2W2V9UM"

#define DHTPIN1 14
#define DHTPIN2 27     
#define MQ1pin 32
#define MQ2pin 33
#define LED_GPIO   25
#define PWM1_Ch    0
#define PWM1_Res   8
#define PWM1_Freq  1000
#define DHTTYPE DHT22   

DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);

FirebaseData firebaseData1;
FirebaseJson json;

float h1,t1,h2,t2;
float MQ1value;
float MQ2value;
float tavg;
float MQAVG;
int PWM1_DutyCycle = 0;
int lcdColumns = 16;
int lcdRows = 4;
int buzzer=23;

// buzzer 23
//all 4 fans-25
//D14 D27 DHT
//loadcell data-D19 clk-D18
//SDA-21 SCL-22
//Buzzer 23
 
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 


void setup() {
  Serial.begin(9600);
  
  pinMode(MQ1pin,INPUT);
  pinMode(MQ2pin,INPUT);
  pinMode(buzzer,OUTPUT);
    // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
  
  Serial.println(F("DHTxx test!"));

  dht1.begin();
  dht2.begin();

  ledcAttachPin(LED_GPIO, PWM1_Ch);
  ledcSetup(PWM1_Ch, PWM1_Freq, PWM1_Res);
  
  WiFi.begin (WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Firebase.begin(FIREBASE_HOST, FIREBASE_Authorization_key);

  if (!Firebase.beginStream(firebaseData1, "Electriculture" ))
  {
    Serial.println("------------------------------------");
    Serial.println("Can't begin stream connection...");
    Serial.println("REASON: " + firebaseData1.errorReason());
    Serial.println("------------------------------------");
    Serial.println();

    Firebase.setStreamCallback(firebaseData1, streamCallback, streamTimeoutCallback);
  }
  else
  {
    Serial.println("Stream success");

    Firebase.setStreamCallback(firebaseData1, streamCallback, streamTimeoutCallback);

  }
  
}

void loop() {

  lcd.setCursor(4, 0);
  lcd.print("SIH 2022");
  lcd.setCursor(1,1);
  lcd.print("E");
  delay(200);
  lcd.setCursor(2,1);
  lcd.print("L");
  delay(200);
  lcd.setCursor(3,1);
  lcd.print("E");
  delay(200);
  lcd.setCursor(4,1);
  lcd.print("C");
  delay(200);
  lcd.setCursor(5,1);
  lcd.print("T");
  delay(200);
  lcd.setCursor(6,1);
  lcd.print("R");
  delay(200);
  lcd.setCursor(7,1);
  lcd.print("I");
  delay(200);
  lcd.setCursor(8,1);
  lcd.print("C");
  delay(200);
  lcd.setCursor(9,1);
  lcd.print("U");
  delay(200);
  lcd.setCursor(10,1);
  lcd.print("L");
  delay(200);
  lcd.setCursor(11,1);
  lcd.print("T");
  delay(200);
  lcd.setCursor(12,1);
  lcd.print("U");
  delay(200);
  lcd.setCursor(13,1);
  lcd.print("R");
  delay(200);
  lcd.setCursor(14,1);
  lcd.print("E");
  delay(200);
  lcd.setCursor(-2,2);
  lcd.print("PRESENTS");
  delay(200);
  lcd.setCursor(7,2);
  lcd.print(".");
  delay(200);
  lcd.setCursor(8,2);
  lcd.print(".");
  delay(200);
  lcd.setCursor(9,2);
  lcd.print(".");
  delay(1000);
  lcd.setCursor(-4,3);
  lcd.print("** SMART SILO **");
  delay(500);
  lcd.clear(); 


//DHT---------------


   h1 = dht1.readHumidity();
   t1 = dht1.readTemperature();
   h2 = dht2.readHumidity();
   t2 = dht2.readTemperature();

//  if (isnan(h1) || isnan(t1) || isnan(h2) || isnan(t2) ) {
//    Serial.println(F("Failed to read from DHT sensor!"));
//    return;
//  }

  Serial.print(F("Humidity 1 : "));
  Serial.print(h1);
  Serial.print(F("%  Temperature 1 : "));
  Serial.print(t1);
  Serial.print(F("°C "));
  Serial.print(F("Humidity 2 : "));
  Serial.print(h2);
  Serial.print(F("%  Temperature 2 : "));
  Serial.print(t2);
  Serial.println(F("°C "));
 

  Firebase.setFloat(firebaseData1, "/EFarm/SmartSilo/Humidity1", h1);
  Firebase.setFloat(firebaseData1, "/EFarm/SmartSilo/Temperature1", t1);
  Firebase.setFloat(firebaseData1, "/EFarm/SmartSilo/Humidity2", h2);
  Firebase.setFloat(firebaseData1, "/EFarm/SmartSilo/Temperature2", t2);
  Firebase.setFloat(firebaseData1, "/EFarm/SmartSilo/Fan", 1);
//Relay-------------

tavg=(t1+t2)/2;
Serial.println(tavg);

Serial.println("Fan Speed");

  if(26>=tavg){
   ledcWrite(PWM1_Ch, 50);
   Serial.println("50");
   digitalWrite(buzzer,LOW);
  }

  else if((26<tavg) && (tavg<28)){
    ledcWrite(PWM1_Ch, 150);
    Serial.println("150");
    digitalWrite(buzzer,LOW);
  }

  else if(tavg>=28){
    ledcWrite(PWM1_Ch, 255);
    Serial.println("255");
    digitalWrite(buzzer,HIGH);

  }

//MQ135-----------

  MQ1value = analogRead(MQ1pin);       
  Serial.print("MQ135 sensor no.1 : ");
  Serial.println(MQ1value, DEC);
  
  Firebase.setFloat(firebaseData1, "/EFarm/SmartSilo/MQ1value", MQ1value);

  MQ2value = analogRead(MQ2pin); 
  Serial.print("MQ135 sensor no.2 : ");
  Serial.println(MQ2value, DEC); 

  MQAVG = (MQ1value+MQ2value)/2;
  lcd.setCursor(0,0);
  lcd.print("NH3 value:");
  lcd.setCursor(11,0);
  lcd.print(MQAVG);
  lcd.setCursor(0,1);
  lcd.print("TEMP value:");
  lcd.setCursor(12,1);
  lcd.print(tavg);
  
  Firebase.setFloat(firebaseData1, "/EFarm/SmartSilo/MQ1value", MQ1value);
  Firebase.setFloat(firebaseData1, "/EFarm/SmartSilo/MQ2value", MQ2value);
  Firebase.setFloat(firebaseData1, "/EFarm/SmartSilo/MQavgvalue", MQAVG);

  
}


void streamCallback(StreamData data)
{
  Serial.println("Data path is ");
  Serial.println(data.dataPath());
  Serial.println("Idhar");
  Serial.println(data.dataType());
  
  if (data.dataType() == "string")
  {


    if (data.dataPath() == "/EFarm/SmartSilo/Fan")
    {
      if (data.stringData() == "1")
      {
        Serial.println("Fan ON");
           ledcWrite(PWM1_Ch, 255);
           Serial.println("255");
      }
      else if (data.stringData() == "0")
      {
        Serial.println("Fan OFF");
        ledcWrite(PWM1_Ch, 0);
        Serial.println("255");
        
      }
      delay(400);
    }

    else{
      Serial.println("No data recieved");
    }
    

    
   }
 }

 void streamTimeoutCallback(bool timeout)
{
  if (timeout)
  {
    Serial.println();
    Serial.println("Stream timeout, resume streaming...");
  }
}     

 
