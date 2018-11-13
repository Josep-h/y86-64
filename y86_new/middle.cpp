#include"y86_essence.h"
//这个函数专门用于处理转发的问题
void middle()
{
    //decode
    if(dreg.icode==8||dreg.icode==7)
    d.valA=dreg.valP;
    else if(d.srcA==e.dstE)
    d.valA=e.valE;
    else if(d.srcA==mreg.dstM)
    d.valA=m.valM;
    else if(d.srcA==mreg.dstE)
    d.valA=mreg.valE;
    else if(d.srcA==wreg.dstM)
    d.valA=wreg.valM;
    else if(d.srcA==wreg.dstE)
    d.valA=wreg.valE;
}