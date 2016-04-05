#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "RegFile.h"
#include "Syscall.h"
#include "utils/heap.h"
#include "elf_reader/elf_reader.h"

//Stats

uint32_t DynInstCount = 0;

void write_initialization_vector(uint32_t sp, uint32_t gp, uint32_t start) {
    printf("\n ----- BOOT Sequence ----- \n");
    printf("Initializing sp=0x%08x; gp=0x%08x; start=0x%08x\n", sp, gp, start);
    RegFile[28] = gp;
    RegFile[29] = sp;
    RegFile[31] = start;
    printRegFile();

}

int get_opcode(uint32_t hs) {
    return (hs >> 26);
}

int get_function(uint32_t hs) {
    hs = hs << 26;
    return (hs >> 26);
}

int get_rs(uint32_t hs) {
    hs = hs << 6;
    return (hs >> 27);
}

int get_rt(uint32_t hs) {
    hs = (hs << 11);
    return (hs >> 27);
}

int get_rd(uint32_t hs) {
    hs = (hs << 16);
    return (hs >> 27);
}

int get_immediate(uint32_t hs) {
    hs = (hs << 16);
    hs = (hs >> 16);
}

int main(int argc, char * argv[]) {

    int MaxInst = 0;
    int status = 0;
    uint32_t i;
    uint32_t PC,newPC;
    uint32_t CurrentInstruction;

    if (argc < 2) {
      printf("Input argument missing \n");
      return -1;
    }
    sscanf (argv[2], "%d", &MaxInst);

    //Open file pointers & initialize Heap & Regsiters
    initHeap();
    initFDT();
    initRegFile(0);

    //Load required code portions into Emulator Memory
    status =  LoadOSMemory(argv[1]);
    if(status <0) { return status; }

    //set Global & Stack Pointers for the Emulator
    // & provide startAddress of Program in Memory to Processor
    write_initialization_vector(exec.GSP, exec.GP, exec.GPC_START);

    printf("\n ----- Execute Program ----- \n");
    printf("Max Instruction to run = %d \n",MaxInst);
    PC = exec.GPC_START;
    for(i=0; i<MaxInst ; i++) {
        DynInstCount++;
        CurrentInstruction = readWord(PC,true);
        printRegFile();
    /********************************/
    //Add your implementation here
    /********************************/

        uint32_t opcode = get_opcode(CurrentInstruction);
	uint32_t functionvalue = get_function(CurrentInstruction);
	
	printf("Iteration:%i\n", i);
	printf("The value of opcode is:%zu\n", opcode);
	printf("The value of function is:%zu\n", functionvalue);

        switch(opcode) {
            case 0:                     // SPECIAL = 0
	        switch(functionvalue) {
		    case 32: // ADD
		        
		    break;
		    case 34: // SUB
		    break;
		    case 24: // MULT
		    break;
		    case 18: // MFLO
		    break;
		    case 38: // XOR
		    break;
		    case 0: // SLL
		    break;
		    case 2: // SRL
		    break;
		    case 26: // DIV
		    break;
		    case 16: // MFHI
		    break;
		}
		break;
            case 1:                     // REGIMM = 1
		break;
            case 2:                     // J
		break;
            case 3:                     // JAL
		break;
            case 4:                     // BEQ
		break;
            case 5:                     // BNE
		break;
            case 6:                     // BLEZ
		break;
            case 7:                     // BGTZ
		break;
            case 8:                     // ADDI
		break;
            case 9:                     // ADDIU
		break;
            case 10:                    // SLTI
		break;
            case 11:                    // SLTIU
		break;
            case 12:                    // ANDI
		break;
            case 13:                    // ORI
		break;
            case 14:                    // XORI
		break;
            case 15:                    // LUI
		break;
            case 20:                    // BEQL
		break;
            case 21:                    // BNEL
		break;
            case 22:                    // BLEZL
		break;
            case 32:                    // LB
		break;
            case 33:                    // LH
		break;
            case 34:                    // LWL
		break;
            case 35:                    // LW
		break;
            case 36:                    // LBU
		break;
            case 37:                    // LHU
		break;
            case 38:                    // LWR
		break;
            case 40:                    // SB
		break;
            case 41:                    // SH
		break;
            case 42:                    // SWL
		break;
            case 43:                    // SW
		break;
            case 46:                    // SWR
		break;
        }

        PC = PC + 4;
    } //end fori


    //Close file pointers & free allocated Memory
    closeFDT();
    CleanUp();
    return 1;
}
