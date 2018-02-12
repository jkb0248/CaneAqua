#include <Wire.h>
#include "DS3231.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
RTClib RTC;

#define ONE_WIRE_BUS 13
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

int yr;
int mth;
int d;
int hr;
int m;
int s;

//timers
int current_hr;
int current_s;

//pump control
int pump_pin = 9;
int pump_state = 0;
unsigned long pump_start;
unsigned long pump_timer = 0;

//lights
int light_pin = 8;
int light_state = 0;
unsigned long light_start;

void setup () {
    Serial.begin(9600);
    Wire.begin();
    sensors.begin();
    lcd.init();
    // Print a message to the LCD.
    lcd.backlight();
    DateTime now = RTC.now();
    current_hr = now.hour() - 1; //during winter
    current_s = now.second();
    pinMode(light_pin,OUTPUT);
}

void loop () {
    lcd.setCursor(0,0);
    DateTime now = RTC.now();
    yr = now.year();
    mth = now.month();
    d = now.day();
    hr = now.hour() - 1; //during winter
    m = now.minute();
    s = now.second();
    
    Serial.print(yr, DEC);
    Serial.print('/');
    lcd.print(yr);
    lcd.print('/');
    
    Serial.print(mth, DEC);
    Serial.print('/');
    lcd.print(mth);
    lcd.print('/');
    
    Serial.print(d, DEC);
    Serial.print(' ');
    lcd.print(d);
    lcd.print(' ');
    
    Serial.print(hr, DEC);
    Serial.print(':');
    lcd.print(hr);
    lcd.print(':');
    
    Serial.print(m, DEC);
    Serial.print(':');
    if (m < 10) {
      lcd.print("0");
    }
    lcd.print(m);
    lcd.print(':');
    
    Serial.print(s, DEC);
    Serial.println();
    if (s < 10) {
      lcd.print("0");
    }
    lcd.print(s);

    lcd.setCursor(0,3);
    sensors.requestTemperatures();  
    lcd.print("T: ");
    float C = sensors.getTempCByIndex(0);
    lcd.print(C); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
    lcd.print(" C ");
    float F = (C * 1.8) + 32;
    lcd.print(F); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
    lcd.print(" F ");
    

    //Main!//
    if (hr < 13 && hr > 8 && light_state == 0){
       light_state = 1;
       light_start = hr;
    }

    //if ((hr - light_start) < 7){
    if (hr < 17){
       digitalWrite(light_pin, LOW);
    } else {
       digitalWrite(light_pin, HIGH);
       light_state = 0;
    }

    lcd.setCursor(0,2);
    lcd.print("lights: ");
    lcd.print(light_state);
    lcd.print(" ");
    lcd.print(hr - light_start);
    
    

    //if (current_hr != hr){
    if (m%20 == 0 && pump_state == 0){
      current_hr = hr;
      pump_state = 1;
      pump_start = millis();
    }

    pump_timer = millis() - pump_start;
    lcd.setCursor(0,1);
    lcd.print("pump: ");
    lcd.print(pump_state);
    lcd.print(" ");
    lcd.print(pump_timer);
    
    
    if (pump_state == 1 && pump_timer < 240000){
      digitalWrite(pump_pin,HIGH);
    } else {
      digitalWrite(pump_pin,LOW);
      pump_state = 0;
    }

    int button = digitalRead(6);
    if (button == 1) {
      digitalWrite(pump_pin,HIGH);
    } else if (button == 0 && pump_state == 0) {
      digitalWrite(pump_pin,LOW);
    }
    //delay(500);
    //if (current_s != s){
    if (s % 10 == 0){
      current_s = s;
      lcd.clear(); 
    }
}

