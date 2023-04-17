#include <LiquidCrystal.h> //LCD Library
#include <DHT.h> //Temperature sensor library

#include <SPI.h> //RFID SPI Library
#include <MFRC522.h> //RFID Library

#define RST_PIN         49           // Reset pin of RFID
#define SS_PIN          53           //  SS pin of RFID
MFRC522 mfrc522(SS_PIN, RST_PIN);    // Create MFRC522 instance

#define DHTPIN 13                   // Temp sensor pin 
#define DHTTYPE DHT11               // DHT Type of Temp Sensor

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); //LCD Pins

int buzzer = 10;                    // Buzzer pin
int photoPin = A0;                  //Photoresistor pin
int light = 0;                      //Light Variable
int tiltSensor = 6;                 //Tilt sensor pin
int roomTemp = 20.10; // Room temperature to compare to
float threshold = 2; // Room temperature threshold
int redLight = 9;    // Red LED light that will blinks

const int trigPin = 18; //Ultrasonic trig pin
const int echoPin = 19; //Ultrasonic echo pin


void setup() {
  Serial.begin(9600); //Start serial connection
  pinMode(tiltSensor, INPUT); //set sensor as input/output
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(redLight, OUTPUT);
  dht.begin();
  lcd.begin(16, 2);
  pinMode(buzzer, OUTPUT);

  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
}



bool checkBadge() { //checks if the card is present
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
  Serial.print("Message : ");
  content.toUpperCase();

  if(content.substring(1) == "DF 24 91 06"){ //if a specific staff badge is presented then the system can be deactivated
    digitalWrite(redLight, HIGH);
    delay(500);
    digitalWrite(redLight, LOW);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Alarm");
    lcd.setCursor(0,1);
    lcd.print("Deactivated");
    digitalWrite(redLight, HIGH);   //the LED will blink when the alarm is off
    delay(500);
    digitalWrite(redLight, LOW);
    delay(500);
    digitalWrite(redLight, HIGH);
    delay(500);
    digitalWrite(redLight, LOW);
    delay(500);
    digitalWrite(redLight, HIGH);
    delay(500);
    digitalWrite(redLight, LOW);
    delay(500);
    digitalWrite(redLight, HIGH);
    delay(500);
    digitalWrite(redLight, LOW);
    delay(500);
    digitalWrite(redLight, HIGH);
    delay(500);
    digitalWrite(redLight, LOW);
    delay(500);
    
  }
  else{ //if badge not recognised
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Badge not");
    lcd.setCursor(0,1);
    lcd.print("Recognised");
    delay(2000);
  }
}


void loop() {
  if (checkBadge()) {
    // Deactivate alarm for 2 seconds
    digitalWrite(buzzer, LOW); //Turns off the buzzer
    delay(5000);
  }

  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.println(distance);

  if (distance <= 30) { //checks if the external object distance is < 30 cm
    
    tone(buzzer, 1000);
    lcd.setCursor(0,0);
    lcd.print("ext object");
    lcd.setCursor(0,1);
    lcd.print("Detected!");
    
  } else {
    lcd.clear();
    noTone(buzzer);
  

  //delay(500);


  float temperature = dht.readTemperature(); //Sensor reads the temp.
  light = analogRead(A0);
  Serial.println(digitalRead(tiltSensor)); //Prints to the serial monitor
  delay(100);

  Serial.println(temperature);
  Serial.println(light);

  if (digitalRead(tiltSensor) == HIGH) { //if the device is tilted
    digitalWrite(redLight, HIGH);
    tone(buzzer, 1000);
    lcd.setCursor(0, 0);
    lcd.print("Device Tilted!");
    delay(3000);
    lcd.clear();
    digitalWrite(redLight, LOW);
  }
  else if (temperature >= threshold + roomTemp) { //check temp if is high than the room temp of threshold target.
    tone(buzzer, 1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp increased!");
    lcd.setCursor(0, 1);
    lcd.print("temp: ");
    lcd.print(temperature);
    delay(3000);
    lcd.clear();
  } 
  else if (temperature <= roomTemp - threshold) { //check temp if is low than the room temp of threshold target.
    tone(buzzer, 1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp decreased!");
    lcd.setCursor(0, 1);
    lcd.print("temp: ");
    lcd.print(temperature);
    delay(3000);
    lcd.clear();
  } 
  else {
    noTone(buzzer);

    if (light > 160) {
      tone(buzzer, 1000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Flashlight");
      lcd.setCursor(0, 1);
      lcd.print("Detected!");
      delay(2000);
      lcd.clear();
    } 
    else {
      noTone(buzzer);
        lcd.setCursor(0, 0);
        lcd.print("Temp: ");
        lcd.print(temperature);
        lcd.print("C");
        delay(5000);
        lcd.clear();
      }
    }
  }
}
