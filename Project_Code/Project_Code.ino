#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
Servo myservo; //ประกาศตัวแปรแทน Servo
LiquidCrystal_I2C lcd(0x27, 16, 2);

int trigger_outside = 6, trigger_inside = 11;
int echo_outside = 7, echo_inside = 12;
int state = 0, red = 2, green = 3, blue = 4;

void setup()
{
  Serial.begin(9600);
  lcd.init(); // เริ่มต้นหน้าจอ LCD
  lcd.backlight(); // เปิดไฟในหน้าจอ LCD

  pinMode(trigger_outside, OUTPUT);
  pinMode(echo_outside, INPUT);
  pinMode(trigger_inside, OUTPUT);
  pinMode(echo_inside, INPUT);

  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);

  myservo.attach(9); // กำหนดขา 9 ควบคุม Servo
  myservo.write(0);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SMACK MY TRASH");
  lcd.setCursor(0, 1);
  lcd.print("SMART BIN");
  
}

void open() {
  digitalWrite(red, HIGH);
  digitalWrite(green, LOW);
  digitalWrite(blue, HIGH);
  
  lcd.clear();
  lcd.setCursor(6, 0);
  lcd.print("OPEN");
  myservo.write(90); // สั่งให้ Servo หมุนไปองศาที่ 90
  delay(1000);

  lcd.setCursor(1, 1);
  lcd.print("CLOSING IN..");

  for (int i = 5; i > 0; i--) {
    Serial.print("Closing in ");
    Serial.println(i);
    lcd.setCursor(14, 1);
    lcd.print(i);
    delay(1000);
  }
}

void close() {
  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("CLOSE");

  for (int j = 0; j < 40; j++) {
    digitalWrite(green, LOW);
    delay(25);
    digitalWrite(green, HIGH);
    delay(25);
  }
  
  myservo.write(0); // สั่งให้ Servo หมุนไปองศาที่ 90
  delay(1000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SMACK MY TRASH");
  lcd.setCursor(0, 1);
  lcd.print("SMART BIN");

}

int check_full() {
  digitalWrite(trigger_inside, LOW);
  delay(25);
  digitalWrite(trigger_inside, HIGH);
  delay(25);
  digitalWrite(trigger_inside, LOW);

  int pulseWidth_inside = pulseIn(echo_inside, HIGH);

  if (pulseWidth_inside >= 100 && pulseWidth_inside <= 500) {
    return 1;
  }
  else {
    return 0;
  }
}

void full() {
  lcd.clear();

  lcd.setCursor(3, 0);
  lcd.print("!! FULL !!");
  lcd.setCursor(1, 1);
  lcd.print("CLEAR YOUR BIN");

  digitalWrite(green, HIGH);
  digitalWrite(blue, HIGH);

  for (int i = 0; i < 30; i++) {
    digitalWrite(red, LOW);
    delay(500);
    digitalWrite(red, HIGH);
    delay(500);
  }

  state = 1;
  
}

void loop() {
  // ตัววัดด้านนอก
  digitalWrite(trigger_outside, LOW);
  delay(25);
  digitalWrite(trigger_outside, HIGH);
  delay(25);
  digitalWrite(trigger_outside, LOW);

  int pulseWidth = pulseIn(echo_outside, HIGH);
  long distance = pulseWidth/29/2;

  // เช็คถังเต็ม
  if (check_full()) {
    full();
  }
  else if (distance >= 0.0 && distance <= 10.0) {
    open();
    if (check_full()) {
      full();
      loop();
    }
    close();
  }
  else {
    if (state) {
      state = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SMACK MY TRASH");
      lcd.setCursor(0, 1);
      lcd.print("SMART BIN");
    }
    digitalWrite(red, HIGH);
    digitalWrite(green, HIGH);
    digitalWrite(blue, LOW);
  }
}
