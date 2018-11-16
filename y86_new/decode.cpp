#include"y86_essence.h"

void D::decode()
{
    rA=dreg.rA;rB=dreg.rB;icode=dreg.icode;
    valP=dreg.valP;
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
    if(wreg.stat==BUB)
    Stat=0;
    else Stat=wreg.stat;
}