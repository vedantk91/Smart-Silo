#include "DHT.h"

#define DHTPIN1 2    
#define DHTPIN2 4

#define DHTTYPE DHT22

DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);

float h1,h2,t1,t2,hic1,hic2;
void setup() {
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));

  dht1.begin();
  dht2.begin();
}

void loop() {
  
  delay(2000);

  float h1 = dht1.readHumidity();
  float t1 = dht1.readTemperature();

  float h2 = dht2.readHumidity();
  float t2 = dht2.readTemperature();

  if (isnan(h1) || isnan(t1) ) {
    Serial.println(F("Failed to read from DHT sensor 1!"));
    return;
  }

  if (isnan(h2) || isnan(t2)) {
    Serial.println(F("Failed to read from DHT sensor 2!"));
    return;
  }


  // Compute heat index in Celsius (isFahreheit = false)
  hic1 = dht1.computeHeatIndex(t1, h1, false);
  hic2 = dht2.computeHeatIndex(t2, h2, false);

  Serial.print(F("Humidity(1): "));
  Serial.print(h1);
  Serial.print(F("%  Temperature(1): "));
  Serial.print(t1);
  Serial.print(F("°C "));
  Serial.print(F(" Heat index(1): "));
  Serial.print(hic1);
  Serial.print(F("°C "));

   Serial.print(F("Humidity(2): "));
  Serial.print(h2);
  Serial.print(F("%  Temperature(2): "));
  Serial.print(t2);
  Serial.print(F("°C "));
  Serial.print(F(" Heat index(2): "));
  Serial.print(hic2);
  Serial.print(F("°C "));

}
