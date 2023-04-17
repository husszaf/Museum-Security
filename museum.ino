#include <LiquidCrystal.h>
#include <DHT.h>

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         49           // Configurable, see typical pin layout above
#define SS_PIN          53          // Configurable, see typical pin layout above
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

#define DHTPIN 13
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int buzzer = 10;
int photoPin = A0;
int light = 0;
int tiltSensor = 6;
int roomTemp = 23.10; // Room temperature to compare to
float threshold = 2; // Room temperature threshold

const int trigPin = 18;
const int echoPin = 19;


void setup() {
  Serial.begin(9600);
  pinMode(tiltSensor, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  dht.begin();
  lcd.begin(16, 2);
  pinMode(buzzer, OUTPUT);

  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
}

void loop() {

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

  if (distance <= 30) {
    
    tone(buzzer, 1000);
    lcd.setCursor(0,0);
    lcd.print("ext object");
    lcd.setCursor(0,1);
    lcd.print("Detected!");
    
  } else {
    lcd.clear();
    noTone(buzzer);
  

  //delay(500);


  float temperature = dht.readTemperature();
  light = analogRead(A0);
  Serial.println(digitalRead(tiltSensor));
  delay(100);

  Serial.println(temperature);
  Serial.println(light);

  if (digitalRead(tiltSensor) == HIGH) {
    tone(buzzer, 1000);
    lcd.setCursor(0, 0);
    lcd.print("Device Tilted!");
    delay(3000);
    lcd.clear();
  } 
  else if (temperature >= threshold + roomTemp) {
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
  else if (temperature <= roomTemp - threshold) {
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