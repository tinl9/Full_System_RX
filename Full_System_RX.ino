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

#define CLEAR 100
#define DANGER 101
#define HAZARD 102

#define on_time_ms 500
#define off_time_ms 2000
#define maxTimeBetweenSignalSeconds 30

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
    if(char(incomingByte) != '\n')
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
    changeColor(words);
    currentColor = words;
    Serial.println(words);
  }   
    
  if(words == "Danger")
  {
    resetTime = millis()/1000;
    Serial.println("Writing to 101 flash memory");
    Serial.printf("Reset timer to: %d s\n", resetTime);    
    EEPROM.write(0, DANGER);
    EEPROM.commit();
    changeColor("red");
    currentColor = "red";
  }
  else if(words == "Hazard")
  {
    resetTime = millis()/1000;
    Serial.println("Writing to 102 flash memory");
    Serial.printf("Reset timer to: %d s\n", resetTime);
    EEPROM.write(0, HAZARD);
    EEPROM.commit();  
    changeColor("blue");
    currentColor = "blue";      
  }
}

void flashLED(void)
{
  delay(on_time_ms);
  RGB_color(0, 0, 0);  
  delay(off_time_ms);
  changeColor(currentColor);
  
}
