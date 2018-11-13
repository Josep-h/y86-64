#include"y86_essence.h"

void SelectPC()
{
    //jxx
    if(mreg.icode==7&&!mreg.Cnd)
        PC=mreg.valA;
    //ret
    else if(wreg.icode==9)
        PC=wreg.valM;
    else PC=predictPC;
}

int cons_code::PredictPC()
{
    if(icode==8||icode==7)//jxx||call
        return valC;
    else return valP;
}