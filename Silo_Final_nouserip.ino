#include <FirebaseESP32.h>
#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <HX711_ADC.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

//pins:
const int HX711_dout = 19; //mcu > HX711 dout pin
const int HX711_sck = 18; //mcu > HX711 sck pin

//HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);

const int calVal_calVal_eepromAdress = 0;
unsigned long t = 0;

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

float h1, t1, h2, t2;
float MQ1value;
float MQ2value;
float tavg;
float MQAVG;
int PWM1_DutyCycle = 0;
int lcdColumns = 16;
int lcdRows = 4;
int buzzer = 23;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);


void setup() {
  Serial.begin(9600);

  pinMode(MQ1pin, INPUT);
  pinMode(MQ2pin, INPUT);
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
  }
  else
  {
    Serial.println("Stream success");
  }

  float calibrationValue; // calibration value
  calibrationValue = 696.0; // uncomment this if you want to set this value in the sketch
#if defined(ESP8266) || defined(ESP32)
  //EEPROM.begin(512); // uncomment this if you use ESP8266 and want to fetch this value from eeprom
#endif
  //EEPROM.get(calVal_eepromAdress, calibrationValue); // uncomment this if you want to fetch this value from eeprom

  LoadCell.begin();
  //LoadCell.setReverseOutput();
  unsigned long stabilizingtime = 2000; // tare preciscion can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
  }
  else {
    LoadCell.setCalFactor(calibrationValue); // set calibration factor (float)
    Serial.println("Startup is complete");
  }
  while (!LoadCell.update());
  Serial.print("Calibration value: ");
  Serial.println(LoadCell.getCalFactor());
  Serial.print("HX711 measured conversion time ms: ");
  Serial.println(LoadCell.getConversionTime());
  Serial.print("HX711 measured sampling rate HZ: ");
  Serial.println(LoadCell.getSPS());
  Serial.print("HX711 measured settlingtime ms: ");
  Serial.println(LoadCell.getSettlingTime());
  Serial.println("Note that the settling time may increase significantly if you use delay() in your sketch!");
  if (LoadCell.getSPS() < 7) {
    Serial.println("!!Sampling rate is lower than specification, check MCU>HX711 wiring and pin designations");
  }
  else if (LoadCell.getSPS() > 100) {
    Serial.println("!!Sampling rate is higher than specification, check MCU>HX711 wiring and pin designations");
  }
}

void loop() {

  lcd.setCursor(4, 0);
  lcd.print("SIH 2022");
  lcd.setCursor(1, 1);
  lcd.print("E");
  delay(200);
  lcd.setCursor(2, 1);
  lcd.print("L");
  delay(200);
  lcd.setCursor(3, 1);
  lcd.print("E");
  delay(200);
  lcd.setCursor(4, 1);
  lcd.print("C");
  delay(200);
  lcd.setCursor(5, 1);
  lcd.print("T");
  delay(200);
  lcd.setCursor(6, 1);
  lcd.print("R");
  delay(200);
  lcd.setCursor(7, 1);
  lcd.print("I");
  delay(200);
  lcd.setCursor(8, 1);
  lcd.print("C");
  delay(200);
  lcd.setCursor(9, 1);
  lcd.print("U");
  delay(200);
  lcd.setCursor(10, 1);
  lcd.print("L");
  delay(200);
  lcd.setCursor(11, 1);
  lcd.print("T");
  delay(200);
  lcd.setCursor(12, 1);
  lcd.print("U");
  delay(200);
  lcd.setCursor(13, 1);
  lcd.print("R");
  delay(200);
  lcd.setCursor(14, 1);
  lcd.print("E");
  delay(200);
  lcd.setCursor(-2, 2);
  lcd.print("PRESENTS");
  delay(200);
  lcd.setCursor(7, 2);
  lcd.print(".");
  delay(200);
  lcd.setCursor(8, 2);
  lcd.print(".");
  delay(200);
  lcd.setCursor(9, 2);
  lcd.print(".");
  delay(1000);
  lcd.setCursor(-4, 3);
  lcd.print("* SMART SILO *");
  delay(500);
  lcd.clear();


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
  Serial.println(F("°C "));


  Firebase.setFloat(firebaseData1, "/EFarm/SmartSilo/Humidity1", h1);
  Firebase.setFloat(firebaseData1, "/EFarm/SmartSilo/Temperature1", t1);
  Firebase.setFloat(firebaseData1, "/EFarm/SmartSilo/Humidity2", h2);
  Firebase.setFloat(firebaseData1, "/EFarm/SmartSilo/Temperature2", t2);

  //Relay-------------

  tavg = (t1 + t2) / 2;
  Serial.println(tavg);

  Serial.println("Fan Speed");

  if (26 >= tavg) {
    ledcWrite(PWM1_Ch, 50);
    Serial.println("50");
    digitalWrite(buzzer,LOW);
  }

  else if ((26 < tavg) && (tavg < 28)) {
    ledcWrite(PWM1_Ch, 150);
    Serial.println("150");
    digitalWrite(buzzer,LOW);
  }

  else if (tavg >= 28) {
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

  MQAVG = (MQ1value + MQ2value) / 2;
  lcd.setCursor(0, 0);
  lcd.print("NH3 value:");
  lcd.setCursor(11, 0);
  lcd.print(MQAVG);
  lcd.setCursor(0, 1);
  lcd.print("TEMP value:");
  lcd.setCursor(12, 1);
  lcd.print(tavg);

  Firebase.setFloat(firebaseData1, "/EFarm/SmartSilo/MQ1value", MQ1value);
  Firebase.setFloat(firebaseData1, "/EFarm/SmartSilo/MQ2value", MQ2value);
  Firebase.setFloat(firebaseData1, "/EFarm/SmartSilo/MQavgvalue", MQAVG);

  static boolean newDataReady = 0;
  const int serialPrintInterval = 2000; //increase value to slow down serial print activity

  // check for new data/start next conversion:
  if (LoadCell.update()) newDataReady = true;

  // get smoothed value from the dataset:
  if (newDataReady) {
    if (millis() > t + serialPrintInterval) {
      float i = LoadCell.getData();
      Serial.print("Load_cell output val: ");
      Serial.println(i);
      Firebase.setFloat(firebaseData1, "/EFarm/SmartSilo/Loadcell", i);
      newDataReady = 0;
      t = millis();
    }
  }

  // receive command from serial terminal, send 't' to initiate tare operation:
  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') LoadCell.tareNoDelay();
  }

  // check if last tare operation is complete:
  if (LoadCell.getTareStatus() == true) {
    Serial.println("Tare complete");
  }



}
