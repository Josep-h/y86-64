#include"y86_essence.h"

void SelectPC()
{
    //jxx
    if(mreg.icode==JXX&&!mreg.Cnd)
        PC=mreg.valA;
    //ret
    else if(wreg.icode==RET)
        PC=wreg.valM;

    else PC=freg.predPC;
}

void F::f_pred()
{
    if(icode==CALL||icode==JXX)
        f.predPC=f.valC;
    else f.predPC=f.valP;
}