#include <NewPing.h>
#include <Servo.h>
#include "HX711.h"
#include<SoftwareSerial.h>
#define DOUT 4
#define CLK 2
#define trig_pin1 6
#define echo_pin1 7
#define trig_pin2 8
#define echo_pin2 9
#define MAX_DISTANCE1 33
#define MAX_DISTANCE2 35
int dist1, dist2;
String text;

SoftwareSerial mySerial1(3, 5);
HX711 scale;

NewPing sonar1(trig_pin1, echo_pin1, MAX_DISTANCE1);
NewPing sonar2(trig_pin2, echo_pin2, MAX_DISTANCE2);

Servo myservo1;
Servo myservo2;

float calibration_factor = 175550;
float units, ounces;
int pos = 0;
int calibrationTime = 10;

int pirPin1 = 12;
int pirPin2 = 11;
int pirPos1 = 10;
int pirPos2 = 13;

void setup() {
  myservo1.attach(A1);
  myservo2.attach(A2);
  Serial.begin(9600);
  mySerial1.begin(9600);
  delay(5000);
  scale.begin(DOUT, CLK);
  scale.set_scale(calibration_factor);
  scale.tare();
  pinMode(pirPin1, INPUT);
  pinMode(pirPin2, INPUT);
  pinMode(pirPos1, OUTPUT);
  pinMode(pirPos2, OUTPUT);
  pinMode(7, INPUT);
  pinMode(9, INPUT);
  digitalWrite(pirPos1, HIGH);
  digitalWrite(pirPos2, HIGH);

  Serial.print("Calibrating sensor \n");
  for (int i = 0; i < calibrationTime; i++) {
    Serial.print(calibrationTime - i);
    Serial.print("-");
    delay(1000);
  }

  while (digitalRead(pirPin1) == HIGH) {
    delay(500);
    Serial.print(".");
  }

  while (digitalRead(pirPin2) == HIGH) {
    delay(500);
    Serial.print(".");
  }


}

void loop() {

  if (digitalRead(pirPin1) == HIGH) {


    for (pos = 0; pos < 100; pos += 1)
    {
      myservo1.write(pos);
      delay(15);
    }
    for (pos = 100; pos >= 1; pos -= 1)
    {
      myservo1.write(pos);
      delay(90);
    }

    delay(50);
    Serial.print("\n1=");
    dist1 = sonar1.ping_cm();
    Serial.print(dist1);
    delay(500);
    units = scale.get_units(), 10;
    if (units < 0)
    {
      units = 0.00;
    }
    ounces = units * 0.035274;
    Serial.print("\nP1=");
    Serial.print(units);
    delay(500);
    Serial.println();
  }

  if (digitalRead(pirPin2) == HIGH) {


    for (pos = 0; pos < 100; pos += 1)
    {
      myservo2.write(pos);
      delay(15);
    }
    for (pos = 100; pos >= 1; pos -= 1)
    {
      myservo2.write(pos);
      delay(90);
    }

    delay(50);
    Serial.print("\n2=");
    dist2 = sonar2.ping_cm();
    Serial.print(dist2);
    delay(500);
    Serial.println();

  }

  String INCOMING = "";
  boolean Stringready = false;

  while (mySerial1.available())
  {
    INCOMING = mySerial1.readString();
    Stringready = true;
  }

  if (Stringready)
  {
    if ((INCOMING.indexOf("BinOpen1", 0) > -1)) {
      Serial.println("Bin1 Opened");
      digitalWrite(pirPin1, HIGH);
      myservo1.write(60);
      delay(15000);
    }
    if ((INCOMING.indexOf("BinOpen2", 0) > -1)) {
      Serial.println("Bin2 Opened");
      digitalWrite(pirPin2, HIGH);
      myservo2.write(60);
      delay(15000);
    }
    if ((INCOMING.indexOf("bin/threshold/1", 0) > -1)) {
      Serial.println("Bin1 Full");
      digitalWrite(pirPin1, LOW);
      myservo1.write(0);
      delay(15000);
    }
    if ((INCOMING.indexOf("bin/threshold/2", 0) > -1)) {
      Serial.println("Bin2 Full");
      digitalWrite(pirPin2, LOW);
      myservo2.write(0);
      delay(15000);
    }
  }
  delay(1000);
}
