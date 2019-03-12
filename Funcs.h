//
//  Funcs.h
//  
//
//  Created by Max Weissman on 4/6/18.
//

#ifndef Funcs_h
#define Funcs_h

int add(int rd,int rs,int rt, int sa,int PC);      // index 0 (32)
int addu(int rd,int rs,int rt, int sa,int PC);
int sub(int rd,int rs,int rt, int sa,int PC);
int subu(int rd,int rs,int rt, int sa,int PC);     // index 3 (35)
int divv(int rd,int rs,int rt,int sa,int PC);
int divu(int rd,int rs,int rt, int sa,int PC);
int mult(int rd,int rs,int rt, int sa,int PC);
int multu(int rd,int rs,int rt, int sa,int PC);    // index 7 (25)
int mfhi(int rd,int rs,int rt, int sa,int PC);
int mflo(int rd,int rs,int rt, int sa,int PC);
int mthi(int rd,int rs,int rt, int sa,int PC);
int mtlo(int rd,int rs,int rt, int sa,int PC);
int and(int rd,int rs,int rt, int sa,int PC);
int xor(int rd,int rs,int rt, int sa,int PC);      // index 13 (38)
int nor(int rd,int rs,int rt, int sa,int PC);
int or(int rd,int rs,int rt, int sa,int PC);
int sll(int rd,int rs,int rt, int sa,int PC);
int sllv(int rd,int rs,int rt, int sa,int PC);      // index 17 (4)
int slt(int rd,int rs,int rt, int sa,int PC);
int sltu(int rd,int rs,int rt, int sa,int PC);
int sra(int rd,int rs,int rt, int sa,int PC);
int srav(int rd,int rs,int rt, int sa,int PC);
int srl(int rd,int rs,int rt, int sa,int PC);
int srlv(int rd,int rs,int rt, int sa,int PC);     // index 23 (6)
int jalr(int rd,int rs,int rt, int sa,int PC);
int jr(int rd,int rs,int rt, int sa,int PC);       // index 25 (8)


// Immediate Functions
int addi(int rt, int rs, int imm,int PC);  // index 0 (8)
int addiu(int rt, int rs, int imm,int PC);
int andi(int rt, int rs, int imm,int PC);
int xori(int rt, int rs, int imm,int PC);  // index 3 (14)
int ori(int rt, int rs, int imm,int PC);
int slti(int rt, int rs, int imm,int PC);
int sltiu(int rt, int rs, int imm,int PC);  // index 6 (11)
int beq(int rt,int rs,int imm,int PC);
int beql(int rt,int rs,int imm,int PC);     // index 8 (20)
int bgez(int rt,int rs,int imm,int PC);
int bgezal(int rt,int rs,int imm,int PC);
int bgtz(int rt,int rs,int imm,int PC);
int blez(int rt,int rs,int imm,int PC);
int blezl(int rt,int rs,int imm,int PC);    // index 11 (22)
int bltz(int rt,int rs,int imm,int PC);
int bltzal(int rt,int rs,int imm,int PC);
int bne(int rt,int rs,int imm,int PC);
int bnel(int rt,int rs,int imm,int PC);     // index 13 (21)
int lb(int rt,int rs,int imm,int PC);
int lbu(int rt,int rs,int imm,int PC);
int lh(int rt,int rs,int imm,int PC);      // index 16 (33)
int lhu(int rt,int rs,int imm,int PC);
int lui(int rt,int rs,int imm,int PC);
int lw(int rt,int rs,int imm,int PC);      // index 19 (35)
int lwl(int rt,int rs,int imm,int PC);
int lwr(int rt,int rs,int imm,int PC);
int sb(int rt,int rs,int imm,int PC);      // index 22 (40)
int sh(int rt,int rs,int imm,int PC);
int sw(int rt,int rs,int imm,int PC);      // index 24 (43)
int swl(int rt,int rs,int imm,int PC);
int swr(int rt,int rs,int imm,int PC);     // index 26 (46)

int j(int PC,int instr_index,int rs, int rt);
int jal(int PC,int instr_index,int rs, int rt);


#endif /* Funcs_h */
