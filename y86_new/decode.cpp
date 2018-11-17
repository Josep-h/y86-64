#include"y86_essence.h"

void D::decode()
{
    dstM=15;dstE=15;//默认一个不修改的寄存器
    rA=dreg.rA;rB=dreg.rB;
    valP=dreg.valP;valC=dreg.valC;
    icode=dreg.icode;ifun=dreg.ifun;stat=dreg.stat;
    switch(icode)
    {
        case RR:
            valA=reg[rA];
            srcA=dreg.rA;break;
        case IR:
            dstE=dreg.rB;break; 
        case RM:
            valB=reg[rB];
            srcA=dreg.rA;srcB=rB;
            dstE=dreg.rB;break;
        case MR:
            valA=reg[rA];srcB=rB;
            valB=reg[rB];
            dstM=rA;break;
        case OP:
            valA=reg[rA];
            valB=reg[rB];
            srcA=dreg.rA;srcB=rB;
            dstE=dreg.rB;break;
        case JXX:
            valA=dreg.valP;break;
        case CALL:
            valB=reg[4];valA=dreg.valP;
            srcB=4;
            dstE=4;break;
        case RET:
            valA=reg[4];
            valB=reg[4];
            srcA=4;srcB=4;
            dstE=4;break;
        case PUSH:
            valA=reg[rA];
            valB=reg[4];
            srcA=dreg.rA;srcB=4;
            dstE=4;break;
        case POP:
            valA=reg[4];
            valB=reg[4];
            srcA=4;srcB=4;
            dstE=4;dstM=rA;
    }
    if(wreg.stat==1)
    Stat=0;
    else Stat=wreg.stat;
}