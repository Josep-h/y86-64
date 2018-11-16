#include"y86_essence.h"

void Wreg::write()
{
    icode=wreg.icode;valE=wreg.valE;valM=wreg.valM;
    switch(icode)
    {
        case 2:
        case 3:
        case 6:
            reg[wreg.dstE]=valE;break;
        case 8:
        case 9:
        case 10:
            reg[4]=valE;break;
        case 4:
            reg[wreg.dstM]=valM;break;
        case 11:
            reg[4]=valE;
            reg[wreg.dstM]=valM;break;
    }
}