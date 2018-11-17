extern const int AOK,HLT,ADR,INS; 
extern const int HALT,NOP,RR,IR,RM,MR,OP,JXX,CM,CALL,RET,PUSH,POP;
extern const int RSP,NONE;
extern int ZF,SF,OF,set_cc;//条件寄存器
extern long long reg[16];//寄存器
extern int PC,Stat;
//PC相当于这个数组的下标
extern int memory[200000];