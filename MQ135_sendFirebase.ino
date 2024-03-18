#include <FirebaseESP32.h>
#include  <WiFi.h>

#define MQ1pin 32
#define MQ2pin 33
#define FIREBASE_HOST "https://smartsilo-4115a-default-rtdb.firebaseio.com/"
#define WIFI_SSID "AndroidAPD9EE" // Change the name of your WIFI
#define WIFI_PASSWORD "Vedant@1712" // Change the password of your WIFI
#define FIREBASE_Authorization_key "olLWg0GODF6Y5IqQ8FgshqvknrkMUyR2B2W2V9UM"

float MQ1value;
float MQ2value;

FirebaseData firebaseData1;
FirebaseJson json;



void setup() {
  
  pinMode(MQ1pin,INPUT);
  pinMode(MQ2pin,INPUT);

  Serial.begin(9600);
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


  MQ1value = analogRead(MQ1pin);       
  Serial.print("MQ135 sensor no.1 : ");
  Serial.println(MQ1value, DEC);
  Firebase.setFloat(firebaseData1, "/EFarm/MQ1value", MQ1value);

  MQ2value = analogRead(MQ2pin);       
  Serial.print("MQ135 sensor no.2 : ");
  Serial.println(MQ2value, DEC);
  Firebase.setFloat(firebaseData1, "/EFarm/MQ2value", MQ2value);
  


  delay(1000);

}
