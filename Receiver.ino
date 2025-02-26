//Receiver
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

RF24 radio(9, 10);  // CE, CSN pins
const byte address[6] = "00001";  // Communication address
Servo servo;

// Structure to receive data
struct DataPacket {
  bool buttonState;
  byte speed;
  byte counter;
  byte xValue;
  byte yValue;
} receivedData;


void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT); // Buzzer
  pinMode(3, OUTPUT); // DC motor/LED PWM
  pinMode(6, OUTPUT); // DC motor/LED PWM
  servo.attach(5); //PWM

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();  // Set as Receiver
}

void loop() {
  if (radio.available()) {
  radio.read(&receivedData, sizeof(receivedData));
  Serial.println(receivedData.counter);
  analogWrite(6, receivedData.speed); 
  analogWrite(5, receivedData.counter); 
  digitalWrite(2, receivedData.buttonState);  
  servo.write(receivedData.counter);
  }

  delay(10);  
}
