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
    freg.stall=((ereg.icode==3||ereg.icode==11)&&(ereg.dstM==d.srcA||ereg.dstM==d.srcB)||\
    (dreg.icode==9||ereg.icode==9||mreg.icode==9));

    dreg.stall=((ereg.icode==5||ereg.icode==11)&&(ereg.dstM==d.srcA||ereg.dstM==d.srcB));

    dreg.bubble=((ereg.icode==7&&!e.Cnd)||\
    !((ereg.icode==5||ereg.icode==11)&&(ereg.dstM==d.srcA||ereg.dstM==d.srcB))&&\
    (dreg.icode==9||ereg.icode==9||mreg.icode==9));

    ereg.bubble=(ereg.icode==7&&!e.valC)||\
    (ereg.icode==5||ereg.icode==11)&&\
    (ereg.dstM==d.srcA||ereg.dstM==d.srcB);

    set_cc=(ereg.icode==11)&&\
    !(m.stat==ADR||m.stat==INS||m.stat==HLT)&&\
    !(wreg.stat==ADR||wreg.stat==INS||wreg.stat==HLT);

    mreg.bubble=(m.stat==ADR||m.stat==INS||m.stat==HLT)||\
    (wreg.stat==ADR||wreg.stat==INS||wreg.stat==HLT);

    wreg.stall=(wreg.stat==ADR||wreg.stat==INS||wreg.stat==HLT);
}