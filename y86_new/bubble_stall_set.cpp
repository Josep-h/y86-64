#include"y86_essence.h"

void bubble_stall_set()
{
    clearF();//状态清零,在头文件中
    F_stall=((ereg.icode==3||ereg.icode==11)&&(ereg.dstM==d.srcA||ereg.dstM==d.srcB)||\
    (dreg.icode==9||ereg.icode==9||mreg.icode==9));

    dreg.stall=((ereg.icode==5||ereg.icode==11)&&(ereg.dstM==d.srcA||ereg.dstM==d.srcB));

    dreg.bubble=((ereg.icode==7&&!e.Cnd)||\
    !((ereg.icode==5||ereg.icode==11)&&(ereg.dstM==d.srcA||ereg.dstM==d.srcB))&&\
    (dreg.icode==9||ereg.icode==9||mreg.icode==9));

    ereg.bubble=(ereg.icode==7&&!e.valC)||\
    (ereg.icode==5||ereg.icode==11)&&\
    (ereg.dstM==d.srcA||ereg.dstM==d.srcB);

    set_cc=(ereg.icode==11)&&\
    !(m.stat==SADR||m.stat==SINS||m.stat==SHLT)&&\
    !(wreg.stat==SADR||wreg.stat==SINS||wreg.stat==SHLT);

    mreg.bubble=(m.stat==SADR||m.stat==SINS||m.stat==SHLT)||\
    (wreg.stat==SADR||wreg.stat==SINS||wreg.stat==SHLT);

    wreg.stall=(wreg.stat==SADR||wreg.stat==SINS||wreg.stat==SHLT);
}