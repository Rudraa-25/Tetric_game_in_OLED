# 🎮 Tetris on OLED — Game Project

A fully playable **Tetris** for **ESP32 C3 Super mini** using a **128×64 OLED display**, **joystick**, **touch sensor**, and **speaker**.  
Includes realistic sounds, next-piece preview, scoring, levels, and smooth OLED rendering.

---

## ⚙️ Overview

This project brings the **classic Tetris** to microcontrollers with minimal components.  
It’s designed for the **SSD1306 OLED** using **Adafruit GFX** and **SSD1306** libraries.

**Gameplay Highlights:**
- Move pieces with a **joystick**  
- Rotate with a **touch sensor**  
- Hard drop using **joystick button**  
- Level up as speed increases  
- Real-time **score**, **lines**, and **next piece preview**  
- **Sound effects** for every action  

---

## 🧰 Hardware Requirements

| Component            | Description / Notes                              |
|----------------------|--------------------------------------------------|
| **Microcontroller**  | ESP32 C3 Super Mini,Arduino                      |
| **Display**          | 0.96” or 1.3” OLED (SSD1306, I²C)                |
| **Joystick**         | 2-axis analog with push button                   |
| **Touch Sensor**     | TTP223 or any digital capacitive touch module    |
| **Speaker/Buzzer**   | Small 5V active buzzer                           |
| **Breadboard & Wires** | For connections                                |
| **Power Source**     | 5V or USB supply                                 |

---

## 🔌 Pin Configuration

![Tetris OLED Preview]("C:\Users\Hemangi\OneDrive\Desktop\TETRIC_IN_ESP32C3_WITH_0.96OLED\0193881d-69f1-42ac-894c-688cd1ec4c11.jpg")


| Component         | Arduino Pin | Description         |
|-------------------|-------------|---------------------|
| **OLED SDA**      | D8          | I²C SDA             |
| **OLED SCL**      | D9          | I²C SCL             |
| **Joystick X**     | A0          | Horizontal movement |
| **Joystick Y**     | A1          | Vertical movement   |
| **Joystick Button**| D4          | Hard drop           |
| **Touch Sensor**   | D5          | Rotate piece        |
| **Speaker**        | D6          | Game sounds         |

> Adjust pins if you use a different board (ESP32, etc.).

---

## 🕹️ Controls

| Action             | Input Method             |
|--------------------|--------------------------|
| Move Left / Right  | Joystick (X-axis)        |
| Rotate Piece       | Touch sensor tap         |
| Hard Drop          | Joystick button press    |
| Start / Restart    | Touch sensor tap         |
| Soft Drop          | Automatic / joystick down|

---

