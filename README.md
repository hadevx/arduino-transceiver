# 📡 Arduino NRF24L01 Wireless Transceiver

A wireless transceiver system built on Arduino using the **NRF24L01** radio module. A transmitter reads joystick and rotary encoder inputs and sends them wirelessly to a receiver that controls a servo motor, DC motor, and buzzer in real time.

---

## 🔧 Hardware Used

### Transmitter
| Component | Purpose |
|---|---|
| Arduino (Uno/Nano) | Main microcontroller |
| NRF24L01 | Wireless communication |
| Joystick 1 (X-axis) | Controls X direction |
| Joystick 2 (Y-axis) | Controls Y direction |
| Joystick Button | Triggers buzzer on receiver |
| Rotary Encoder 1 | Adjusts motor speed (0–255) |
| Rotary Encoder 2 | Adjusts servo angle (0–180°) |
| LCD 16x2 (I2C) | Displays live joystick and encoder values |
| OLED 128x32 (SSD1306) | Displays NRF24L01 status on startup |

### Receiver
| Component | Purpose |
|---|---|
| Arduino (Uno/Nano) | Main microcontroller |
| NRF24L01 | Wireless communication |
| Servo Motor (pin 5) | Controlled by Encoder 2 (0–180°) |
| DC Motor / LED (pin 6) | Speed controlled by Encoder 1 (0–255 PWM) |
| Buzzer (pin 2) | Triggered by Joystick 1 button press |

---

## 📦 Libraries Required

Install these via the Arduino Library Manager:

| Library | Usage |
|---|---|
| `RF24` | NRF24L01 radio communication |
| `LiquidCrystal_I2C` | I2C LCD display |
| `Adafruit_GFX` | OLED graphics base library |
| `Adafruit_SSD1306` | OLED display driver |
| `Servo` | Servo motor control (receiver) |
| `SPI` | SPI bus communication |
| `Wire` | I2C bus communication |

---

## 🗂️ Project Structure

```
├── transmitter/
│   └── transmitter.ino     # Reads joysticks + encoders, sends data via NRF24L01
└── receiver/
    └── receiver.ino        # Receives data, drives servo, motor, and buzzer
```

---

## 📡 How It Works

### Data Packet

Both transmitter and receiver share the same data structure:

```cpp
struct DataPacket {
  bool buttonState;   // Joystick 1 button — triggers buzzer
  byte speed;         // Encoder 1 value (0–255) — controls DC motor PWM
  byte counter;       // Encoder 2 value (0–180) — controls servo angle
  byte xValue;        // Joystick 1 X-axis (0–1023 mapped)
  byte yValue;        // Joystick 2 Y-axis (0–1023 mapped, inverted)
};
```

### Transmitter Flow

```
Read Joysticks & Encoders
        │
Apply Deadzone (±50 from center)
        │
Update LCD (only on change)
        │
Pack into DataPacket struct
        │
Send via NRF24L01 (writing mode)
```

### Receiver Flow

```
Listen for incoming packet
        │
Read DataPacket struct
        │
├── analogWrite(6, speed)       → DC Motor speed
├── servo.write(counter)        → Servo angle
└── digitalWrite(2, buttonState) → Buzzer on/off
```

---

## 📌 Pin Reference

### Transmitter

| Pin | Component |
|---|---|
| A1 | Joystick 1 X-axis |
| A3 | Joystick 2 Y-axis |
| 7 | Joystick 1 Button |
| 8 | Joystick 2 Button |
| 2 | Rotary Encoder 1 CLK (Interrupt) |
| 4 | Rotary Encoder 1 DT |
| 3 | Rotary Encoder 2 CLK (Interrupt) |
| 5 | Rotary Encoder 2 DT |
| 9 | NRF24L01 CE |
| 10 | NRF24L01 CSN |
| SDA/SCL | LCD + OLED (I2C) |

### Receiver

| Pin | Component |
|---|---|
| 2 | Buzzer |
| 3 | DC Motor / LED (PWM) |
| 5 | Servo Motor (PWM) |
| 6 | DC Motor / LED (PWM) |
| 9 | NRF24L01 CE |
| 10 | NRF24L01 CSN |

---

## ⚙️ Configuration

Both transmitter and receiver must share the same **address** and **PA level**:

```cpp
const byte address[6] = "00001";
radio.setPALevel(RF24_PA_LOW);  // Change to RF24_PA_HIGH for longer range
```

To increase wireless range, change `RF24_PA_LOW` to `RF24_PA_HIGH` on both sides.

---

## 🖥️ Transmitter Display

### LCD (16x2)
Updates only when values change to reduce flickering:
```
X:512  Y:512
S:100  C:90
```

### OLED (128x32)
Shows NRF24L01 status on startup:
```
NRF Success
Data rate: 1 Mbps
Channel: 76
```

---

## 🚀 Getting Started

1. Wire up both Arduinos according to the pin reference above
2. Install all required libraries via Arduino Library Manager
3. Upload `transmitter.ino` to the transmitter Arduino
4. Upload `receiver.ino` to the receiver Arduino
5. Power both boards — the OLED will confirm NRF24L01 status on the transmitter
6. Move the joysticks and rotate the encoders to control the receiver outputs

---

## 📄 License

This project is licensed under the MIT License.
