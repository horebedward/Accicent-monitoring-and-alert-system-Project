#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
SoftwareSerial mySerial(10, 11);

TinyGPS gps;
SoftwareSerial ss(4, 3);

int accX = A7;
int accY = A6;
int accZ = A5;

int sw1 = 6;
int sw2 = 7;

int buzzer = 5;

int xval;
int yval;
int zval;
int i;

float lat, lng;
static char lat1[8];
static char lng1[8];

static char message[100];

LiquidCrystal lcd(13, A0, A1, A2, A3, A4);

void setup()
{
  Serial.begin(115200);
  ss.begin(9600);

  lcd.begin(16, 2);

  pinMode(accX,INPUT);
  pinMode(accY,INPUT);
  pinMode(accZ,INPUT);

  pinMode(sw1,INPUT_PULLUP);
  pinMode(sw2,INPUT_PULLUP);

  pinMode(buzzer,OUTPUT);
}

void loop(){
  B: 
  
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (ss.available())
    {
      char c = ss.read();
      if (gps.encode(c))
        newData = true;
    }
  }

  if (newData)
  {
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);

     
    lcd.setCursor (0,0);
    lcd.print("LAT = ");
    lcd.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    lcd.print(" ");

    lcd.setCursor(0, 1);
    lcd.print("LON = ");
    lcd.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    lcd.print(" ");
    
    lat = flat;
    lng = flon;
  }

  dtostrf(lat,7, 6, lat1);
  dtostrf(lng,7, 6, lng1);

  xval = analogRead(accX)/4;
  yval = analogRead(accY)/4;
  zval = analogRead(accZ)/4;

  if (digitalRead(sw1)==LOW) {

    lcd.setCursor (0,0);
    lcd.print("ACC Calibrartion");
    
    EEPROM.write(1,xval);  
    EEPROM.write(2,yval);  
    EEPROM.write(3,zval ); 
    delay(1000);
    }

  if (abs(xval-EEPROM.read(1)) > 10 || abs(yval-EEPROM.read(2)) > 10 || abs(zval-EEPROM.read(3)) > 10 ){
    mySerial.begin(9600); 

    lcd.setCursor (0,0);
    lcd.print("  ARE YOU OKAY  ");
    i = 0;

    A:
    if (i < 1000) {
      digitalWrite(buzzer,HIGH);  delay(10);
      digitalWrite(buzzer,LOW);   delay(10);
      i++;

      if (digitalRead(sw2) == LOW) {     
      lcd.setCursor (0,0);
      lcd.print("  YOU ARE OKAY  ");

      lcd.setCursor (0,1);
      lcd.print("MESSAGE NOT SENT");
      delay (1000);
      goto B;
      }

      lcd.setCursor (0,1);
      lcd.print("RES. TIME = ");
      lcd.print(1000 - i);
     // mySerial.begin(9600);
      goto A;
    }

    else {
      mySerial.begin(9600);
      delay (1000);
      SendMessage();
      }
      }
  }

void SendMessage()
{
  mySerial.println("AT+CMGF=1");
  delay(1000);
  
  mySerial.println("AT+CMGS=\"+91xxxxxxxxxx\"\r");
  delay(1000);
  
  mySerial.println("Tom is in Trouble LAT = ");
  mySerial.println(lat);
  mySerial.println(" LNG = ");
  mySerial.println(lng);
  delay(100);
  
  mySerial.println((char)26);
  delay(1000);

  lcd.setCursor (0,0);
  lcd.print("  MESSAGE SENT  ");
  lcd.setCursor (0,1);
  lcd.print("  JERRY COMING  ");
  delay (1000);

  ss.begin(9600);
}
