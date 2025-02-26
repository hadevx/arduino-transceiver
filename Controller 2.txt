#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 32, &Wire, -1);

// Joystick 1
//#define JOY1Y_PIN A0  
#define JOY1X_PIN A1 
#define JOY1BTN_PIN 7   

// Joystick 2
#define JOY2Y_PIN A3  
//#define JOY2X_PIN A2  
#define JOY2BTN_PIN 8  

// Encoder 1
#define CLK1 2       // Rotary encoder 1 clock pin (Interrupt)
#define DT1 4        // Rotary encoder 1 data pin

// Encoder 2
#define CLK2 3       // Rotary encoder 2 clock pin (Interrupt)
#define DT2 5        // Rotary encoder 2 data pin

#define DEADZONE 50

RF24 radio(9, 10);  // CE, CSN pins
LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte address[6] = "00001";  // Communication address

// Structure to send multiple values
struct DataPacket {
  bool buttonState;
  byte speed;
  byte counter;
  byte xValue;
  byte yValue;
};

DataPacket dataToSend;

volatile int speed = 0;  // Motor speed (0-255)
int counter = 0;
int lastX = -1, lastY = -1, lastSpeed = -1, lastCounter = -1; // To track changes


void setup() {
    Serial.begin(9600);

    setupOLED();

    lcd.init();
    lcd.backlight();

    pinMode(JOY1BTN_PIN, INPUT_PULLUP);
    pinMode(JOY2BTN_PIN, INPUT_PULLUP);
    pinMode(CLK1, INPUT_PULLUP);
    pinMode(DT1, INPUT_PULLUP);
    pinMode(CLK2, INPUT_PULLUP);
    pinMode(DT2, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(CLK1), updateEncoder1, CHANGE);
    attachInterrupt(digitalPinToInterrupt(CLK2), updateEncoder2, CHANGE);

    radio.begin();
    radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_LOW);
    radio.stopListening();  // Set as Transmitter

}


void loop() {
    bool currentState = digitalRead(JOY1BTN_PIN) == LOW;  // Pressed = LOW
    int xValue = analogRead(JOY1X_PIN);  // Read X-axis
    int yValue = 1023 - analogRead(JOY2Y_PIN);

    // Apply deadzone to ignore small fluctuations
    if (abs(xValue - 512) < DEADZONE) xValue = 512;
    if (abs(yValue - 512) < DEADZONE) yValue = 512;
    
    updateLCD(xValue, yValue, speed, counter);

    // Update structure
    dataToSend.buttonState = currentState;
    dataToSend.speed = speed;
    dataToSend.counter = counter;
    dataToSend.xValue = xValue;
    dataToSend.yValue = yValue;

    // Send data
    radio.write(&dataToSend, sizeof(dataToSend));
}

// Interrupt function for rotary encoder 1
void updateEncoder1() {
    if (digitalRead(DT1) != digitalRead(CLK1)) {
        speed += 5;  // Clockwise -> Increase speed
    } else {
        speed -= 5;  // Counterclockwise -> Decrease speed
    }
    speed = constrain(speed, 0, 255);  // Keep speed within range
}

// Interrupt function for rotary encoder 2 (example)
void updateEncoder2() {
      if (digitalRead(DT2) == digitalRead(CLK2)) {
        counter += 5;  // Clockwise -> Increase speed
    } else {
        counter -= 5;  // Counterclockwise -> Decrease speed
    }
    counter = constrain(counter, 0, 180);  
}
  
void updateLCD(int xValue, int yValue, int speed, int counter) {
    if (xValue != lastX || yValue != lastY || speed != lastSpeed || counter != lastCounter) {
        lcd.setCursor(0, 0);
        lcd.print("X:");
        lcd.print(xValue);
        lcd.print("  Y:");
        lcd.print(yValue);
        lcd.print("   "); // Overwrite old characters

        lcd.setCursor(0, 1);
        lcd.print("S:");
        lcd.print(speed);
        lcd.print("  C:");
        lcd.print(counter);
        lcd.print("   "); // Overwrite old characters

        // Store last values to prevent unnecessary updates
        lastX = xValue;
        lastY = yValue;
        lastSpeed = speed;
        lastCounter = counter;
    }

    delay(100); // Slightly higher delay to reduce flickering
}



void setupOLED() {
    // Initialize OLED display
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        for (;;); // Halt if OLED fails
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);

    // Initialize NRF24L01 and check status
    if (radio.begin()) {
        display.println("NRF Success");
    } else {
        display.println("NRF Failed");
        display.display();
        return; // Exit function if NRF fails
    }

    // Get and display Data Rate
    display.setCursor(0, 10);
    display.print("Data rate: ");
    switch (radio.getDataRate()) {
        case RF24_250KBPS:
            display.println("250 kbps");
            break;
        case RF24_1MBPS:
            display.println("1 Mbps");
            break;
        case RF24_2MBPS:
            display.println("2 Mbps");
            break;
    }

    // Display Channel
    display.setCursor(0, 20);
    display.print("Channel: ");
    display.println(radio.getChannel());

    display.display(); // Show all info
}

