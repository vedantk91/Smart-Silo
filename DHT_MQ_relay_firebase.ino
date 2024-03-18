#include <FirebaseESP32.h>
#include  <WiFi.h>

#include "DHT.h"

#define FIREBASE_HOST "https://smartsilo-4115a-default-rtdb.firebaseio.com/"
#define WIFI_SSID "AndroidAPD9EE" // Change the name of your WIFI
#define WIFI_PASSWORD "Vedant@1712" // Change the password of your WIFI
#define FIREBASE_Authorization_key "olLWg0GODF6Y5IqQ8FgshqvknrkMUyR2B2W2V9UM"
#define DHTPIN1 13
#define DHTPIN2 15     
#define MQ1pin 32
#define MQ2pin 33

#define DHTTYPE DHT22   

DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);

FirebaseData firebaseData1;
FirebaseJson json;

float h1,t1,h2,t2;
int bottomfanpin=26;
int topfanpin=25;
float MQ1value;
float MQ2value;
float tavg;

void setup() {
  Serial.begin(9600);
  
  pinMode(bottomfanpin,OUTPUT);
  pinMode(topfanpin,OUTPUT);
  pinMode(MQ1pin,INPUT);
  pinMode(MQ2pin,INPUT);
  
  Serial.println(F("DHTxx test!"));

  dht1.begin();
  dht2.begin();
  
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
  }
  else
  {
    Serial.println("Stream success");
  }
}

void loop() {
  //delay(2000);

//DHT---------------


   h1 = dht1.readHumidity();
   t1 = dht1.readTemperature();
   h2 = dht2.readHumidity();
   t2 = dht2.readTemperature();

  if (isnan(h1) || isnan(t1) || isnan(h2) || isnan(t2) ) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humidity 1 : "));
  Serial.print(h1);
  Serial.print(F("%  Temperature 1 : "));
  Serial.print(t1);
  Serial.print(F("°C "));
  Serial.print(F("Humidity 2 : "));
  Serial.print(h2);
  Serial.print(F("%  Temperature 2 : "));
  Serial.print(t2);
  Serial.print(F("°C "));
 

  Firebase.setFloat(firebaseData1, "/EFarm/Humidity1", h1);
  Firebase.setFloat(firebaseData1, "/EFarm/Temperature1", t1);
  Firebase.setFloat(firebaseData1, "/EFarm/Humidity2", h2);
  Firebase.setFloat(firebaseData1, "/EFarm/Temperature2", t2);

//Relay-------------

tavg=(t1+t2)/2;
Serial.println(tavg);

  if(tavg>=28){
   digitalWrite(topfanpin,LOW);
   digitalWrite(bottomfanpin,LOW);
  }

  else{
   digitalWrite(topfanpin,HIGH);
   digitalWrite(bottomfanpin,HIGH);
  }

//MQ135-----------

  MQ1value = analogRead(MQ1pin);       
  Serial.print("MQ135 sensor no.1 : ");
  Serial.println(MQ1value, DEC);
  
  Firebase.setFloat(firebaseData1, "/EFarm/MQ1value", MQ1value);

  MQ2value = analogRead(MQ2pin);       
  Serial.print("MQ135 sensor no.2 : ");
  Serial.println(MQ2value, DEC);
  
  Firebase.setFloat(firebaseData1, "/EFarm/MQ2value", MQ2value);
  

}
