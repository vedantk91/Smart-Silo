#include <FirebaseESP32.h>
#include  <WiFi.h>

#include "DHT.h"

#define DHTPIN1 13
//#define DHTPIN1 15
     
#define DHTTYPE DHT22   
DHT dht(DHTPIN1, DHTTYPE);

#define FIREBASE_HOST "https://smartsilo-4115a-default-rtdb.firebaseio.com/"
#define WIFI_SSID "AndroidAPD9EE" // Change the name of your WIFI
#define WIFI_PASSWORD "Vedant@1712" // Change the password of your WIFI
#define FIREBASE_Authorization_key "olLWg0GODF6Y5IqQ8FgshqvknrkMUyR2B2W2V9UM"

FirebaseData firebaseData1;
FirebaseJson json;

float h1,t1;
int bottomfanpin=26;
int topfanpin=25;

void setup() {
  Serial.begin(9600);
  pinMode(bottomfanpin,OUTPUT);
  pinMode(topfanpin,OUTPUT);
  
  Serial.println(F("DHTxx test!"));

  dht.begin();
  
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

 
   h1 = dht.readHumidity();
   t1 = dht.readTemperature();

  if (isnan(h1) || isnan(t1) ) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }



  Serial.print(F("Humidity: "));
  Serial.print(h1);
  Serial.print(F("%  Temperature: "));
  Serial.print(t1);
  Serial.print(F("°C "));
 

  Firebase.setFloat(firebaseData1, "/EFarm/Humidity1", h1);
  Firebase.setFloat(firebaseData1, "/EFarm/Temperature1", t1);

  if(t1>=28){
   digitalWrite(topfanpin,LOW);
   digitalWrite(bottomfanpin,LOW);
  }

  else{
   digitalWrite(topfanpin,HIGH);
   digitalWrite(bottomfanpin,HIGH);
  }

}
