const int AOK=1,HLT=2,ADR=3,INS=4; 
const int HALT=0,NOP=1,RR=2,IR=3,RM=4,MR=5,OP=6,JXX=7,CM=2,CALL=8,RET=9,PUSH=10,POP=11;
const int RSP=4,NONE=15;
int ZF,SF,OF,set_cc;//条件寄存器
long long reg[16];//寄存器

int PC,Stat;
//PC相当于这个数组的下标

int memory[200000];