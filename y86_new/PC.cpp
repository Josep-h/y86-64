#include"y86_essence.h"

void SelectPC()
{
    //jxx
    if(mreg.icode==7&&!mreg.Cnd)
        f_pc=mreg.valA;
    //ret
    else if(wreg.icode==9)
        f_pc=wreg.valM;
    else f_pc=F_predPC;

    PC=f_pc;
}

void cons_code::f_pred()
{
    if(icode==8||icode==7)//jxx||call
        f_predPC=f.valC;
    else f_predPC=f.valP;
}