#include <stdio.h>

// general structurs and information

#define iNES 0
#define NES20 1
#define UNIF 2
#define NSF 3
#define NSF2 4
#define NSFE 5
#define FDS 6
#define QD 7

struct CPU{
	char memory[8000]; // 6502 interrnal memory 8000kB, 64kb
	int PC; // program counter 64bit
	short int S; // Stack pointer 16bit
	char A; // accumulator 8bit
	char RX; // register X 8bit
	char RY; // register Y 8bit
	char P; // status register 8bit: Negative, Overflow, always 1, B flag, Decimal, Interupt Disable, Zero, Carry {NV1BDIZC}

};

struct PPU{
	char PPUCTRL; // VPHB SINN; NMI enable (V), PPU master/slave (P), sprite height (H), background tile select (B), sprite tile select (S), increment mode (I), nametable select / X and Y scroll bit 8 (NN)
	char PPUMASK; // BGRs bMmG; color emphasis (BGR), sprite enable (s), background enable (b), sprite left column enable (M), background left column enable (m), greyscale (G)
	char PPUSTATUS; // VSO- ----; vblank (V), sprite 0 hit (S), sprite overflow (O); read resets write pair for $2005/$2006 
	char OAMADDR; // AAAA AAAA; OAM read/write address
	char OAMDATA; // DDDD DDDD; OAM data read/write
	char PPUSCROLL; // XXXX XXXX/YYYY YYYY; X and Y scroll bits 7-0 (two writes: X scroll, then Y scroll)
	char PPUADDR; // AAAA AAAA/AAAA AAAA; VRAM address (two writes: most significant byte, then least significant byte)
	char PPUDATA; // DDDD DDDD; VRAM data read/write
	char OAMDMA; // AAAA AAAA; OAM DMA high address 
};

char opCodesLUT[256][4] = {
	"BRK","ORA","STP","SLO","NOP","ORA","ASL","SLO","PHP","ORA","ASL","ANC","NOP","ORA","ASL","SLO","BPL","ORA","STP","SLO","NOP","ORA","ASL","SLO","CLC","ORA","NOP","SLO","NOP","ORA","ASL","SLO",
	"JSR","AND","STP","RLA","BIT","AND","ROL","RLA","PLP","AND","ROL","ANC","BIT","AND","ROL","RLA","BMI","AND","STP","RLA","NOP","AND","ROL","RLA","SEC","AND","NOP","RLA","NOP","AND","ROL","RLA",
	"RTI","EOR","STP","SRE","NOP","EOR","LSR","SRE","PHA","EOR","LSR","ALR","JMP","EOR","LSR","SRE","BVC","EOR","STP","SRE","NOP","EOR","LSR","SRE","CLI","EOR","NOP","SRE","NOP","EOR","LSR","SRE",
	"RTS","ADC","STP","RRA","NOP","ADC","ROR","RRA","PLA","ADC","ROR","ARR","JMP","ADC","ROR","RRA","BVS","ADC","STP","RRA","NOP","ADC","ROR","RRA","SEI","ADC","NOP","RRA","NOP","ADC","ROR","RRA",
	"NOP","STA","NOP","SAX","STY","STA","STX","SAX","DEY","NOP","TXA","XAA","STY","STA","STX","SAX","BCC","STA","STP","AHX","STY","STA","STX","SAX","TYA","STA","TXS","TAS","SHY","STA","SHX","AHX",
	"LDY","LDA","LDX","LAX","LDY","LDA","LDX","LAX","TAY","LDA","TAX","LAX","LDY","LDA","LDX","LAX","BCS","LDA","STP","LAX","LDY","LDA","LDX","LAX","CLV","LDA","TSX","LAS","LDY","LDA","LDX","LAX",
	"CPY","CMP","NOP","DCP","CPY","CMP","DEC","DCP","INY","CMP","DEX","AXS","CPY","CMP","DEC","DCP","BNE","CMP","STP","DCP","NOP","CMP","DEC","DCP","CLD","CMP","NOP","DCP","NOP","CMP","DEC","DCP",
	"CPX","SBC","NOP","ISC","CPX","SBC","INC","ISC","INX","SBC","NOP","SBC","CPX","SBC","INC","ISC","BEQ","SBC","STP","ISC","NOP","SBC","INC","ISC","SED","SBC","NOP","ISC","NOP","SBC","INC","ISC",
}; 

char headersLUT[7][6] = {"iNES", "NES20", "UNIF", "NSF", "NSF2", "FDS", "QD"};

// defined functions to identify information forr the emulation

char getHeader (char* header){
	char headerID = 0;
	if (header[0]=='N' && header[1]=='E' && header[2]=='S' && header[3]==0x1A){
		headerID = iNES; // iNES header
		printf("Identified iNES\n");
		if ((header[7]&0x0C) == 0x08){
			headerID = NES20; // NES 2.0 header
			printf("Identified NES2.0\n");
		}
	}else if(header[0]=='U' && header[1]=='N'&& header[2]=='I' && header[3]=='F'){
		headerID = UNIF; //UNIF header
		printf("Identified UNIF\n");
	}else if(header[0]=='N' && header[1]=='E' && header[2]=='S' && header[3]=='M' && header[4]==0x1A){
		switch (header[5]){
			case 1:
				headerID = NSF;
				printf("Identified NSF\n");
			case 2:
				headerID = NSF2;
				printf("Identified NSF2\n");
		}
	}else if(header[0]=='N' && header[1]=='S'&& header[2]=='F' && header[3]=='E'){
		headerID = NSFE;

	}else if (header[0]=='F' && header[1]=='D' && header[2]=='S' && header[3]==0x1A){
		headerID = FDS;
		printf("Identified FDS\n");
	}else{
		headerID = QD; // assume QD format
		printf("Identified default: QD\n");
	}
	return headerID;
}

void getHeaderInfo(char headerID, char* header){
	switch (headerID){
		case iNES:
		case NES20:
		case UNIF:
		case NSF:
		case NSF2:
		case NSFE:
		case FDS:
		case QD:
	}
}

//defined functions for the emulation

void exec (struct CPU* processor,FILE* program){
	fseek(program, processor->PC,SEEK_SET);
	short int code[1];
	fread(code, 1, 2, program);
	switch (code[0]) {
		// access
		case 0xA2: //LDA (d,x)
			processor->A = 0; // need memory mapping before doing the access opcodes
		// arithmetic
		// bitwise
		// branch
		// compare
		// flags
		// jump
		// other
		// shift
		// stack
		// transfer
	}
}

void powerOn(struct CPU* CPU, struct PPU* PPU){ // pass in system components to initialize them.
						// 
	CPU->PC = 0xfffc; // setting the program counter at it's highest
	CPU->S = 0xfd; // setting the stack pointer at it's highest
	CPU->P = 0b00100000; // setting the 1 flag which is always 1.

	PPU->PPUCTRL=0;
	PPU->PPUMASK=0;
	PPU->PPUSTATUS= PPU->PPUSTATUS & 0b10111111; // PPUSTATUS & 0b10111111 all ones other than the 2nd highest bit will stay,
	PPU->OAMADDR=0;
	PPU->PPUSCROLL=0;
	PPU->PPUADDR=0;
	PPU->PPUDATA=0;
}

int main (int argc, char* argv[]){
	struct CPU CPU;
	struct PPU PPU;

	printf("Got %d arguments.\n", argc);
	for (int i = 0; i < argc; i++){
		printf("got argument(%d): %s\n",i, argv[i]);
	}
	printf("Got path: %s\n", argv[1]);

	if (argc == 1){
		printf("Please provide a path to an nes rom\n");
	}else{
		FILE* rom = fopen(argv[1], "rb");
		if (rom == NULL) { //if the file is empty or failed to open, error
			printf("failed to open file: %s\n", argv[1]);
			return 0;
		}else{
			char header[8];
			fread(header, 1, 8, rom); // push 1 byte 5 times from file[0] to header
			char fileFormat = getHeader(header);
			printf("File format detected: %s\n", headersLUT[fileFormat]);

			if (fileFormat == iNES | fileFormat == NES20 | fileFormat == FDS ){  // allocates the apropriate size for a given header
		                //all 16 byte headers
				char header[16];
				fread(header,1,16,rom);
				printf("header has a size of 16 bytes\n");
			}else if(fileFormat == UNIF){
				//32 byte header
				char header[32];
				fread(header,1,32,rom);
				printf("header has a size of 32 bytes\n");
			}else if (fileFormat == NSF | fileFormat == NSF2){
				char header[0x80];
				fread(header,1,0x80,rom);
				printf("header has a size of 128 bytes\n");
			}else if(fileFormat == NSFE | fileFormat == QD){
				printf("File doesn't have additional info in the header\n");
			}else{
				printf("failed to identify header exited\n");
				return 0;
			}
			char status = 0;
		}
	}


	powerOn(&CPU, &PPU);

	return 0;
}
