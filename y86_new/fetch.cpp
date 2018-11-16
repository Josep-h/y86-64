#include"y86_essence.h"

void F::fetch()
{
    icode=memory[PC]/16;
    ifun=memory[PC]%16;
    if(icode<16&&icode>=0)
    {instr_valid=1;}
    else{instr_valid=0;
    switch(icode)
    {
        case 2:
        case 6:
        case 10:
        case 11:
            rA=memory[PC+1]/16;
            rB=memory[PC+1]%16;
            valP=PC+2;break;
        case 3:
        case 4:
        case 5:
            rA=memory[PC+1]/16;
            rB=memory[PC+1]%16;
            long long sum=0;
            for(int i=PC+9;i!=PC+1;i--)
            {
                sum*=16*16;
                sum+=memory[i];
            }
            valC=sum;
            valP=PC+10;break;
        case 7:
        case 8:
            long long sum=0;
            for(int i=PC+8;i!=PC;i--)
            {
                sum*=16*16;
                sum+=memory[i];
            }
            valC=sum;
            valP=PC+9;break;
        case 0:
        case 1:
        case 9:break;
    }
    }
    if(imem_error) stat=ADR;
    else if(!instr_valid) stat=INS;
    else if(icode==HALT) stat=HLT;
    else stat=0;
}