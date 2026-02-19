# CHIP-8 Emulator

## 🎮 Overview

This project is a **CHIP-8 Emulator** built in **C++ using SDL2**.

It replicates the CHIP-8 virtual machine by implementing:

- Instruction decoding & execution
- Memory and stack management
- Graphics rendering
- Keyboard input handling
- ROM loading and execution

The emulator can run classic CHIP-8 games like Pong, Space Invaders, and Tetris.

## ✨ Features

- 35 CHIP-8 opcodes implemented
- Accurate 64×32 monochrome display rendering
- Real-time keyboard input mapping
- Adjustable execution speed
- Cross-platform build with CMake
- SDL2-based graphics output

## 🛠How to Build

1. Install CMake and a C++ compiler (MinGW / GCC).
2. Open terminal in project folder.

mkdir build
cd build
cmake ..
cmake --build .

## How to Run

Run the emulator with a ROM file:

./chip8 path/to/rom.ch8

Example:

./chip8 ../roms/pong.ch8

## Controls

Original CHIP-8 keypad:

1 2 3 C
4 5 6 D
7 8 9 E
A 0 B F

Keyboard mapping:

1 2 3 4
Q W E R
A S D F
Z X C V

## Project Structure

chip-8/
├── include/      → header files
├── src/          → emulator source code
├── roms/         → test ROMs
├── vendor/       → external libraries
└── CMakeLists.txt

## Learning Outcomes
This project demonstrates:
- Low-level system emulation
- Opcode decoding & CPU architecture understanding
- Memory and stack management
- Graphics rendering using SDL
- Event-driven input handling

## 🛠 Build Instructions

```bash
mkdir build
cd build
cmake ..
cmake --build .


## 🎥 Demo

Gameplay running inside the emulator:

![CHIP-8 Pong](pong-demo.png)

## 🎮 Controls

CHIP-8 keypad mapped to keyboard:

1 2 3 4  -> 1 2 3 C
Q W E R  -> 4 5 6 D
A S D F  -> 7 8 9 E
Z X C V  -> A 0 B F
 
## ▶️ Run the Emulator

From the build directory:

```bash.\chip8.exe 15 2 "..\roms\Pong (1 player).ch8"

### Supported ROMs
- Pong
- Space Invaders
- Tetris
