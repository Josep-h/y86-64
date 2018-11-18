#include"y86_essence.h"

void D::decode()
{
    dstM=NONE;dstE=NONE;srcA=16;srcB=16;//默认一个不修改的寄存器
    rA=dreg.rA;rB=dreg.rB;
    valP=dreg.valP;valC=dreg.valC;
    valA=valB=0;
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
            break;
        case CALL:
            valB=reg[RSP];
            srcB=RSP;
            dstE=RSP;break;
        case RET:
            valA=reg[RSP];
            valB=reg[RSP];
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
}