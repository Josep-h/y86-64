#include"y86_essence.h"

bool valid(int i)
{
    return i<16&&i>=0;
}

void F::fetch()
{
    icode=memory[PC]/16;
    ifun=memory[PC]%16;
    int instr_valid=icode<12&&icode>=0;
    if(icode==OP)
    instr_valid=instr_valid&&(ifun<4&&ifun>=0);
    if(icode==JXX)
    instr_valid=instr_valid&&(ifun<7&&ifun>=0);
    if(icode==CM)
    instr_valid=instr_valid&&(ifun<7&&ifun>=0);
    rA=rB=15;//先默认一个不修改的寄存器
    int imem_error=0;long long sum=0;
    switch(icode)
    {
        case OP:
        case PUSH:
        case POP:
        case RR:
            rA=memory[PC+1]/16;
            rB=memory[PC+1]%16;
            valP=PC+2;
            imem_error=!(valid(rA)&&valid(rB));
            break;
        case IR:
        case RM:
        case MR:
            rA=memory[PC+1]/16;
            rB=memory[PC+1]%16;
            for(int i=PC+5;i!=PC+1;i--)
            {
                sum*=16*16;
                sum+=memory[i];
            }
            valC=sum;
            valP=PC+10;
            imem_error=!(valid(rA)&&valid(rB));
            break;
        case JXX:
        case CALL:
            for(int i=PC+8;i!=PC;i--)
            {
                sum*=16*16;
                sum+=memory[i];
            }
            valC=sum;
            valP=PC+9;break;
        case RET:valP=PC+1;break;
        case NOP:valP=PC+1;break;
    }
    if(imem_error) stat=ADR;
    else if(!instr_valid) stat=INS;
    else if(icode==HALT) stat=HLT;
    else stat=AOK;
}