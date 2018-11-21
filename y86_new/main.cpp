#include"y86_essence.h"
#include"decode.cpp"
#include"execute.cpp"
#include"fetch.cpp"
#include"memory.cpp"
#include"PC.cpp"
#include"decoder.cpp"
#include"run_in_reg.cpp"
#include"bubble_stall_set.cpp"
using namespace std;
void run_in_cons()
{
        //fetch
        f.fetch();//取指阶段完成
        f.f_pred();//获得预计的PC值并存入f_predPC
        d.decode();    
        e.execute();
        m.memo();
}

void forward()
{
    //decode
    //valA
    if(dreg.icode==CALL||dreg.icode==JXX) d.valA=dreg.valP;
    else if(d.srcA==e.dstE) d.valA=e.valE;
    else if(d.srcA==mreg.dstM) d.valA=m.valM;
    else if(d.srcA==mreg.dstE) d.valA=mreg.valE;
    else if(d.srcA==wreg.dstM) d.valA=wreg.valM;
    else if(d.srcA==wreg.dstE) d.valA=wreg.valE;

    //valB
    if(d.srcB==e.dstE) d.valB=e.valE;
    else if(d.srcB==mreg.dstM) d.valB=m.valM;
    else if(d.srcB==mreg.dstE) d.valB=mreg.valE;
    else if(d.srcB==wreg.dstM) d.valB=wreg.valM;
    else if(d.srcB==wreg.dstE) d.valB=wreg.valE;
}

int main()
{
    PC=0;
    freg.predPC=0;ZF=1;SF=0;OF=0;//初始化
    decoder();mreg.Cnd=1;
    dreg.icode=wreg.icode=mreg.icode=ereg.icode=1;
    f.icode=d.icode=e.icode=m.icode=0;
    f.stat=dreg.stat=d.stat=e.stat=ereg.stat=m.stat=mreg.stat=wreg.stat=AOK;
    reg[RSP]=0;int r=0;
    while(1)
    {
        //SelectPC
        SelectPC();
        run_in_cons();
        forward();
        bubble_stall_set();
        run_in_reg();
        r++;
        if(Stat)
        break;
    }
    cout<<"Your simulation stops here with stat of "<<Stat<<endl;
    cout<<"Thanks for using, see you next time! Wish you a good day."<<endl;
    cout<<"and you have executed "<<r<<" cycles.";
    for(;;);
    return 0;
}