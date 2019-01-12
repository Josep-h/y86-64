#include"y86_essence.h"
#include"y86_essence.cpp"
#include"decoder.cpp"
#include <stdlib.h>
#include<mutex>
using namespace std;
string error[]={"AOK","AOK","HLT","ADR","INS"};

//用来控制是否需要后退
bool back=0;

void BACK(){back=1;}//这个函数用来修改back的值用以回退

int main()
{
    L2:
    hitcoutn_all=0;
    misscount_all=0;
    r=0;
    PC=0;
    cout<<"G'Day, nice to meet you!"<<endl;
    cout<<"This is a y86-simulator version 3.0's terminal version produced by Runpeng Xie and Mingzhe Zhu. "<<endl;
    cout<<"Hope you have nice time here. Please input the location of your .yo file, and then we can start!"<<endl;
    freg.predPC=0;ZF=1;SF=0;OF=0;//初始化
    decoder();
    mreg.Cnd=1;
    dreg.icode=wreg.icode=mreg.icode=ereg.icode=1;
    f.icode=d.icode=e.icode=m.icode=0;
    f.stat=dreg.stat=d.stat=e.stat=ereg.stat=m.stat=mreg.stat=wreg.stat=AOK;
    reg[RSP]=0;int flag=0;
    int time_machine;bool flag_time_machine=0;
    while(1)
    {
        if(back)
        {
            if(r==1) 
            {
                cout<<"Sorry. It's dangerous. Please go ahead."<<endl;
                back=0;
            }
            else{
                Path.dataGet();
                back=0;
            }
        }
        else if(flag_time_machine&&time_machine<r)
        {Path.dataGet();continue;}
        //SelectPC
        else if(flag_time_machine&&time_machine==r)
        {
            flag_time_machine=0;
            cout<<"That's end of your little time travel. Let's go on!"<<endl;
            cout<<"----------------------------------------------------------"<<endl;
            cout<<"----------------------------------------------------------"<<endl;
        }
        else{
            GetPC();
            OneCycle();
            r++;}
        cout<<"It's the Cycle "<<r<<":"<<endl;
        cout<<"PC= 0x"<<hex<<PC;
        cout<<"   Stat= "<<error[Stat]<<endl;
        cout<<"SF= "<<SF<<"   ZF= "<<ZF<<"   OF= "<<OF<<endl;
        
        cout<<"D: icode= "<<dec<<dreg.icode<<"  rA= "<<dreg.rA<<"  rB= "<<dreg.rB;
        printf("   valC= 0x%llx  valP== 0x%llx   ",dreg.valC,dreg.valP);
        cout<<"Stat= "<<error[dreg.stat]<<endl<<"E: icode= "<<ereg.icode;
        printf("   valA= 0x%llx   valB= 0x%llx    valC= 0x%llx",ereg.valA,ereg.valB,ereg.valC);
        cout<<"  srcA="<<ereg.srcA<<"  srcB="<<ereg.srcB<<"  Stat="<<error[dreg.stat]<<endl;
        
        cout<<"M: icode= "<<mreg.icode<<"   Cnd="<<mreg.Cnd;
        printf("   valE= 0x%llx   valA= 0x%llx",mreg.valE,mreg.valA);
        cout<<"  dstE="<<mreg.dstE<<"  Stat="<<error[dreg.stat]<<endl;

        cout<<"W: icode= "<<wreg.icode;
        printf("   valE= 0x%llx",wreg.valE);
        cout<<"   dstE="<<wreg.dstE<<"  Stat="<<wreg.stat;
        printf("   ValM=0x%lld\n",wreg.valM);
        cout<<"Cache Visit Status: "<<cache_word_table[WRITE_FLAG]<<endl;
        cout<<"Hit times: "<<hitcount<<"  Miss times: "<<misscount<<endl;
        cout<<"Execute Order(approximately):"<<endl;
        for(int i=1;i<=16;i++)
        {
            cout<<i<<": "<<EXEORDER[i]<<endl;
        }
        cout<<"----------------------------------------------------"<<endl;
        cout<<endl;
        if(Stat)
        {
            //把cache中的值写回
            cache.cache_memory();
            if(flag_time_machine)
            cout<<"Sorry, but it's the end of the time, you can't go any further. See you next time!"<<endl;

            cout<<"----------------------------------------------"<<endl;
            cout<<"====   *******   ===    ===  =====      ======"<<endl;
            cout<<"====   **        ===  =  ==  =====  ===   ===="<<endl;
            cout<<"====   *******   ===  ==  =  =====  ====   ==="<<endl;
            cout<<"====   **        ===  ===    =====  ===   ===="<<endl;
            cout<<"====   *******   ===  ====   =====     ======="<<endl;
            cout<<"----------------------------------------------"<<endl;
            break;
        }
        if(flag) continue;
        char s;
        if(flag_time_machine) continue;
LSTACK:
        cout<<"Input c for next turn, b for last cycle, s for viewing the stack, t to start time machine"<<endl;
        cout<<"And other words for the final anwser. G'day!"<<endl;
        cin>>s;
        if(s=='c') continue;
        else if(s=='b') back=1;
        else if(s=='s') 
        {
            cout<<"Please input a positive decimal x to view the top x nums in the stack..."<<endl;
            int tp;
            cin>>tp;
            bool OVER=0;
            for(int i=0;i!=tp&&OVER==0;i++)//查询栈的情况
            {
                long long sum=0;
                for(int j=7;j!=-1;j--)
                {
                    sum=sum<<8;
                    if(reg[RSP]+j-8*i<0)
                    {cout<<"Sorry, but that's all. There's nothing else in the stack."<<endl;OVER=1;break;}
                    
                    sum+=memory[reg[RSP]+j-8*i];
                }
                cout<<"   0x"<<hex<<reg[RSP]-8*i<<" : "<<sum<<endl;
            }
            cout<<"That's all you want. What's next?"<<endl;
            goto LSTACK;
        }
        else if(s=='t')//启动时光机，到达任何一个循环
        {
            int tp;
            cout<<"Time machine starts! Which cycle do you want to go? Input a positive decimal,please."<<endl;
            cin>>tp;
            if(tp<0)
            {
                cout<<"No, that's a dangerous try, please don't do that."<<endl;
                goto LSTACK;
            }
            flag_time_machine=1;time_machine=tp;
            cout<<"Time machine starts now. "<<endl;
            cout<<"----------------------------------------------------------"<<endl;
            cout<<"----------------------------------------------------------"<<endl;
        }
        else flag=1;
    }
    cout<<"Your simulation stops here with stat of "<<error[Stat]<<endl;    
    cout<<"and you have executed  "<<dec<<r<<"  Cycles.";
    cout<<"Now is the final situation: "<<endl;

    cout<<"Condition code: ZF: "<<ZF<<"  "<<"SF: "<<SF<<"  "<<"OF: "<<OF<<endl;
    cout<<"---------"<<endl;
    cout<<"Cache Visit Data:"<<endl;
    cout<<"Hit times in all: "<<hitcoutn_all<<"   Miss times in all: "<<misscount_all<<endl;
    cout<<"Hit ratio: "<<double(hitcoutn_all)/double(hitcoutn_all+misscount_all)<<endl;
    cout<<"---------"<<endl;
    cout<<"Now you can input a positive decimal to get the num in the memory"<<endl;
    cout<<"Or a decimal x which x>=-16&&x<0 to view a num in register,(e.g. -1 for %rax)"<<endl;
    cout<<"Put in something else to quit"<<endl;
    cout<<"And Please in a decimal number"<<endl;
    
    while(1)
    {    
        int tp;
        cin>>tp;
        if(tp<0&&tp>=-16)//查看寄存器的情况
        {
            cout<<"the register you want is: 0x";
            //printf("%llx",reg[-tp-1]);
            cout<<hex<<reg[-tp-1];
            cout<<endl<<"Go on please."<<endl;
        }
        else if(tp>=0) //查看内存的情况
        {
            long long sum=0;
            for(int i=7;i!=-1;i--)
            {
                sum=sum<<8;
                sum+=memory[tp+i];
            }
            cout<<"So is that what you want? 0x";
            printf("%llx\n",sum);
            cout<<"Go on please."<<endl;
        }
        else if(tp==-200) goto L2;
        else break;
    }
    
    cout<<"Thanks for using, see you next time! Wish you a good day."<<endl;
    return 0;
}