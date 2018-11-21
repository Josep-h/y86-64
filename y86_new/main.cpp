#include"y86_essence.h"
#include"decode.cpp"
#include"execute.cpp"
#include"fetch.cpp"
#include"memory.cpp"
#include"PC.cpp"
#include"decoder.cpp"
#include"run_in_reg.cpp"
#include"bubble_stall_set.cpp"
# include <stdlib.h>
using namespace std;
string error[]={"AOK","AOK","HLT","ADR","INS"};

string insixteen(long long num)
{
    char s[25];
    itoa(num,s,16);
    return s;
}

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
    cout<<"G'Day, nice to meet you! This is a y86-simulator version 1.0's terminal version produced by Runpeng Xie and Mingzhe Zhu. "<<endl;
    cout<<"Hope you have nice time here. Please input the location of your .yo file, and then we can start!"<<endl;
    freg.predPC=0;ZF=1;SF=0;OF=0;//初始化
    decoder();mreg.Cnd=1;
    dreg.icode=wreg.icode=mreg.icode=ereg.icode=1;
    f.icode=d.icode=e.icode=m.icode=0;
    f.stat=dreg.stat=d.stat=e.stat=ereg.stat=m.stat=mreg.stat=wreg.stat=AOK;
    reg[RSP]=0;int r=0;int flag=0;
    while(1)
    {
        //SelectPC
        SelectPC();
        run_in_cons();
        forward();
        bubble_stall_set();
        run_in_reg();
        r++;
        cout<<"It's the Cycle "<<r<<":"<<endl;
        cout<<"PC: 0x"<<insixteen(PC)<<"  Stat: "<<error[Stat]<<endl;
        cout<<"Condition code: ZF: "<<ZF<<"  "<<"SF: "<<SF<<"  "<<"OF: "<<OF<<endl;
        cout<<"D: icode= "<<dreg.icode<<"   rA="<<dreg.rA<<"  rB="<<dreg.rB<<"  valC=0x"<<insixteen(dreg.valC)<<"  valP=0x"<<insixteen(dreg.valP)<<"  Stat="<<error[dreg.stat]<<endl;
        cout<<"E: icode= "<<ereg.icode<<"   valC=0x"<<insixteen(ereg.valA)<<"  valB=0x"<<insixteen(ereg.valB)<<"  srcA="<<ereg.srcA<<"  srcB="<<ereg.srcB<<"  Stat="<<error[dreg.stat]<<endl;
        cout<<"M: icode= "<<mreg.icode<<"   Cnd="<<mreg.Cnd<<"  valE=0x"<<insixteen(mreg.valE)<<"  valA=0x"<<insixteen(mreg.valA)<<"  dstE="<<mreg.dstE<<"  Stat="<<error[dreg.stat]<<endl;
        cout<<"W: icode= "<<wreg.icode<<"   valE=0x"<<insixteen(wreg.valE)<<"  dstE="<<wreg.dstE<<"  Stat="<<dreg.stat<<endl;
        cout<<"----------------------------------------------------"<<endl;
        cout<<endl;
        if(Stat)
        break;
        if(flag) continue;
        char s;
        cout<<"Input c for next turn, and ohther words for the final anwser."<<endl;
        cin>>s;
        if(s=='c') continue;
        else flag=1;
        
    }
    cout<<"----------------------------------------------------"<<endl;
    cout<<"Your simulation stops here with stat of "<<error[Stat]<<endl;    
    cout<<"and you have executed  "<<r<<"  Cycles.";
    cout<<"Now is the final situation: "<<endl;

    cout<<"Condition code: ZF: "<<ZF<<"  "<<"SF: "<<SF<<"  "<<"OF: "<<OF<<endl;

    cout<<"Now you can input a positive decimal to get the num in the memory"<<endl;
    cout<<"Or a decimal x which x>=-16&&x<0 to view a num in register,(e.g. -1 for /% rax)"<<endl;
    cout<<"Put in something else to quit"<<endl;
    cout<<"And Please in a decimal number"<<endl;
    
    while(1)
    {    
        int tp;
        cin>>tp;
        if(tp<0&&tp>=-16)
        {
            cout<<"the register you want is: 0x"<<insixteen(reg[-tp-1])<<endl<<"Go on please."<<endl;
        }
        else if(tp>=0) 
        {
            long long sum=0;
            for(int i=7;i!=-1;i--)
            {
                sum=sum<<8;
                sum+=memory[tp+i];
            }
            cout<<"So is that what you want? 0x"<<insixteen(sum)<<endl<<"Go on please."<<endl;
        }
        else break;
    }

    cout<<"Thanks for using, see you next time! Wish you a good day."<<endl;
    return 0;
}