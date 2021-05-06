/*
 * Receiver
 * Receives signal from microcontroller located on mile marker.
 * Signals include "Clear" and "Hazard"
 * From these signals/lack of signal, there are 3 states:
 *  1. "Clear" - LEDs flash white on a 2.5s period; 0.5s on, 2s off
 *  2. "Danger" = LEDs flash blue
 *  3. No signal received after certain time period - LEDs flash yellow
 * All LEDs are RGB and can be programmed to any color.
 * Road conditions are saved to non-volatile memory and updated every time a signal is received.
 */

/* Setup
 * While pin headers are closest to you
 * From HC12 to Seeduino
 * Pin 1 to Power
 * Pin 2 to GND
 * Pin 3 to pin 6
 * pin 4 to pin 7
 * Pin 5 open or Power if transmit receive (GND if configuring)
 */
#include <SoftwareSerial.h>
#include<FlashAsEEPROM.h>

//LEDs on one side of solar stud
#define RED 2
#define GREEN 1
#define BLUE 0

//LEDs on other side of stud, this is just blinking orange for now
#define RED2  9
#define GREEN2  8
#define BLUE2 3

//codes for road conditions
#define CLEAR 100
#define DANGER 101  //this is not used
#define HAZARD 102

//LED on/off times
#define on_time_ms 500
#define off_time_ms 2000
#define maxTimeBetweenSignalSeconds 5 //time before caution signal is displayed

SoftwareSerial HC12(7,6); //HC12 TX Pin, HC12 RX pin

byte incomingByte;
String currentColor = "";
int currentTime = 0;
int resetTime = 0;

void setup(){
  Serial.begin(9600);
  HC12.begin(9600);
  while (!Serial && !HC12) {
    Serial.println("Waiting..."); // wait for serial port to connect. Needed for native USB port only
  }  
  pinMode(2,OUTPUT);

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT); 
//  Serial.write("AT+FU4,B1200"); //long range mode, setpin must be low
}

void loop()
{
  currentTime = millis()/1000; //get how long program has run (in seconds)
  delay(100);
  
  String sig = recieveData();
  checkSignal(sig);     //determine road conditions
  sendDataFromSerial(); //send data if anything was typed into serial monitor
  flashLED();           //blink the LEDs based on road conditions

  //determine if it's been too long since last transmission
  if(currentTime - resetTime > maxTimeBetweenSignalSeconds)
  {
    Serial.println("Too much time between signal from master");
    //Do stuff
    changeColor("yellow");
    currentColor = "yellow";
    /*
     * 
     */
  }
}

String recieveData(void)
{
  String data = "";
  //receiving data
  while (HC12.available()) //if HC-12 has recieved
  {     
    incomingByte = HC12.read(); //read next byte
//    Serial.println(char(incomingByte));
    if(char(incomingByte) != '\n' && char(incomingByte) != '\r')
    {
      data += char(incomingByte);  //concatenate to 'data'
    }
  }
  return data; 
}

void sendDataFromSerial(void)
{
  //sending data
  while(Serial.available())//if we have inputted data to serial monitor
  {    
    HC12.write(Serial.read());  //send data to other HC12
  }  
}
/* change LEDs based on RGB values
 *  analogWrite on Seeeduino is bugged as some pins are tied together
 * this implementation assumes that blue value is either 0 or 255.
 * if blue value is 255, you must use digitalWrite(BLUE, HIGH)
 */
void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
{
  analogWrite(RED, red_light_value);
  analogWrite(GREEN, green_light_value);
  digitalWrite(BLUE, LOW);
}

void RGB_color2(int red_light_value, int green_light_value, int blue_light_value)
{
  analogWrite(RED2, red_light_value);
  analogWrite(GREEN2, green_light_value);
  digitalWrite(BLUE2, LOW);
}

/*
 * you can change the colors of the leds by sending one of these strings to the serial of a transmitter
 * colors:
 *  red
 *  green
 *  blue
 *  white
 *  yellow
 */
void changeColor(String color)
{
  if(color == "red")
  {
    RGB_color(255, 0, 0);
  }
  else if(color == "green")
  {
    RGB_color(0, 255, 0);
  }
  else if(color == "blue")
  {
    RGB_color(0,0,0);
    digitalWrite(BLUE, HIGH);
  }
  else if(color == "yellow")
  {
    RGB_color(255, 175, 0);
  }
  else if(color == "white")
  {
    RGB_color(255, 255, 0);
    digitalWrite(BLUE, HIGH);
  }
  else
  {
    RGB_color(0, 0, 0);
  }
}

/*
 * checks signal from transmitter and saves it to flash memory
 * since flash memory has limited number of writes, it first checks if the current state is already saved
 */
void checkSignal(String words)
{
  //check to see if there is non-empty transmission
  if (words != "")
  {
    resetTime = millis()/1000;    
    changeColor(words);
    currentColor = words;
    Serial.println(words);
  }   
  if(words == "Clear")
  {
    resetTime = millis()/1000;  //reset timer to current time
    changeColor("white");
    currentColor = "white";
    if(EEPROM.read(0) != CLEAR)//check to see if condition is already saved in memory
    {
      Serial.println("Writing to 100 flash memory");
      Serial.printf("Reset timer to: %d s\n", resetTime);    
      EEPROM.write(0, CLEAR);
      EEPROM.commit();
    }
    else
    {
      Serial.println("Clear signal already present on EEPROM");
      Serial.println("Timer reset");      
    }
  }
  if(words == "Danger") //this condition is not used
  {
    resetTime = millis()/1000;  //reset timer to current time
    changeColor("red");
    currentColor = "red";
    if(EEPROM.read(0) != DANGER) //check to see if condition is already saved in memory
    {
      Serial.println("Writing to 101 flash memory");
      Serial.printf("Reset timer to: %d s\n", resetTime);    
      EEPROM.write(0, DANGER);
      EEPROM.commit();
    }
    else
    {
      Serial.println("Danger signal already present on EEPROM");
      Serial.println("Timer reset");      
    }

  }
  else if(words == "Hazard")
  {
    resetTime = millis()/1000;  //reset timer to current time
    changeColor("blue");
    currentColor = "blue"; 
    if(EEPROM.read(0) != HAZARD)  //check to see if condition is already saved in memory
    {
      Serial.println("Writing to 102 flash memory");
      Serial.printf("Reset timer to: %d s\n", resetTime);
      EEPROM.write(0, HAZARD);
      EEPROM.commit();  
    }
    else
    {
      Serial.println("Hazard signal already present on EEPROM");
      Serial.println("Timer reset");
    }
     
  }
}
/*
 * flashses leds
 */
void flashLED(void)
{
  delay(on_time_ms);
  RGB_color(0, 0, 0); 
  RGB_color2(0, 0, 0); 
  delay(off_time_ms);
  changeColor(currentColor);
  RGB_color2(255, 5, 0);   
  
}
