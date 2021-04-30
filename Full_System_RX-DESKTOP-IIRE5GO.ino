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

#define RED 2
#define GREEN 1
#define BLUE 0

#define RED2  9
#define GREEN2  8
#define BLUE2 3

#define CLEAR 100
#define DANGER 101
#define HAZARD 102

#define on_time_ms 500
#define off_time_ms 2000
#define maxTimeBetweenSignalSeconds 10

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
//  Serial.write("AT+FU4,B1200");
}

void loop()
{
  currentTime = millis()/1000;
  delay(100);
  String sig = recieveData();
  checkSignal(sig);
  sendDataFromSerial(); 
  flashLED();
  if(currentTime - resetTime > maxTimeBetweenSignalSeconds)
  {
    Serial.println("Too much time between signal from master");
    //Do stuff
    changeColor("yellow");
    currentColor = "yellow";
  }
}
String recieveData(void)
{
  String data = "";
  //receiving data
  while (HC12.available()) //if HC-12 has recieved
  {     
    incomingByte = HC12.read(); //read next byte
    Serial.println(char(incomingByte));
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
void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
{
  analogWrite(RED, red_light_value);
  analogWrite(GREEN, green_light_value);
  digitalWrite(BLUE, LOW);
}

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
    RGB_color(255, 50, 0);
  }
  else if(color == "white")
  {
    RGB_color(255, 50, 0);
    digitalWrite(BLUE, HIGH);
  }
  else
  {
    RGB_color(0, 0, 0);
  }
}

void checkSignal(String words)
{
  if (words != "")
  {
    resetTime = millis()/1000;    
    changeColor(words);
    currentColor = words;
    Serial.println(words);
  }   
  if(words == "Clear")
  {
    resetTime = millis()/1000;
    changeColor("white");
    currentColor = "white";
    if(EEPROM.read(0) != CLEAR)
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
  if(words == "Danger")
  {
    resetTime = millis()/1000;
    changeColor("red");
    currentColor = "red";
    if(EEPROM.read(0) != DANGER)
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
    resetTime = millis()/1000;
    changeColor("blue");
    currentColor = "blue"; 
    if(EEPROM.read(0) != HAZARD)
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

void flashLED(void)
{
  delay(on_time_ms);
  RGB_color(0, 0, 0);  
  delay(off_time_ms);
  changeColor(currentColor);
  
}
