#include"y86_essence.h"
//------------------------------------
        //运行阶段，这里做的一切事情是从现在的流水线寄存器中拿值出来计算
        //但不写入下一个流水线寄存器
        //这一步中所有的操作都是以cons_code为对象完成的
        //cons_code模拟的就是程序运行的中间两形态
//------------------------------------

void run_in_cons(int r)
{
    //fetch
        f.fetch();//取指阶段完成
        predictPC=f.PredictPC();//获得预计的PC值
        
        //deocde
        if(r>0)
        {
            d.rA=dreg.rA;d.rB=dreg.rB;d.icode=dreg.icode;
            d.valP=dreg.valP;
            d.decode();
            if(d.icode==8||d.icode==7)
            d.valA=dreg.valP;
        }
        if(r>1)//execute
        {
            
        }
        if(r>2)//memo
        {

        }
        if(r>3)//write
        {

        }
}