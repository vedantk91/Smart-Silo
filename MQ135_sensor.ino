#define MQ1pin 2
#define MQ2pin 4


float MQ1value;
float MQ2value;



void setup() {

  Serial.begin(9600);
  
  pinMode(MQ1pin,INPUT);
  pinMode(MQ2pin,INPUT);

}

void loop() {
  
  MQ1value = analogRead(MQ1pin);       
  Serial.print("MQ135 sensor no.1 : ");
  Serial.println(MQ1value, DEC);

  MQ2value = analogRead(MQ2pin);       
  Serial.print("MQ135 sensor no.2 : ");
  Serial.println(MQ2value, DEC);

  delay(1000);

}
