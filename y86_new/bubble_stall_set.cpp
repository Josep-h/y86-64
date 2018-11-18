#include"y86_essence.h"

void clearF()
{
    freg.stall=0;
    dreg.bubble=dreg.stall=0;
    mreg.bubble=mreg.stall=0;
    ereg.bubble=ereg.stall=0;
    wreg.bubble=wreg.stall=0;
}

void bubble_stall_set()
{
    clearF();
    freg.stall=(ereg.icode==MR||ereg.icode==POP)&&\
    (ereg.dstM==d.srcA||ereg.dstM==d.srcB)||\
    (dreg.icode==RET||ereg.icode==RET||mreg.icode==RET);

    dreg.stall=(ereg.icode==MR||ereg.icode==POP)&&(ereg.dstM==d.srcA||ereg.dstM==d.srcB);

    dreg.bubble=((ereg.icode==JXX)&&!e.Cnd)||\
    !((ereg.icode==MR||ereg.icode==POP)&&(ereg.dstM==d.srcA||ereg.dstM==d.srcB))&&\
    (dreg.icode==RET||ereg.icode==RET||mreg.icode==RET);

    ereg.bubble=(ereg.icode==JXX&&!e.Cnd)||\
    (ereg.icode==MR||ereg.icode==POP)&&\
    (ereg.dstM==d.srcA||ereg.dstM==d.srcB);

    set_cc=(ereg.icode==OP)&&(m.stat==AOK)&&(wreg.stat==AOK);

    mreg.bubble=m.stat!=AOK||wreg.stat!=AOK;

    wreg.stall=wreg.stat!=AOK;  
}