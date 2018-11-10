#include"y86_essence.h"

void cons_code::write(cons_code &code)
{
    switch(icode)
    {
        case 2:
        case 3:
        case 6:
            reg[rB]=valE;break;
        case 8:
        case 9:
        case 10:
            reg[4]=valE;break;
        case 4:
            reg[rA]=valM;break;
        case 11:
            reg[4]=valE;
            reg[rA]=valM;break;
    }
}