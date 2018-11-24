#include"y86_essence.h"
#include<stack>
#include<vector>
class Data
{
//这个数据结构接受之前所有的计算结果，并且把之前的结果压缩在栈里面
//通过弹栈来恢复上一个数据，当然这样会消耗巨大的空间，当循环次数太长的时候
    public:
    stack<int> hisR;
    stack<int> hisPC;
    stack<int> hisStat;
    //Condition code
    stack<bool> hisZF,hisSF,hisOF,hisset_cc;
    //memory
    stack<pair<int,long long> > hisMemory;
    stack<vector<long long> > hisReg;
    //register
    stack<long long> F_predPC;
    stack<bool> F_stall,D_bubble,D_stall,E_bubble,E_stall,M_bubble,M_stall,W_bubble,W_stall;
    stack<int> W_stat,D_stat,E_stat,M_stat;
    stack<int> W_icode,D_icode,E_icode,M_icode;
    stack<int> M_Cnd;
    stack<int> D_rA,D_rB;
    stack<int> E_ifun,D_ifun;
    stack<int> W_dstE,W_dstM,M_dstE,M_dstM,E_dstE,E_dstM,E_srcA,E_srcB;
    stack<long long> W_valE,W_valM,M_valE,M_valA,E_valA,E_valB,E_valC,D_valC,D_valP;

    void dataGet();
    void dataStore();
    void memoryDataStore();
};

void Data::dataGet()
{
    r=hisR.top();hisR.pop();
    PC=hisPC.top();hisPC.pop();
    Stat=hisStat.top();hisStat.pop();
    ZF=hisZF.top();hisZF.pop();
    OF=hisSF.top();hisSF.pop();
    OF=hisOF.top();hisOF.pop();
    freg.predPC=F_predPC.top();F_predPC.pop();
    //reg
    for(int i=0;i!=16;i++)
    reg[i]=hisReg.top()[i];
    hisReg.pop();
    //stall&bubble;
    freg.stall=F_stall.top();F_stall.pop();
    dreg.stall=D_stall.top();D_stall.pop();
    ereg.stall=E_stall.top();E_stall.pop();
    mreg.stall=M_stall.top();M_stall.pop();
    wreg.stall=W_stall.top();W_stall.pop();
    dreg.bubble=D_bubble.top();D_bubble.pop();
    ereg.bubble=E_bubble.top();E_bubble.pop();
    mreg.bubble=M_bubble.top();M_bubble.pop();
    wreg.bubble=W_bubble.top();W_bubble.pop();
    
    //stat
    dreg.stat=D_stat.top();D_stat.pop();
    ereg.stat=E_stat.top();E_stat.pop();
    mreg.stat=M_stat.top();M_stat.pop();
    wreg.stat=W_stat.top();W_stat.pop();

    //icode&ifun
    dreg.icode=D_icode.top();D_icode.pop();
    ereg.icode=E_icode.top();E_icode.pop();
    mreg.icode=M_icode.top();M_icode.pop();
    wreg.icode=W_icode.top();W_icode.pop();
    dreg.ifun=D_ifun.top();D_ifun.pop();
    ereg.ifun=E_ifun.top();E_ifun.pop();
    
    //cnd
    mreg.Cnd=M_Cnd.top();M_Cnd.pop();

    //dst&src
    wreg.dstE=W_dstE.top();W_dstE.pop();
    wreg.dstM=W_dstM.top();W_dstM.pop();
    mreg.dstE=M_dstE.top();M_dstE.pop();
    mreg.dstM=M_dstM.top();M_dstM.pop();
    ereg.dstE=E_dstE.top();E_dstE.pop();
    ereg.dstM=E_dstM.top();E_dstM.pop();
    ereg.srcA=E_srcA.top();E_srcA.pop();
    ereg.srcB=E_srcB.top();E_srcB.pop();
    
    //val
    E_valA,E_valB,E_valC,D_valC,D_valP;
    wreg.valE=W_valE.top();W_valE.pop();
    wreg.valM=W_valM.top();W_valM.pop();
    mreg.valE=M_valE.top();M_valE.pop();
    mreg.valA=M_valA.top();M_valA.pop();
    ereg.valA=E_valA.top();E_valA.pop();
    ereg.valB=E_valB.top();E_valB.pop();
    ereg.valC=E_valC.top();E_valC.pop();
    dreg.rA=D_rA.top();D_rA.pop();
    dreg.rB=D_rB.top();D_rB.pop();
    dreg.valP=D_valP.top();D_valP.pop();
    dreg.valC=D_valC.top();D_valC.pop();
    //memory
    if(hisMemory.top().first!=-1)
    {
        int location=hisMemory.top().first;
        long long val=hisMemory.top().second;
        for(int i=0;i!=8;i++)
        {
            memory[location+i]=val&255;
            val=val>>8;
        }
    }
    hisMemory.pop();
}

void Data::dataStore()
{
    hisPC.push(PC);hisStat.push(Stat);
    hisZF.push(ZF);hisSF.push(SF);hisOF.push(OF);
    hisset_cc.push(set_cc);
    hisR.push(r);
    
    vector<long long> Reg;
    for(int i=0;i!=16;i++)
    Reg.push_back(reg[i]);
    hisReg.push(Reg);
    
    F_predPC.push(freg.predPC);
    F_stall.push(freg.stall);
    D_bubble.push(dreg.bubble);
    D_stall.push(dreg.stall);
    E_bubble.push(ereg.bubble);
    E_stall.push(ereg.stall);
    M_bubble.push(mreg.bubble);
    M_stall.push(mreg.stall);
    W_bubble.push(wreg.bubble);
    W_stall.push(wreg.stall);

    W_stat.push(wreg.stat);
    D_stat.push(dreg.stat);
    E_stat.push(ereg.stat);
    M_stat.push(mreg.stat);

    W_icode.push(wreg.icode);
    D_icode.push(dreg.icode);
    E_icode.push(ereg.icode);
    M_icode.push(mreg.icode);

    M_Cnd.push(mreg.Cnd);

    W_dstE.push(wreg.dstE);
    W_dstM.push(wreg.dstM);
    M_dstE.push(mreg.dstE);
    M_dstM.push(mreg.dstM);
    E_dstE.push(ereg.dstE);
    E_dstM.push(ereg.dstM);
    E_srcA.push(ereg.srcA);
    E_srcB.push(ereg.srcB);

    W_valE.push(wreg.valE);
    W_valM.push(wreg.valM);
    M_valE.push(mreg.valE);
    M_valA.push(mreg.valA);
    E_valA.push(ereg.valA);
    E_valB.push(ereg.valB);
    E_valC.push(ereg.valC);
    D_valC.push(dreg.valC);
    D_valP.push(dreg.valP);

    E_ifun.push(ereg.ifun);
    D_ifun.push(dreg.ifun);
    D_rA.push(dreg.rA);
    D_rB.push(dreg.rB);
}

//如果没有写入任何数据则标记一个-1
    //反之，记录他的地址与更改的值
    //注意，这个标记是在什么也没有做得时候完成的
void Data::memoryDataStore()
{
    if((mreg.icode==PUSH||mreg.icode==CALL||mreg.icode==RM)&&mreg.valE>=0)
    {
        long long sum=0;
        for(int i=7;i!=-1;i--)
        {
            sum=sum<<8;
            sum+=memory[mreg.valE+i];
        }
        hisMemory.push(pair<int,long long>(mreg.valE,sum));
    }
    else hisMemory.push(pair<int,long>(-1,0));
}