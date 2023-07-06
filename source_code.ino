#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP32Servo.h>
//OLED
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "thingProperties.h"
#define ONE_WIRE_BUS 5
#define RELAY_PIN1 27 // heater
#define RELAY_PIN2 19 // fan
CHAPTER 3
SOURCE CODE
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

//Water Level
#define POWER_PIN 17 
#define SIGNAL_PIN 36 
#define min 0
#define max 1023
const int RELAY_PIN3 = 2; //motor
int value = 0;
int level = 0;
//Servo motor for Fish feeding
Servo myservo;
int pos = 0;
int servoPin = 4;
// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
// Declaration for SSD1306 display connected using I2C
#define OLED_RESET -1 // Reset pin
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH,SCREEN_HEIGHT,&Wire,OLED_RESET);
// LDR
#define ldrpin 13
int ldr_value;
const int LED_PIN = 14;
void setup() {
Serial.begin(9600);
pinMode(ONE_WIRE_BUS,INPUT);
pinMode(SIGNAL_PIN , INPUT);
pinMode(ldrpin , INPUT);
pinMode(RELAY_PIN1, OUTPUT); // Temp
pinMode(RELAY_PIN2, OUTPUT); // Fan
pinMode(RELAY_PIN3, OUTPUT); // Pump
sensors.begin();
//water level
pinMode(POWER_PIN, OUTPUT);
digitalWrite(POWER_PIN, LOW);
//servo motor
ESP32PWM::allocateTimer(0);
myservo.setPeriodHertz(50); // standard 50 hz servo
myservo.attach(servoPin, 500, 2400);
//initialize oled obj
if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
Serial.println(F("SSD1306 allocation failed"));
for(;;); // Don't proceed, loop forever
}
display.clearDisplay();
// Defined in thingProperties.h
initProperties();
// Connect to Arduino IoT Cloud
ArduinoCloud.begin(ArduinoIoTPreferredConnection);
setDebugMessageLevel(2);
ArduinoCloud.printDebugInfo();
}
void loop() {
ArduinoCloud.update();
//Temperature
sensors.requestTemperatures();
Serial.print("Celsius temperature: ");
temperature = sensors.getTempCByIndex(0);
Serial.print(temperature);
Serial.print("\n");
// Heater Actuation
if (temperature <=20 ){
Serial.println("Heater ON");
digitalWrite(RELAY_PIN1, HIGH);
delay(5000);
digitalWrite(RELAY_PIN1, LOW);
}
elseif(temperature>23){
Serial.println("Fan ON");
digitalWrite(RELAY_PIN2, HIGH);
delay(5000);
digitalWrite(RELAY_PIN2, LOW);
}
else {
Serial.println("Heater OFF ");
digitalWrite(RELAY_PIN1, LOW);
Serial.println("Fan OFF ");
digitalWrite(RELAY_PIN2, LOW);
delay(4000);
}
//water level
digitalWrite(POWER_PIN, HIGH); // turn the sensor ON
delay(100); /
value = analogRead(SIGNAL_PIN); // read the analog value from sensor
Serial.print("analog Water level: ");
Serial.println(level);
digitalWrite(POWER_PIN, LOW); // turn the sensor OFF
level = map(value, min, max , 0, 4);
Serial.print("Mapped Water level: ");
Serial.println(level);
delay(1000);
if (level <= 3) {
// Turn on the relay to activate the pump
digitalWrite(RELAY_PIN3, HIGH);
Serial.println("Motor is on");
}
else {
// Turn off the relay
digitalWrite(RELAY_PIN3, LOW);
Serial.println("Motor is off");
}
// Wait for some time before checking the water level again
delay(1000);
// oled display
// display.setCursor(30,40);
display.setCursor(0,0);
display.setTextSize(1.5);
display.setTextColor(WHITE);
display.println("Temperature :");
display.setTextSize(2);
display.println(temperature);
display.setTextSize(1.5);
display.println("WaterLevel:");
display.println(level);
display.display();
delay(250);
//ldr
ldr_value = analogRead(ldrpin); // Read analog value from LDR sensor
// Serial.println(ldr_value); // Print LDR value to the serial monitor
if (ldr_value < 500) { // If LDR value is less than 500
digitalWrite(LED_PIN, HIGH); // Turn on LED
}
else {
digitalWrite(LED_PIN, LOW); // Turn off LED
}
delay(1000);
//servo motor
for (pos = 0; pos <= 90; pos += 1)
{ // goes from 0 degrees to 90 degrees
myservo.write(pos); // tell servo to go to position in variable 'pos'
delay(15); // waits 15ms for the servo to reach the position
}
for (pos = 90; pos >= 0; pos -= 1)
{ // goes from 90 degrees to 0 degrees
myservo.write(pos); // tell servo to go to position in variable 'pos'
delay(30); // waits 30ms for the servo to reach the position
}
myservo.detach();
message="Temperature = " + String (temperature) + "\n Water Level = " + String(level);
delay(1000);
}
void onTemperatureChange() {}
void onMessageChange() {}