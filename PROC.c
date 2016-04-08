#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "RegFile.h"
#include "Syscall.h"
#include "utils/heap.h"
#include "elf_reader/elf_reader.h"

//Stats

uint32_t DynInstCount = 0;
uint32_t hi = 32;
uint32_t lo = 33;
int64_t ans64;
uint64_t uans64;
uint32_t temp;
uint32_t boolJump = 0;

void write_initialization_vector(uint32_t sp, uint32_t gp, uint32_t start) {
    printf("\n ----- BOOT Sequence ----- \n");
    printf("Initializing sp=0x%08x; gp=0x%08x; start=0x%08x\n", sp, gp, start);
    RegFile[28] = gp;
    RegFile[29] = sp;
    RegFile[31] = start;
    printRegFile();

}

 uint32_t get_opcode(uint32_t hs) {           // find opcode - first step of decoding
     return (hs >> 26);
 }
 
 uint32_t get_function(uint32_t hs) {         // get function if opcode 0 or 1
     hs = hs << 26;
     return (hs >> 26);
 }
 
 uint32_t get_rs(uint32_t hs) {       // rs is always operated on
     hs = hs << 6;
     return (hs >> 27);
 }
 
 uint32_t get_rt(uint32_t hs) {       // destination register for immediates, else it's operated on
     hs = (hs << 11);
     return (hs >> 27);
 }
 
 uint32_t get_rd(uint32_t hs) {       // destination register for special
     hs = (hs << 16);
     return (hs >> 27);
 }

int32_t get_immediate(int32_t hs) {            // immediate constant to operate on
    hs = (hs << 16);                  // still need to test for negatives
    return (hs >> 16);
}

int32_t get_offset(int32_t hs) {            // immediate constant to operate on
    hs = (hs << 16);                  // still need to test for negatives
    return (hs >> 16);
}

 uint32_t get_sa(uint32_t hs) {            // get shift amt for shifts
     hs = (hs << 21);       
     return (hs >> 27);
 }
 
 uint32_t get_ii(uint32_t hs) {            // get shift amt for shifts
     hs = (hs << 26);       
     return (hs >> 26);
 }

int main(int argc, char * argv[]) {

    int MaxInst = 0;
    int status = 0;
    uint32_t i;
    uint32_t PC, jumpAddress, linkAddress;
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
    uint32_t rs = get_rs(CurrentInstruction);
    uint32_t base = get_rs(CurrentInstruction);
    uint32_t rt = get_rt(CurrentInstruction);
    uint32_t rd = get_rd(CurrentInstruction);           // destination register
    int32_t imme = get_immediate(CurrentInstruction);
    int32_t offset = get_offset(CurrentInstruction);
    uint32_t sa = get_sa(CurrentInstruction);           // shift amount
    uint32_t instr_index = get_ii(CurrentInstruction);           // shift amount

    
    printf("Iteration:%i\n", i);
    printf("The value of opcode is:%d\n", opcode);
    printf("The value of function is:%d\n", functionvalue);
    printf("The value of rs is:%d\n", rs);
    printf("The value of rt is:%d\n", rt);
    printf("The value of rd is:%d\n", rd);
    printf("The value of imme is:%i\n", imme);
    printf("The value of offset is:%i\n", offset);
    printf("The value of sa is:%d\n", sa);
    printf("The value of instr_index is:%d\n", instr_index);

        switch(opcode) {
            case 0:                     // SPECIAL = 0

            switch(functionvalue) {
                case 0: // SLL and NOP
                    if (sa == 0 && rd == 0 && rt == 0) {                // NOP
                        break;
                    }
                    else {
                        RegFile[rd] = RegFile[rt] << sa;       // SLL
                        break;
                    }

                case 2: // SRL
                    RegFile[rd] = (int32_t)((uint32_t) RegFile[rt] >> sa);      // TEST THIS
                    break;

                case 3: // SRA
                    RegFile[rd] = RegFile[rt] >> sa;
                    break;

                case 4: // SLLV
                    RegFile[rd] = RegFile[rt] << RegFile[rs];
                    break;

                case 6: // SRLV
                    RegFile[rd] = (int32_t) ((uint32_t) RegFile[rt] >> RegFile[rs]);        // TEST
                    break;

                case 7: // SRAV
                    RegFile[rd] = RegFile[rt] >> RegFile[rs];
                    break;

                case 8: // JR
                    boolJump++;
                    jumpAddress = RegFile[rs];
                    break;

                case 9: // JALR
                    boolJump++;
                    RegFile[rd] = PC + 8;
                    jumpAddress = RegFile[rs];
                    break;

                case 16: // MFHI
                    RegFile[rd] = RegFile[hi];
                    break;

                case 17: // MTHI
                    RegFile[hi] = RegFile[rs];
                    break;

                case 18: // MFLO
                    RegFile[rd] = RegFile[lo];
                    break;

                case 19: // MTLO
                    RegFile[lo] = RegFile[rs];
                    break;

                case 24: // MULT
                    ans64 = (int64_t) RegFile[rs] * (int64_t) RegFile[rt];
                    RegFile[lo] = (int32_t)((ans64 << 32) >> 32);
                    RegFile[hi] = (int32_t)(ans64 >> 32);
                    break;

                case 25: // MULTU
                    uans64 = (uint64_t) RegFile[rs] * (uint64_t) RegFile[rt];
                    RegFile[lo] = (uint32_t)((uans64 << 32) >> 32);
                    RegFile[hi] = (uint32_t)(uans64 >> 32);
                    break;

                case 26: // DIV
                    RegFile[lo] = RegFile[rs] / RegFile[rt];
                    RegFile[hi] = RegFile[rs] % RegFile[rt];
                    break;

                case 27:  // DIVU
                    RegFile[lo] = (uint32_t) RegFile[rs] / (uint32_t) RegFile[rt];
                    RegFile[hi] = (uint32_t) RegFile[rs] % (uint32_t) RegFile[rt];
                    break;

                case 32:             // ADD
                    RegFile[rd] = RegFile[rs] + RegFile[rt];
                    break;

                case 33:            // ADDU
                    RegFile[rd] = RegFile[rs] + RegFile[rt];
                    break;

                case 34:            // SUB
                    RegFile[rd] = RegFile[rs] - RegFile[rt];
                    break;

                case 35:            // SUBU
                    RegFile[rd] = RegFile[rs] - RegFile[rt];
                    break;

                case 36:  // AND
                    RegFile[rd] = RegFile[rs] & RegFile[rt];
                    break;

                case 37: // OR
                    RegFile[rd] = RegFile[rs] | RegFile[rt];
                    break;

                case 38: // XOR
                    RegFile[rd] = RegFile[rs] ^ RegFile[rt];
                    break;

                case 39: // NOR
                    RegFile[rd] = !(RegFile[rs] | RegFile[rt]);
                    break;

                case 42: // SLT
                    if (RegFile[rs] < RegFile[rt]){
                        RegFile[rd] = 1;
                    } else {
                        RegFile[rd] = 0;
                    }
                    break;

                case 43: // SLTU
                    if ((uint32_t) RegFile[rs] < (uint32_t) RegFile[rt]){
                        RegFile[rd] = 1;
                    } else {
                        RegFile[rd] = 0;
                    }
                    break;

            }
            break;

            case 1:                     // REGIMM = 1

            switch(rt) {
                case 0:                 // BLTZ
                if (RegFile[rs] < 0) {
                    boolJump++;
                    jumpAddress = PC + 4 + (offset << 2);
                }
                    break;

                case 1:                 // BGEZ
                if (RegFile[rs] >= 0) {
                    boolJump++;
                    jumpAddress = PC + 4 + (offset << 2);
                }
                    break;

                case 16:                // BLTZAL
                if (RegFile[rs] < 0) {
                    boolJump++;
                    jumpAddress = PC + 4 + (offset << 2);
                    linkAddress = PC + 8;
                }
                    break;

                case 17:                // BGEZAL
                if (RegFile[rs] >= 0) {
                    boolJump++;
                    jumpAddress = PC + 4 + (offset << 2);
                    linkAddress = PC + 8;
                }
                    break;
            }
                break;
            case 2:                     // J
                boolJump++;
                jumpAddress = instr_index;
                break;
            case 3:                     // JAL
                boolJump++;
                jumpAddress = instr_index;
                linkAddress = (PC + 8);
                break;
            case 4:                     // BEQ
                if (RegFile[rs] == RegFile[rt]) {
                    boolJump++;
                    jumpAddress = PC + 4 + (offset << 2);
                }
                break;
            case 5:                     // BNE
                if (RegFile[rs] != RegFile[rt]) {
                    boolJump++;
                    jumpAddress = PC + 4 + (offset << 2);
                }
                break;
            case 6:                     // BLEZ
                if (RegFile[rs] <= 0) {
                    boolJump++;
                    jumpAddress = PC + 4 + (offset << 2);
                }
                break;
            case 7:                     // BGTZ
                if (RegFile[rs] > 0) {
                    boolJump++;
                    jumpAddress = PC + 4 + (offset << 2);
                }
                break;
            case 8:                     // ADDI
                RegFile[rt] = RegFile[rs] + imme;
                break;

            case 9:                     // ADDIU            // U means allow overflow
                RegFile[rt] = RegFile[rs] + imme;
                break;

            case 10:                    // SLTI
                if (RegFile[rs] < imme) {
                    RegFile[rt] = 1;
                }
                else {
                    RegFile[rt] = 0;
                }
                break;
            case 11:                    // SLTIU
                if ((uint32_t) RegFile[rs] < (uint32_t) imme) {
                    RegFile[rt] = 1;
                }
                else {
                    RegFile[rt] = 0;
                }
                break;
            case 12:                    // ANDI
                RegFile[rt] = RegFile[rs] & imme;
                break;
            case 13:                    // ORI
                RegFile[rt] = RegFile[rs] | imme;
                break;
            case 14:                    // XORI
                RegFile[rt] = RegFile[rs] ^ imme;
                break;
            case 15:                    // LUI
                RegFile[rt] = (imme << 16);
                break;

            // TOLD THAT WE DONT NEED TO IMPLEMENT BRANCH LIKELY INSTRUCTIONS
            // case 20:                    // BEQL
            //     if (RegFile[rs] == RegFile[rt]) {
            //         boolJump++;
            //         jumpAddress = PC + 4 + (offset << 2);
            //     }
            //     else {
            //         PC = PC + 4;
            //     }
            //     break;
            // case 21:                    // BNEL
            //     if (RegFile[rs] != RegFile[rt]) {
            //         boolJump++;
            //         jumpAddress = PC + 4 + (offset << 2);
            //     }
            //     else {
            //         PC = PC + 4;
            //     }
            //     break;

            // case 22:                    // BLEZL
            //     if (RegFile[rs] <= 0) {
            //         boolJump++;
            //         jumpAddress = PC + 4 + (offset << 2);
            //     }
            //     else {
            //         PC = PC + 4;
            //     }
            //     break;

            case 32:                    // LB
                RegFile[rt] = readByte((base + offset), false);
                break;
            case 33:                    // LH
                RegFile[rt] = readByte(base + offset, false);
                RegFile[rt] = RegFile[rt] << 8;
                RegFile[rt] = RegFile[rt] | readByte(base + offset + 1, false);
                break;
            case 34:                    // LWL
                if ((base + offset) % 4 == 0) {
                    RegFile[rt] = readWord(base + offset, false);
                }
                else if ((base + offset) % 4 == 1) {
                    RegFile[rt] = ((RegFile[rt] & 0x000000FF) | 
                        (0xFFFFFF00 & readWord(base + offset, false)));
                }
                else if ((base + offset) % 4 == 2) {
                    RegFile[rt] = ((RegFile[rt] & 0x0000FFFF) | 
                        (0xFFFF0000 & readWord(base + offset, false)));
                }
                else if ((base + offset) % 4 == 3) {
                    RegFile[rt] = ((RegFile[rt] & 0x00FFFFFF) | 
                        (0xFF000000 & readWord(base + offset, false)));
                }
                break;
            case 35:                    // LW
                RegFile[rt] = readWord(base + offset, false);
                break;
            case 36:                    // LBU
                RegFile[rt] = (uint32_t) readByte(base + offset, false);
                break;
            case 37:                    // LHU
                RegFile[rt] = (uint32_t) readByte(base + offset, false);
                RegFile[rt] = (uint32_t) RegFile[rt] << 8;
                RegFile[rt] = (uint32_t) RegFile[rt] | (uint32_t) readByte(base + offset + 1, false);
                break;
            case 38:                    // LWR
                if ((base + offset) % 4 == 3) {
                    RegFile[rt] = readWord(base + offset - 3, false);
                }
                if ((base + offset) % 4 == 2) {
                    RegFile[rt] = ((RegFile[rt] & 0xFF000000) | 
                        (0x00FFFFFF & readWord(base + offset - 3, false)));
                }
                if ((base + offset) % 4 == 1) {
                    RegFile[rt] = ((RegFile[rt] & 0xFFFF0000) | 
                        (0x0000FFFF & readWord(base + offset - 3, false)));
                }
                if ((base + offset) % 4 == 0) {
                    RegFile[rt] = ((RegFile[rt] & 0xFFFFFF00) | 
                        (0x000000FF & readWord(base + offset - 3, false)));
                }

                break;
            case 40:                    // SB
                writeByte(base + offset, RegFile[rt], false);
                break;
            case 41:                    // SH
                writeByte(base + offset, (RegFile[rt] << 16) >> 24, false);
                writeByte(base + offset + 1, (RegFile[rt] << 24) >> 24, false);  // +1 or +4 ???
                break;
            case 42:                    // SWL
                if ((base + offset) % 4 == 0) {
                    writeWord(base + offset, RegFile[rt], false);
                }
                else if ((base + offset) % 4 == 1) {
                    writeWord(base + offset, (readWord(base + offset, false) & 0x000000FF) |
                        (RegFile[rt] &  0xFFFFFF00), false);
                }
                else if ((base + offset) % 4 == 2) {
                    writeWord(base + offset, (readWord(base + offset, false) & 0x0000FFFF) |
                        (RegFile[rt] &  0xFFFF0000), false);
                }
                else if ((base + offset) % 4 == 3) {
                    writeWord(base + offset, (readWord(base + offset, false) & 0x00FFFFFF) |
                        (RegFile[rt] &  0xFF000000), false);
                }
                break;
            case 43:                    // SW
                writeWord(base + offset, RegFile[rt], false);
                break;
            case 46:                    // SWR
                if ((base + offset) % 4 == 3) {
                    writeWord(base + offset - 3, RegFile[rt], false);
                }
                if ((base + offset) % 4 == 2) {
                    writeWord(base + offset - 3, (readWord(base + offset - 3, false) & 0xFF000000) |
                        (RegFile[rt] &  0x00FFFFFF), false);
                }
                if ((base + offset) % 4 == 1) {
                    writeWord(base + offset - 3, (readWord(base + offset - 3, false) & 0xFFFF0000) |
                        (RegFile[rt] &  0x0000FFFF), false);
                }
                if ((base + offset) % 4 == 0) {
                    writeWord(base + offset - 3, (readWord(base + offset - 3, false) & 0xFFFFFF00) |
                        (RegFile[rt] &  0x000000FF), false);
                }
                break;
        }
        if (boolJump > 0) {                         // for branch delay
            if (boolJump > 1) {
                PC = (jumpAddress);
                boolJump = 0;
            }
            else {
                boolJump++;
                PC = PC + 4;
            }
        }
        else {
            PC = PC + 4;
        }
    } //end fori


    //Close file pointers & free allocated Memory
    closeFDT();
    CleanUp();
    return 1;
}