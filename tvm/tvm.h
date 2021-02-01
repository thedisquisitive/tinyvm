#include <iostream>
#include <cstdlib>
#include <fstream>
#include "hexout.h"

#define BYTE unsigned char	// 8 Bit BYTE
#define WORD unsigned short	// 16 Bit WORD
#define u32  unsigned int      // 32 bit unsigned integer

#define MAX_MEMORY 1024*64	// We can have 64KB of memory total

struct Memory {
	BYTE data[MAX_MEMORY]; // Our array of bytes in memory
	
	void Reset() {
		for (u32 i = 0; i < MAX_MEMORY; i++) {
			data[0] = 0;
		}
	}
	
	void Dump() {
		std::ofstream fout("memdump.txt");
		for (u32 i = 0; i < MAX_MEMORY; i++) {
		       fout << hex_out_s(data[i]) << ' ';
	       	       //if ((i % 32) == 0)
			//fout << std::endl;
		}	       
	}

	BYTE operator[]( u32 Address ) const {
		//if (Address < MAX_MEMORY)
		return data[Address];
	}

	BYTE& operator[]( u32 Address ) {
		return data[Address];
	}

	void WriteWord(WORD d, u32 Address) {
		data[Address] = d & 0xFF;
		data[Address+1] = (d >> 8);
	}
};

/*
 * This tiny VM is based on the 6502 CPU
 * One might even consider it a 6502 emulator
 */

struct CPU {
	WORD programCounter;	// Current location in memory
	WORD stackPointer;	// Current stack location

	BYTE A, X, Y;		// the 6502 uses 3 8-bit registers

	// Processor Status Flags
	// Using bit fields;
	BYTE carryFlag 		: 1; // Set if last op caused overflow from bit 7 or underflow from bit 0
	BYTE zeroFlag  		: 1; // Set is last op was zero
        BYTE interruptDisable 	: 1; // Set is last op was Set Interrupt Disable (SEI), CPU ignores interrupts until CLI op
	BYTE decimalFlag 	: 1; // CPU will obey binary coded decimal rules until cleared with CLD. Set by SED
	BYTE breakCommand 	: 1; // Set when a BRK command is executed and interrupt has been generated to process it
	BYTE overflowFlag 	: 1; // Set during arithmetic if op yields invalid 2's compliment result. Determined by looking at carry on bits 6 and 7, and 7 and carry
	BYTE negativeFlag	: 1; // Set if result of last op had bit 7 set to 1	

	
	
	void Reset(Memory &memory) {
		programCounter = 0xFFFC;
		carryFlag = interruptDisable = breakCommand = overflowFlag = negativeFlag = decimalFlag = 0;
		stackPointer = 0x0100;
		A = X = Y = 0;
		memory.Reset();
	}
	
	BYTE Fetch(Memory &memory, u32 &cycles) {
		BYTE Data = memory[programCounter];
		programCounter++;
		cycles--;
		return Data;
	}

	WORD FetchWord(Memory &memory, u32 &cycles) {
		WORD Data = memory[programCounter];
		programCounter++;
		//cycles--;
		Data |= (memory[programCounter] << 8);
		programCounter++;

		Cycles -= 2;
	}

	BYTE ReadByte(Memory &memory, BYTE address, u32 &cycles) {
		BYTE Data = memory[address];
		cycles--;
		return Data;
	}

	void LDASetStatus() {
		zeroFlag = (A == 0);
		negativeFlag = (A & 0b10000000) > 0;
	}

	static constexpr BYTE INS_LDA_IM = 0xA9;	// Load Accumalator Immediate Mode
	static constexpr BYTE INS_LDA_ZP = 0xA5;
	static constexpr BYTE INS_LDA_ZPX = 0xB5;
	static constexpr BYTE INS_LDA_ABS = 0xAD;
	static constexpr BYTE INS_JSR	  = 0x20;

	void Execute(Memory &memory, u32 cycles) {
		while (cycles > 0) {
			BYTE instruction = Fetch(memory, cycles);
			switch (instruction) {
				case INS_LDA_IM: {
					BYTE value = Fetch(memory, cycles);
					A = value;
					LDASetStatus();
					break;
				}
				case INS_LDA_ZP: {
					A = ReadByte(memory, Fetch(memory, cycles), cycles);
					LDASetStatus();
					break;
				}
				case INS_LDA_ZPX: {
					BYTE zpa = FetchByte(memory, cycles);
					cycles--;
					A = ReadByte(memory, zpa, cycles);
					LDASetStatus();
				}
				case INS_JSR: {
					WORD subaddr = FetchWord(memory, cycles);
					memory[stackPointer] = programCounter - 1;
					cycles--;
					programCounter = subaddr;
					cycles--;
					stackPointer++;
				}
				default: { 
					std::cout << "Instruction unknown: " << instruction << std::endl;
					break;
				}
			}
		}

	}	
};

