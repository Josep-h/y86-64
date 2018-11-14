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
        f.f_pred();//获得预计的PC值并存入f_predPC
        
        //deocde
        if(r>0)
        {
            d.rA=dreg.rA;d.rB=dreg.rB;d.icode=dreg.icode;
            d.valP=dreg.valP;
            d.decode();
            if(d.icode==8||d.icode==7)
            d.valA=dreg.valP;
            
            //确定srcA
            if(d.icode==2||d.icode==4||d.icode==6||d.icode==10)
            d.srcA=dreg.rA;
            else if(d.icode==11||d.icode==9)
            d.srcA=4;//rsp
            else d.srcA=15;//没有寄存器
        }
        if(r>1)//execute
        {
            e.icode=ereg.icode;e.valC=ereg.valC;e.valA=ereg.valA;
            e.valB=ereg.valB;e.dstE=ereg.dstE;
            e.execute();
        }
        if(r>2)//memo
        {
            m.icode=mreg.icode;m.valE=mreg.valE;m.valA=mreg.valA;
            m.memo();
        }
        if(r>3)//write
        {
            w.icode=wreg.icode;w.valE=wreg.valE;w.valE=wreg.valE;
            w.write();
        }
}