#include "tvm.h"
#include "hexout.h"

int main() {
	CPU cpu;
	Memory memory;
	cpu.Reset(memory);
	memory[0xFFFC] = CPU::INS_LDA_ZP;
	memory[0xFFFD] = 0x42;
	memory[0x0042] = 0x84;
	cpu.Execute(memory, 3);
	
	std::cout << "Accumulator: " << hex_out_s(cpu.A) << "\nMemory[0xFFFC-0xFFFD]: " << hex_out_s(memory[0xFFFC]) << ' ' << hex_out_s(memory[0xFFFD]) << std::endl;
	memory.Dump();
	return 0;
}
