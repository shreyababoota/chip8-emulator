#include "chip8.h"
#include "platform.h"
#include <fstream>
#include <vector>
#include <cstring>
#include <random>
#include <iostream>

const unsigned int start_address = 0x200;
const unsigned int fontset_start_address = 0x50;

chip8::chip8() {
    pc = start_address;

    std::vector<uint8_t> fontset = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    for (size_t i = 0; i < fontset.size(); ++i) {
        memory[fontset_start_address + i] = fontset[i];
    }
}
void chip8::LoadROM(const char* filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        std::streampos size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::vector<char> buffer(size);
        file.read(buffer.data(), size);
        file.close();

        for (long i = 0; i < size; ++i) {
            memory[start_address + i] = buffer[i];
        }
    }
}

// In chip8.cpp
void chip8::Cycle() {
    // Fetch Opcode
    opcode = (memory[pc] << 8) | memory[pc + 1];

    // Decode and Execute Opcode
    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x00FF) {
                case 0x00E0: // CLS: Clear the display
                    memset(video, 0, sizeof(video));
                    pc += 2;
                    break;
                case 0x00EE: // RET: Return from a subroutine
                    --sp;
                    pc = stack[sp];
                    pc += 2;
                    break;
                default:
                    pc += 2;
                    break;
            }
            break;

        case 0x1000: // JP addr: Jump to location nnn
            pc = opcode & 0x0FFF;
            break;

        case 0x2000: // CALL addr: Call subroutine at nnn
            stack[sp] = pc;
            ++sp;
            pc = opcode & 0x0FFF;
            break;

        case 0x3000: // SE Vx, byte: Skip next instruction if Vx = kk
            if (registers[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
                pc += 4;
            else
                pc += 2;
            break;

        case 0x4000: // SNE Vx, byte: Skip next instruction if Vx != kk
            if (registers[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
                pc += 4;
            else
                pc += 2;
            break;

        case 0x5000: // SE Vx, Vy: Skip next instruction if Vx = Vy
            if (registers[(opcode & 0x0F00) >> 8] == registers[(opcode & 0x00F0) >> 4])
                pc += 4;
            else
                pc += 2;
            break;

        case 0x6000: // LD Vx, byte: Set Vx = kk
            registers[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            pc += 2;
            break;

        case 0x7000: // ADD Vx, byte: Set Vx = Vx + kk
            registers[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            pc += 2;
            break;

        case 0x8000: { // Arithmetic and Logical Opcodes
            uint8_t Vx = (opcode & 0x0F00) >> 8;
            uint8_t Vy = (opcode & 0x00F0) >> 4;
            switch (opcode & 0x000F) {
                case 0x0000: registers[Vx] = registers[Vy]; break; // 8xy0 - LD Vx, Vy
                case 0x0001: registers[Vx] |= registers[Vy]; break; // 8xy1 - OR Vx, Vy
                case 0x0002: registers[Vx] &= registers[Vy]; break; // 8xy2 - AND Vx, Vy
                case 0x0003: registers[Vx] ^= registers[Vy]; break; // 8xy3 - XOR Vx, Vy
                case 0x0004: { // 8xy4 - ADD Vx, Vy
                    uint16_t sum = registers[Vx] + registers[Vy];
                    registers[0xF] = (sum > 255); // Set VF = carry
                    registers[Vx] = sum & 0xFF;
                } break;
                case 0x0005: // 8xy5 - SUB Vx, Vy
                    registers[0xF] = (registers[Vx] > registers[Vy]); // Set VF = NOT borrow
                    registers[Vx] -= registers[Vy];
                    break;
                case 0x0006: // 8xy6 - SHR Vx
                    registers[0xF] = registers[Vx] & 0x1; // Set VF = least significant bit
                    registers[Vx] >>= 1;
                    break;
                case 0x0007: // 8xy7 - SUBN Vx, Vy
                    registers[0xF] = (registers[Vy] > registers[Vx]); // Set VF = NOT borrow
                    registers[Vx] = registers[Vy] - registers[Vx];
                    break;
                case 0x000E: // 8xyE - SHL Vx
                    registers[0xF] = (registers[Vx] & 0x80) >> 7; // Set VF = most significant bit
                    registers[Vx] <<= 1;
                    break;
            }
            pc += 2;
        } break;

        case 0x9000: // SNE Vx, Vy: Skip next instruction if Vx != Vy
            if (registers[(opcode & 0x0F00) >> 8] != registers[(opcode & 0x00F0) >> 4])
                pc += 4;
            else
                pc += 2;
            break;

        case 0xA000: // LD I, addr: Set I = nnn
            index = opcode & 0x0FFF;
            pc += 2;
            break;

        case 0xB000: // JP V0, addr: Jump to location nnn + V0
            pc = (opcode & 0x0FFF) + registers[0];
            break;

        case 0xC000: // RND Vx, byte: Set Vx = random byte AND kk
            registers[(opcode & 0x0F00) >> 8] = (rand() % 256) & (opcode & 0x00FF);
            pc += 2;
            break;

        case 0xD000: // DRW Vx, Vy, nibble
        {
            uint8_t Vx = (opcode & 0x0F00) >> 8;
            uint8_t Vy = (opcode & 0x00F0) >> 4;
            uint8_t height = opcode & 0x000F;
            uint8_t xPos = registers[Vx];
            uint8_t yPos = registers[Vy];
            registers[0xF] = 0;
            for (int row = 0; row < height; ++row) {
                uint8_t spriteByte = memory[index + row];
                for (int col = 0; col < 8; ++col) {
                    if ((spriteByte & (0x80 >> col)) != 0) {
                        int screenX = (xPos + col) % VIDEO_WIDTH;
                        int screenY = (yPos + row) % VIDEO_HEIGHT;
                        uint32_t& screenPixel = video[screenY * VIDEO_WIDTH + screenX];
                        if (screenPixel == 0xFFFFFFFF)
                            registers[0xF] = 1; // Collision detected
                        screenPixel ^= 0xFFFFFFFF;
                    }
                }
            }
            pc += 2;
        }
        break;

        case 0xE000:
            switch (opcode & 0x00FF) {
                case 0x009E: // EX9E: Skips the next instruction if the key stored in VX is pressed
                    if (keypad[registers[(opcode & 0x0F00) >> 8]])
                        pc += 4;
                    else
                        pc += 2;
                    break;
                case 0x00A1: // EXA1: Skips the next instruction if the key stored in VX is NOT pressed
                    if (!keypad[registers[(opcode & 0x0F00) >> 8]])
                        pc += 4;
                    else
                        pc += 2;
                    break;
                default:
                    pc += 2;
                    break;
            }
            break;

        case 0xF000: {
            uint8_t Vx = (opcode & 0x0F00) >> 8;
            switch (opcode & 0x00FF) {
                case 0x0007: registers[Vx] = delayTimer; break; // Fx07 - LD Vx, DT
                case 0x000A: { // Fx0A - LD Vx, K
                    bool keyPressed = false;
                    for (int i = 0; i < 16; ++i) {
                        if (keypad[i]) {
                            registers[Vx] = i;
                            keyPressed = true;
                        }
                    }
                    if (!keyPressed) return; // If no key pressed, repeat this cycle
                } break;
                case 0x0015: delayTimer = registers[Vx]; break; // Fx15 - LD DT, Vx
                case 0x0018: soundTimer = registers[Vx]; break; // Fx18 - LD ST, Vx
                case 0x001E: index += registers[Vx]; break; // Fx1E - ADD I, Vx
                case 0x0029: index = fontset_start_address + (registers[Vx] * 5); break; // Fx29 - LD F, Vx
                case 0x0033: // Fx33 - LD B, Vx
                    memory[index] = registers[Vx] / 100;
                    memory[index + 1] = (registers[Vx] / 10) % 10;
                    memory[index + 2] = registers[Vx] % 10;
                    break;
                case 0x0055: // Fx55 - LD [I], Vx
                    for (int i = 0; i <= Vx; ++i) memory[index + i] = registers[i];
                    break;
                case 0x0065: // Fx65 - LD Vx, [I]
                    for (int i = 0; i <= Vx; ++i) registers[i] = memory[index + i];
                    break;
            }
            pc += 2;
        } break;

        default:
            pc += 2;
            break;
    }

    // Update timers
    if (delayTimer > 0)
        --delayTimer;
    if (soundTimer > 0)
        --soundTimer;
}