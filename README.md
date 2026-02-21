# ATmega32-Electronic-Voting-Machine
Electronic Voting Machine using ATmega32 with PIN authentication, vote counting, and LCD result display (Proteus Simulation + Embedded C Code)

# 🗳 Electronic Voting Machine using ATmega32

## 📌 Project Overview

This project implements a secure and efficient Electronic Voting Machine (EVM) using the ATmega32 microcontroller.  
It features PIN-based authentication, candidate selection via keypad, vote counting, and LCD result display.

Designed and simulated using Proteus.

---

## 🚀 Features

- Initial Security Key Authentication
- Voter PIN Verification
- Master Key Access for Results
- 4 Candidate Voting System
- Vote Confirmation before submission
- Real-time Vote Counting
- Winner Detection Logic
- 16x2 LCD Display Interface
- Matrix Keypad Input

---

## 🛠 Hardware Components

- ATmega32 Microcontroller
- 16x2 LCD (LM016L)
- 4x4 Matrix Keypad
- Crystal Oscillator (8MHz)
- Capacitors & Resistors
- LEDs (Status Indicators)

---

## 🧠 System Workflow

1. System asks for initial security key
2. Voter enters PIN
3. Valid voter selects candidate (1–4)
4. Vote confirmation using '=' key
5. Vote count increments
6. Master key allows viewing:
   - Total votes
   - Winner

---

## 📂 Repository Structure

ATmega32-Electronic-Voting-Machine/
│
├── README.md
├── LICENSE
│
├── evm_atmega32.c
├── Team11_code.doc
├── EVMPilot.DSN
├── EVMPilot.PDF
├── demo_video.mp4
├── EVMPilot.PWI
|


