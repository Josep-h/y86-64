#include"y86_essence.h"

bool valid(int i)
{
    return i<16&&i>=0;
}

void F::fetch()
{
    icode=memory[PC]/16;
    ifun=memory[PC]%16;
    instr_valid=icode<13&&icode>=0&&ifun<4&&ifun>=0;
    int imem_error;
    switch(icode)
    {
        case OP:
        case PUSH:
        case POP:
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
            long long sum=0;
            for(int i=PC+9;i!=PC+1;i--)
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
            long long sum=0;
            for(int i=PC+8;i!=PC;i--)
            {
                sum*=16*16;
                sum+=memory[i];
            }
            valC=sum;
            valP=PC+9;break;
        case RET:break;
    }
    if(imem_error) stat=ADR;
    else if(!instr_valid) stat=INS;
    else if(icode==HALT) stat=HLT;
    else stat=AOK;
}