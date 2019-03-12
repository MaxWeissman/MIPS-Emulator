
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "RegFile.h"
#include "Syscall.h"
#include "utils/heap.h"
#include "elf_reader/elf_reader.h"
#include "Funcs.h" // Header for function declarations

//Stats

int PCG;//PC Global
int branchCorrection = 4; // Global var
uint32_t DynInstCount = 0;

void write_initialization_vector(uint32_t sp, uint32_t gp, uint32_t start) {
        printf("\n ----- BOOT Sequence ----- \n");
        printf("Initializing sp=0x%08x; gp=0x%08x; start=0x%08x\n", sp, gp, start);
        RegFile[28] = gp;
        RegFile[29] = sp;
        RegFile[31] = start;
        printRegFile();

    }

int main(int argc, char * argv[]) {
    int stoopid=0;//delay flag
    // Function Declarations - Non-immediate
    int (*funcname[]) (int x, int y, int z, int w, int v) = {add,addu,sub,subu,divv,divu,mult,multu,mfhi,mflo,mthi,mtlo,and,xor,nor,or,sll,sllv,slt,sltu,
        sra,srav,srl,srlv,jalr,jr};
    int funccode[] = {32,33,34,35,26,27,24,25,16,18,17,19,36,38,39,37,0,4,42,43,3,7,2,6,9,8};
    
    // Function Declarations - Immediate/Offset
    int (*ifuncname[]) (int x, int y, int z, int w) = {addi,addiu,andi,xori,ori,slti,sltiu,beq,beql,bgtz,blez,blezl,bne,bnel,lb,lbu,lh,lhu,lui,lw,lwl,lwr,sb,sh,sw,swl,swr,j,jal};
    int ifunccode[] = {8,9,12,14,13,10,11,4,20,7,6,22,5,21,32,36,33,37,15,35,34,38,40,41,43,42,46,2,3};
    
    // Function Declarations - Immediate/Offset-->Opcode=1
    int (*ifuncname1[])(int x, int y, int z, int w) = {bgez,bgezal,bltz,bltzal};
    int ifunccode1[] = {1,17,0,16};
    
    int MaxInst = 0;
    int status = 0;
    uint32_t i; 
    uint32_t PC,newPC = 0;
    uint32_t CurrentInstruction;
    uint32_t hi = 0, lo = 0;

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
    if(status < 0) { return status; }
    
    //set Global & Stack Pointers for the Emulator
    // & provide startAddress of Program in Memory to Processor
    write_initialization_vector(exec.GSP, exec.GP, exec.GPC_START);

    printf("\n ----- Execute Program ----- \n");
    printf("Max Instruction to run = %d \n",MaxInst);
    fflush(stdout);
    PC = exec.GPC_START;
    for(i=0; i<MaxInst ; i++) {
        DynInstCount++;
        CurrentInstruction = readWord(PC,false);  
        //printRegFile();
    /********************************/
    //Add your implementation here
    /********************************/
        
        // Breaks down the categories
        int opcode = CurrentInstruction >> 26; // isolates first 6 bits (31-26)
        int rs = (CurrentInstruction << 6) >> 27; // isolates bits (25-21)
        int rt = (CurrentInstruction << 11) >> 27; // isolates bits (20-16)
        int rd = (CurrentInstruction << 16) >> 27; // isolates bits (15-11)
        int sa = (CurrentInstruction << 21) >> 27; // isolates bits (10-6)
        int myfunc = (CurrentInstruction << 26) >> 26; // represents 6-bit number indicating 'func' (5-0)

        
         if (opcode == 0) { // if SPECIAL = 0, go on to test 'func' (last 6 bits)
            
            int k=0;
            if(stoopid == 0){
            
              if(myfunc == 12){ // syscall
                  printf("\n");
                  SyscallExe(RegFile[2]);
                  RegFile[2]=0;
                  printf("\n");
                  
              }
              else{
                 while (funccode[k] != myfunc) {
                      k++;
                  } // k is now the index of the func
            
                      stoopid =(*funcname[k]) (rd, rs, rt, sa, PC); // Calls the corresponding function
                
                
              }
            }
            if(stoopid == 1){
              if(myfunc == 12){ //syscall
                  printf("\n");
                  SyscallExe(RegFile[2]);
                  RegFile[2]=0;
                  printf("\n");
              }
              else{
                 while (funccode[k] != myfunc) {
                      k++;
                  } // k is now the index of the func
            
                      stoopid =(*funcname[k]) (rd, rs, rt, sa, PC); // Calls the corresponding function
               }
                PC = PCG;
                stoopid = 0;
                PCG = 0;
            }
            else if(stoopid == 2){
                PC += 4;
                stoopid -= 1;
                printf("right before 'continue' in NON-immed – this is basically a delay. PC = %x\n",PC);
                
            }
            else{
                PC += 4;
            }
            
        }else{// if SPECIAL ? 0, treat as 'immediate or offset' and get opcode
            int imm = (CurrentInstruction << 16) >> 16; // isolates bits (15-0)
            int instr_index = (CurrentInstruction << 6) >> 6;
            int k=0;
            if(stoopid == 0){
                if(opcode == 1){
                  while (ifunccode1[k] != rt) {
                     k++;
                     } // k is now the index of the (immediate) func  
                  stoopid = (*ifuncname1[k]) (rt, rs, imm, PC);                  
                }
                else{
                    while (ifunccode[k] != opcode) {
                        k++;
                    } // k is now the index of the (immediate) func

                    if((opcode == 2)||(opcode == 3)){
                        printf("FUKK.  opcode = %d; \n",opcode);
                        stoopid = (*ifuncname[k]) (PC,instr_index,rs,rt);
                    }else{
                        stoopid = (*ifuncname[k]) (rt, rs, imm, PC); // Calls the corresponding (immediate) function
                    }
                }
            }
              if(stoopid == 1){//condition to branch 
                if(opcode == 1){
                  while (ifunccode1[k] != rt) {
                     k++;
                     } // k is now the index of the (immediate) func  
                  stoopid = (*ifuncname1[k]) (rt, rs, imm, PC);                  
                }else{
                  while (ifunccode[k] != opcode) {
                     k++;
                     } // k is now the index of the (immediate) func
                
                    stoopid = (*ifuncname[k]) (rt, rs, imm, PC); // Calls the corresponding (immediate) function
                }
                    PC = PCG;
                    PCG = 0;
                    stoopid -= 0;
              } 
              
              else if(stoopid == 3){//Condition to skip delay slot instruction
                    PC = PCG;
                          //i++;
                    stoopid = 0;
              }
              else if(stoopid == 2){//Condition to execute delay slot
                  PC +=4;
                  stoopid -= 1;
                  printf("right before 'continue' in immed - this is basically a delay. PC = %x\n",PC);
                  continue;

              }
              else{
                  PC += 4;
              }
        
        }
        
        printf("---start of reg printout---\n");
        printRegFile();
        printf("\nPC: dec= %d;  hex = %x\n",PC,PC);
        printf("---end of reg printout---\n");
        
    } //end fori
    
    //Close file pointers & free allocated Memory
    closeFDT();
    CleanUp();
    return 1;
}


// FUNCTION DEFINITIONS - Non-immediate

int add(rd,rs,rt,sa,PC){
    RegFile[rd] = RegFile[rs] + RegFile[rt];
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int addu(rd,rs,rt,sa,PC){
    RegFile[rd] = (uint32_t)RegFile[rs] + (uint32_t)RegFile[rt]; // type cast to unsigned int
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int sub(rd,rs,rt,sa,PC){
    RegFile[rd] = RegFile[rs] - RegFile[rt];
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int subu(rd,rs,rt,sa,PC){
    RegFile[rd] = (uint32_t)RegFile[rs] - (uint32_t)RegFile[rt];
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int divv(rd, rs, rt, sa,PC){
    int temp = RegFile[rs] / RegFile[rt];
    RegFile[32] = RegFile[rs] % RegFile[rt]; // Put remainder into HI
    if (temp <= 0xFFFFFFFF) {
        RegFile[33] = temp;
    } else {
        int temp2 = (temp<<32)>>32; //lower bits
        RegFile[33] = temp2; // put into LO
    }
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int divu(rd,rs,rt,sa,PC){
    uint32_t temp = (uint32_t)RegFile[rs] / (uint32_t)RegFile[rt];
    RegFile[32] = (uint32_t)RegFile[rs] % (uint32_t)RegFile[rt]; // Put remainder into HI
    if (temp <= 0xFFFFFFFF) {
        RegFile[33] = temp;
    } else {
        uint32_t temp2 = (temp<<32)>>32; //lower bits
        RegFile[33] = temp2; // put into LO
    }
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int mult(rd,rs,rt,sa,PC){
    int temp = RegFile[rs] * RegFile[rt];
    if (temp <= 0xFFFFFFFF) {
        RegFile[33] = temp;
    } else {
        int temp2=(temp<<32)>>32;//lower bits
        int temp3=(temp>>32);//upper bits
        RegFile[32]=temp3; // put into HI
        RegFile[33]=temp2; // put into LO
    }
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int multu(rd,rs,rt,sa,PC){
    uint32_t temp = (uint32_t)RegFile[rs] * (uint32_t)RegFile[rt];
    if (temp <= 0xFFFFFFFF) {
        RegFile[33] = temp;
    } else {
        uint32_t temp2=(temp<<32)>>32;//lower bits
        uint32_t temp3=(temp>>32);//upper bits
        RegFile[32]=temp3;
        RegFile[33]=temp2;
    }
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int mfhi(rd,rs,rt,sa,PC){
    RegFile[rd]=RegFile[32];
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int mflo(rd,rs,rt,sa,PC){
    RegFile[rd]=RegFile[33];
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int mthi(rd,rs,rt,sa,PC){
    RegFile[32] = RegFile[rs];
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int mtlo(rd,rs,rt,sa,PC){
    RegFile[33] = RegFile[rs];
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int and(rd,rs,rt,sa,PC){
    RegFile[rd] = RegFile[rt] & RegFile[rs];
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int xor(rd,rs,rt,sa,PC){
    RegFile[rd] = RegFile[rs] ^ RegFile[rt];
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int nor(rd,rs,rt,sa,PC){
    RegFile[rd] = !(RegFile[rt] | RegFile[rs]);
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int or(rd,rs,rt,sa,PC){
    RegFile[rd] = RegFile[rt] | RegFile[rs];
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int sll(rd,rs,rt,sa,PC){
    if((rd==0)&&(rs==0)&&(rt==0)){
        printf("\n");
        printf("sll (nop)\n");
        printf("\n");
    }else{
    RegFile[rd] = (uint32_t)RegFile[rt] << sa;
    }
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int sllv(rd,rs,rt,sa,PC){
    RegFile[rd] = (uint32_t)RegFile[rt] << RegFile[rs];
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int slt(rd,rs,rt,sa,PC){
    if(RegFile[rs] < RegFile[rt]){
      RegFile[rd]=1;
    }else{
      RegFile[rd]=0;
    }
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int sltu(rd,rs,rt,sa,PC){
    if((uint32_t)RegFile[rs] < (uint32_t)RegFile[rt]){
      RegFile[rd]=1;
    }else{
      RegFile[rd]=0;
    }
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int sra(rd,rs,rt,sa,PC){
     RegFile[rd] = RegFile[rt] >> sa;
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int srav(rd,rs,rt,sa,PC){
    RegFile[rd] = RegFile[rt] >> RegFile[rs];    
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int srl(rd,rs,rt,sa,PC){
    RegFile[rd] = (uint32_t)RegFile[rt] >> sa;
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int srlv(rd,rs,rt,sa,PC){
    RegFile[rd] = (uint32_t)RegFile[rt] >> RegFile[rs];
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //

int jalr(rd,rs,rt,sa,PC){
    int temp = RegFile[rs];
    int targetoffs = temp << 2;
    RegFile[rd] = PC + 8;
    //PCG = PC + targetoffs;
    PCG = temp;
    printf("temp: 0x%x;  targofs:  0x%x;  pcg: 0x%x\n",temp,targetoffs,PCG);
    return 2;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int jr(rd,rs,rt,sa,PC){
    PCG = RegFile[rs];
    return 2;
}
// ------------------------------------------------- //
// ------------------------------------------------- //


// ------------------------------------------------- //
// ------------------------------------------------- //

// FUNCTION DEFINITIONS - Immediate/Offset
int addi(rt,rs,imm,PC){
    RegFile[rt] = RegFile[rs] + imm;
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int addiu(rt,rs,imm,PC){
    RegFile[rt] = (uint32_t)RegFile[rs] + (uint32_t)imm;
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int andi(rt,rs,imm,PC){
    RegFile[rt] = RegFile[rs] + imm;
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int xori(rt,rs,imm,PC){
    RegFile[rt] = RegFile[rs] ^ imm;
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int ori(rt,rs,imm,PC){
    RegFile[rt] = RegFile[rs] | imm;
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int slti(rt,rs,imm,PC){
    RegFile[rt] = RegFile[rs] + imm;
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int sltiu(rt,rs,imm,PC){
    RegFile[rt] = (uint32_t)RegFile[rs] + (uint32_t)imm;
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int beq(rt,rs,imm,PC){
    int imm32 = (imm<<16)>>16; // sign-extends 16-bit imm to be a 32-bit value
    int targetoffs = imm32 << 2;
    if (RegFile[rs] == RegFile[rt]) {
        PCG = targetoffs + PC + branchCorrection;
        printf("beq condition met TRUE\n");
        return 2;
    }else{
        printf("beq condition NOT MET - FALSE\n");
        printf("imm: 0x%x;  imm32:  0x%x;  targetoffs: 0x%x\n",imm,imm32,targetoffs);
        return 0;
    }
    
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int beql(rt,rs,imm,PC){
    int imm32 = (imm<<16)>>16; // sign-extends 16-bit imm to be a 32-bit value
    int targetoffs = imm32 << 2;
     if (RegFile[rs] == RegFile[rt]) {
        PCG = targetoffs + PC + branchCorrection;
        
        return 2; 
     }else{  
        return 3;
    }
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int bgez(rt,rs,imm,PC){
    int imm32 = (imm<<16)>>16; // sign-extends 16-bit imm to be a 32-bit value
     int targetoffs = imm32 << 2;
      if (RegFile[rs] >= 0) {
        PCG = targetoffs + PC + branchCorrection;
        return 2;
      }else{
        return 0;
    }   

}
// ------------------------------------------------- //
// ------------------------------------------------- //
int bgezal(rt,rs,imm,PC){
    int imm32 = (imm<<16)>>16; // sign-extends 16-bit imm to be a 32-bit value
     int targetoffs = imm32 << 2;
     RegFile[31] = PC + 8;
      if (RegFile[rs] >= 0) {
        PCG = targetoffs + PC + branchCorrection;
        return 2;
      }else{
        return 0;
    }

}
// ------------------------------------------------- //
// ------------------------------------------------- //
int bgtz(rt,rs,imm,PC){
    int imm32 = (imm<<16)>>16; // sign-extends 16-bit imm to be a 32-bit value
    int targetoffs = imm32 << 2;
      if (RegFile[rs] > 0) {
        PCG = targetoffs + PC + branchCorrection;
        return 2;
      }else{
          return 0;
    }
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int blez(rt,rs,imm,PC){
    int imm32 = (imm<<16)>>16; // sign-extends 16-bit imm to be a 32-bit value
     int targetoffs = imm32 << 2;
      if (RegFile[rs] <= 0) {
        PCG = targetoffs + PC + branchCorrection;
        return 2;
      }else{
        return 0;
    }   

}
// ------------------------------------------------- //
// ------------------------------------------------- //
int blezl(rt,rs,imm,PC){
    int imm32 = (imm<<16)>>16; // sign-extends 16-bit imm to be a 32-bit value
     int targetoffs = imm32 << 2;
      if (RegFile[rs] <= 0) {
        PCG = targetoffs + PC + branchCorrection;
        return 2;
      }else{
        return 3;
    }   

}
// ------------------------------------------------- //
// ------------------------------------------------- //
int bltz(rt,rs,imm,PC){
    int imm32 = (imm<<16)>>16; // sign-extends 16-bit imm to be a 32-bit value
     int targetoffs = imm32 << 2;
      if (RegFile[rs] > 0) {
        PCG = targetoffs + PC + branchCorrection;
        return 2;
      }else{
        return 0;
    }    
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int bltzal(rt,rs,imm,PC){
    int imm32 = (imm<<16)>>16; // sign-extends 16-bit imm to be a 32-bit value
      RegFile[31] = PC + 8;
      int targetoffs = imm32 << 2;
      if (RegFile[rs] > 0) {
        PCG = targetoffs + PC + branchCorrection;
        return 2;
      }else{
        return 0;
    }    
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int bne(rt,rs,imm,PC){
    int imm32 = (imm<<16)>>16; // sign-extends 16-bit imm to be a 32-bit value
     int targetoffs = imm32 << 2;
    //printf("imm: 0x%x;  targetoffs: 0x%x;  PC = 0x%x;  PC + targoffs: 0x%x \n", imm, targetoffs, PC, PC+targetoffs);
      if (RegFile[rs] != RegFile[rt]) {
        PCG = targetoffs + PC + branchCorrection;
        return 2;
      }else{
        return 0;
    }
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int bnel(rt,rs,imm,PC){
    int imm32 = (imm<<16)>>16; // sign-extends 16-bit imm to be a 32-bit value
     int targetoffs = imm32 << 2;
      if (RegFile[rs] != RegFile[rt]) {
        PCG = targetoffs + PC + branchCorrection;
        return 2;
      }else{
        return 3;
    }
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int lb(rt,rs,imm,PC){
    int imm32 = (imm<<16)>>16; // sign-extends 16-bit imm to be a 32-bit value
    int temp = RegFile[rs]+imm32;
    RegFile[rt]=readByte(temp,false);
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int lbu(rt,rs,imm,PC){
    int imm32 = (imm<<16)>>16; // sign-extends 16-bit imm to be a 32-bit value
    int temp = RegFile[rs]+(uint32_t)imm32;
    RegFile[rt]=(uint32_t)readByte(temp,false);
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int lh(rt,rs,imm,PC){
    int imm32 = (imm<<16)>>16;
    int addr = RegFile[rs] + imm32;
    RegFile[rt] = readWord(addr,false); 
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int lhu(rt,rs,imm,PC){
    int imm32 = (imm<<16)>>16;
    int addr = (uint32_t)RegFile[rs] + (uint32_t)imm32;
    RegFile[rt] = (uint32_t)readWord(addr,false);
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int lui(rt,rs,imm,PC){
    RegFile[rt] = imm << 16;
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int lw(rt,rs,imm,PC){//rs is 'base'
    int imm32 = (imm<<16)>>16; // sign-extends 16-bit imm to be a 32-bit value
    int temp = RegFile[rs]+imm32;
//    printf("rs: %d;   rt: %d;  imm32: dec= %d  hex = 0x%x\n", rs,rt,imm32,imm32);
//    printf("regfile(rs): 0x%x;  regfile(rt): 0x%x\n",RegFile[rs],RegFile[rt]);
//    printf("regfile[rs]+imm32: %x\n",temp);
//    printf("readword of that: %x\n", readWord(temp,true));
    RegFile[rt] = readWord(temp,false);
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int lwl(rt,rs,imm,PC){
    int imm32 = (imm<<16)>>16; // sign-extends 16-bit imm to be a 32-bit value
    int r;
    int temp = ((RegFile[rs]+imm32)<<30)>>30;
    int temp2 = RegFile[rs]+imm32;
    if(temp == 0){
        RegFile[rt] = readWord(temp2,false);
    }else if(temp == 1){
        r = ((readWord(temp2,false))>>4)<<4;
        RegFile[rt] = r | RegFile[rt];
    }else if(temp == 2){
        r = (readWord(temp2,false)>>8)<<8;
        RegFile[rt] = r | RegFile[rt];
    }else{
        r = (readWord(temp2,false)>>12)<<12;
        RegFile[rt] = r | RegFile[rt];
    }
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int lwr(rt,rs,imm,PC){
    int imm32 = (imm<<16)>>16; // sign-extends 16-bit imm to be a 32-bit value
    int r;
    int temp = ((RegFile[rs]+imm32)<<30)>>30;
    int temp2 = RegFile[rs]+imm32;
    if(temp == 0){
        r = (readWord(temp2,false)<<12)>>12;
        RegFile[rs] = readWord(temp2,false);
    }else if(temp == 1){
        r = (readWord(temp2,false)<<8)>>8;
        RegFile[rt] = r | RegFile[rt];
    }else if(temp == 2){
        r = (readWord(temp2,false)<<8)>>8;
        RegFile[rt] = r | RegFile[rt];
    }else{
        r = (readWord(temp2,false)<<4)>>4;
    }
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int sb(rt,rs,imm,PC){
    int imm32 = (imm<<16)>>16; // sign-extends 16-bit imm to be a 32-bit value
    int temp = RegFile[rs]+imm32;
    int data = RegFile[rt];
    writeByte(temp,data,false);
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int sh(rt,rs,imm,PC){
    int imm32 = (imm<<16)>>16;
    int addr = RegFile[rs] + imm32;
    int data = RegFile[rt];
    writeWord(addr, data,false);
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int sw(rt,rs,imm,PC){
    int imm32 = (imm<<16)>>16; // sign-extends 16-bit imm to be a 32-bit value
    int temp = RegFile[rs]+imm32;
    int data = RegFile[rt];
    writeWord(temp,data,false);
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int swl(rt,rs,imm,PC){
    int imm32 = (imm<<16)>>16; // sign-extends 16-bit imm to be a 32-bit value
    int m;
    int temp = ((RegFile[rs] + imm32)<<30)>>30;
    int temp2 = RegFile[rs] + imm32;
    if(temp == 0){
        writeWord(temp2,RegFile[rt],false);
    }else if(temp == 1){
        m = (RegFile[rt]>>4)<<4;
        writeWord(temp2,m,false);
    }else if(temp == 2){
        m = (RegFile[rt]>>8)<<8;
        writeWord(temp,m,false);
    }else{
        m = (RegFile[rt]>>12)<<12;
        writeWord(temp,m,false);
    }
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int swr(rt,rs,imm,PC){
    int imm32 = (imm<<16)>>16; // sign-extends 16-bit imm to be a 32-bit value
    int m;
    int temp = ((RegFile[rs] + imm32)<<30)>>30;
    int temp2 = RegFile[rs] + imm32;
    if(temp == 0){
        m = (RegFile[rt]<<12)>>12;
        writeWord(temp,m,false);
    }else if(temp == 1){
        m = (RegFile[rt]<<8)>>8;
        writeWord(temp,m,false);;
    }else if(temp == 2){
        m = (RegFile[rt]<<4)>>4;
        writeWord(temp2,m,false);
    }else{
        writeWord(temp,RegFile[rt],false);
    }
    return 0;
}
// ------------------------------------------------- //
// ------------------------------------------------- //
int j(PC,instr_index,rs,rt){
    printf("here is j: PC = 0x%x;  instrindex = 0x%x; \n",PC, instr_index);
    PCG=(PC&0xf0000000)|(instr_index<<2);
    return 2;
}

// ------------------------------------------------- //
// ------------------------------------------------- //
int jal(PC,instr_index,rs,rt){
    RegFile[31] = PC + 8;
    PCG=(PC&0xf0000000)|(instr_index<<2);
    
    return 2;
}

// ------------------------------------------------- //
// ------------------------------------------------- //



