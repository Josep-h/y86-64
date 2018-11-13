#include"y86_essence.h"

void cons_code::PCchange()
{
    switch(icode)
    {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 10:
        case 11:
            PC=valP;break;
        case 7:
            PC=Cnd?valC:valP;break;
        case 8:
            PC=valC;break;
        case 9:
            PC=valM;break;
    }
}

int cons_code::PredictPC()
{
    if(icode==8)
        return valC;
    else return valP;
}