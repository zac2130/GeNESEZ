#include <stdio.h>






int main (){
	struct CPU{
		char memory[8000]; //an array for the 6502's 64kb of memory, comes in 16 bits per entry
				       //will need to implement a function for outputing standard hex characters
		short int programCounter; // 16 bit value
		char stackPointer; // an 8 bit pointer to the nest available address in the stack
		char accumulator;
		char indexRegisterX;
		char indexRegisterY;
		char processorStatus; // each bit represents a flag
	}CPU = {{0},0xff,0xf};

	struct PPU {
		
	}PPU;

	printf("%#04x",CPU.programCounter);

	return 0;
}
