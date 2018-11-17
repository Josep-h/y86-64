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
            if(e.Cnd) dstE=rB;
            srcA=rA;
            dstE=rB;break;
        case IR:
            dstE=rB;break; 
        case RM:
            valB=reg[rB];
            srcA=rA;srcB=rB;
            break;
        case MR:
            srcB=rB;
            dstM=rA;
            valB=reg[rB];
            break;
        case OP:
            valA=reg[rA];
            valB=reg[rB];
            srcA=rA;srcB=rB;
            dstE=rB;break;
        case JXX:
            valA=dreg.valP;break;
        case CALL:
            valB=reg[4];valA=dreg.valP;
            srcB=RSP;
            dstE=RSP;break;
        case RET:
            valA=reg[4];
            valB=reg[4];
            srcA=RSP;srcB=RSP;
            dstE=RSP;break;
        case PUSH:
            valA=reg[rA];
            valB=reg[RSP];
            srcA=rA;srcB=RSP;
            dstE=RSP;break;
        case POP:
            valA=reg[RSP];
            valB=reg[RSP];
            srcA=RSP;srcB=RSP;
            dstE=RSP;dstM=rA;
    }
    if(wreg.stat==1)
    Stat=0;
    else Stat=wreg.stat;
}