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

#define RED 6
#define GREEN 5
#define BLUE 4

SoftwareSerial HC12(7,6); //HC12 TX Pin, HC12 RX pin
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
  String input = "";
  //receiving data
  if (HC12.available())
  {
    byte incomingByte;
    while (HC12.available()){     //if HC-12 has recieved
      incomingByte = HC12.read(); //read next byte
      input += char(incomingByte);  //concatenate to 'input'
    }
    Serial.print(input);  //print that data to serial monitor
    digitalWrite(2, HIGH);
    delay(500);
    digitalWrite(2, LOW);
  }

  if(input == "Danger")
  {
    changeColor("red");
  }
  if(input == "Hazardous")
  {
    changeColor("blue");
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
  analogWrite(BLUE, blue_light_value);
}

void changeColor(String color)
{
  if(color == "red")
  {
    RGB_color(255, 0, 0);
  }
  if(color == "green")
  {
    RGB_color(0, 255, 0);
  }
  if(color =="blue")
  {
    RGB_color(0, 0, 255); 
  }
}
