#include"y86_essence.h"

void D::decode()
{
    rA=dreg.rA;rB=dreg.rB;icode=dreg.icode;
    valP=dreg.valP;
    switch(icode)
    {
        case 0:
        case 1:break;
        case 2:
            valA=reg[rA];
            srcA=dreg.rA;break;
        case 3:
            dstE=dreg.rB;
            break; 
        case 4:
            valB=reg[rB];
            srcA=dreg.rA;srcB=rB;
            dstE=dreg.rB;
            break;
        case 5:
            valA=reg[rA];srcB=rB;
            valB=reg[rB];
            dstM=rA;
            break;
        case 6:
            valA=reg[rA];
            valB=reg[rB];
            srcA=dreg.rA;srcB=rB;
            dstE=dreg.rB;
            break;
        case 7:
            valA=dreg.valP;break;
        case 8:
            valB=reg[4];valA=dreg.valP;
            srcB=4;
            dstE=4;break;
        case 9:
            valA=reg[4];
            valB=reg[4];
            srcA=4;srcB=4;
            dstE=4;break;
        case 10:
            valA=reg[rA];
            valB=reg[4];
            srcA=dreg.rA;srcB=4;
            dstE=4;break;
        case 11:
            valA=reg[4];
            valB=reg[4];
            srcA=4;srcB=4;
            dstE=4;dstM=rA;
    }
    if(wreg.stat==BUB)
    Stat=0;
    else Stat=wreg.stat;
}