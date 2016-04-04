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

        switch(opcode) {
            case 0:                     // SPECIAL = 0
            
            case 1:                     // REGIMM = 1

            case 2:                     // J
            case 3:                     // JAL
            case 4:                     // BEQ
            case 5:                     // BNE
            case 6:                     // BLEZ
            case 7:                     // BGTZ
            case 8:                     // ADDI
            case 9:                     // ADDIU
            case 10:                    // SLTI
            case 11:                    // SLTIU
            case 12:                    // ANDI
            case 13:                    // ORI
            case 14:                    // XORI
            case 15:                    // LUI
            case 20:                    // BEQL
            case 21:                    // BNEL
            case 22:                    // BLEZL
            case 32:                    // LB
            case 33:                    // LH
            case 34:                    // LWL
            case 35:                    // LW
            case 36:                    // LBU
            case 37:                    // LHU
            case 38:                    // LWR
            case 40:                    // SB
            case 41:                    // SH
            case 42:                    // SWL
            case 43:                    // SW
            case 46:                    // SWR

        }

        PC = PC + 4;
    } //end fori
    
    
    //Close file pointers & free allocated Memory
    closeFDT();
    CleanUp();
    return 1;
}
