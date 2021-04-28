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

#define RED 2
#define GREEN 1
#define BLUE 0

SoftwareSerial HC12(7,6); //HC12 TX Pin, HC12 RX pin

String input = "";
byte incomingByte;

void setup(){
  Serial.begin(9600);
  HC12.begin(9600);
  pinMode(2,OUTPUT);

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);  
//  Serial.write("AT+FU4,B1200");
}

void loop()
{
  delay(100);
  
  //receiving data
  input = "";
  while (HC12.available()) //if HC-12 has recieved
  {     
    incomingByte = HC12.read(); //read next byte
    if(char(incomingByte) != '\n')
    {
      input += char(incomingByte);  //concatenate to 'input'
    }
  }
  
  if(input == "Danger")
  {
    changeColor("red");
    Serial.println("Changing to red");
  }
  else if(input == "Hazard")
  {
    changeColor("blue");
    Serial.println("Changing to blue");
  }
  else if (input != "")
  {
    changeColor(input);    
    Serial.println(input);
  }
  

  //sending data
  while(Serial.available()){    //if we have inputted data to serial monitor
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
    Serial.println("CHanging to blue");
    RGB_color(0,0,0);
    digitalWrite(BLUE, HIGH);
    
  }
  else if(color == "yellow")
  {
    RGB_color(255, 50, 0);
  }
  else
  {
    RGB_color(0, 0, 0);
  }
}
