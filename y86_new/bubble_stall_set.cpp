#include"y86_essence.h"

void bubble_stall_set()
{
    clearF();//状态清零
    if(dreg.icode==9||ereg.icode==9||m.icode==9)
        {
            F_stall=1;
            dreg.bubble=1;
        }
    //加载使用/冒险
    else if((ereg.icode==5||ereg.icode==11)&&(ereg.dstM==d.srcA||ereg.dstM==d.srcB))
        {
            F_stall=1;
            dreg.stall=1;
            ereg.bubble=1;
        }
    else if(ereg.icode==7&&!e.Cnd)
        {
            dreg.bubble=1;
            ereg.bubble=1;
        }
    else if(m.stat)///待填充
        {

        }
}