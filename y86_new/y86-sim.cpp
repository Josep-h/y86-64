#include<string>
#include<stack>
#include<vector>
#include<iostream>
#include<fstream>
#include <stdlib.h>
using namespace std;
//常量
const int AOK=1,HLT=2,ADR=3,INS=4; 
const int HALT=0,NOP=1,RR=2,IR=3,RM=4,MR=5,OP=6,JXX=7,CM=2,CALL=8,RET=9,PUSH=10,POP=11;
const int RSP=4,NONE=15;
int ZF,SF,OF,set_cc;//条件寄存器
long long reg[16];//寄存器
int r=0;
int PC=0,Stat;
//PC相当于这个数组的下标
int memory[1024];//地址位需要10位完全确定一个位置
//里面的数字都以8个字节为一个单位存储。刚好和命令的最长长度相同。并且保留补码的形式
//以下是五个状态值
class F{
    public:
    int predPC,icode,stat,ifun;
    int rA,rB;
    long long valP,valC;
    void fetch();
    void f_pred();
}f;
class D{
    public:
    int icode,stat,ifun;
    long long valP,valA,valB,valC;
    int dstM,dstE,srcA,srcB,rA,rB;
    void decode();
}d;
class E{
    public:
    int icode,ifun,stat;
    bool Cnd;
    long long valC,valA,valB,valE;
    int dstE,dstM;
    void execute();
}e;
class M{
    public:
    int icode,stat;
    long long valA,valE,valM;
    int dstE,dstM;
    void memo();
}m;

//一下五个是流水线寄存器
class Freg{
    public:
    long long predPC;
    bool stall;
}freg;
class Wreg{
    public:
    int stat,icode,dstE,dstM;
    long long valE,valM;
    bool stall,bubble;
}wreg;
class Mreg{
    public:
    int stat,icode,dstE,dstM;
    long long valE,valA;
    int  Cnd;
    bool stall,bubble;
}mreg;
class Ereg{
    public:
    int stat,icode,ifun;
    int dstE,dstM,srcA,srcB;
    long long valB,valA,valC;
    bool stall,bubble;
}ereg;
class Dreg{
    public:
    int stat,icode,ifun,rA,rB;
    long long valC,valP;
    bool bubble,stall;
}dreg;

class Set{//为了方便
//这里的block里面存储的值就直接用long long型代替了
    public:
    bool valid;bool dirty;
    int tag;
    int blocks[8];//1个byte，一个set可以存1个64位数
    Set(){dirty=0;valid=0;}
    void miss(int set_num,int tag_new);
};

class Cache{
    public:
    Set sets[8];//cache里一个有8个set，每一个set一line，每一line存16个byte
    int visit(long long add);//地址是一个整型值，标注的是一个64位数的开头，返回一个byte
    void write(long long add,int val);//val是一个byte
    void cache_memory();
}cache;

void Set::miss(int set_index,int tag_new)//访问内存并从内存中取出值放入相应的set里面
{
    if(!valid) valid=1;
    long long add=0;
    add+=tag;
    add*=8;
    add+=set_index;
    add*=8;
    //先确定set是否被修改过
    //如果是，需要先写入内存然后再写入set
    if(dirty)
    {
        for(int i=0;i!=8;i++)
        memory[add+i]=blocks[i];
    }
    add=0;
    add+=tag_new;add*=8;add+=set_index;add*=8;//计算新的地址值
    for(int i=0;i!=8;i++)
    {
        blocks[i]=memory[add+i];
    }//拿回值
    tag=tag_new;//更新tag值
    dirty=0;
}

int Cache::visit(long long add)
{
    int offset;
    int set_index;
    int tag;
    long long addtp=add;
    offset=addtp%8;//一个set里面有8个byte
    addtp/=8;
    set_index=addtp%8;//set_index从000,001,010,011,100,101,110,111共8个
    addtp/=8;//<16
    tag=addtp;
    if(sets[set_index].valid&&sets[set_index].tag==tag)//hit
        return sets[set_index].blocks[offset];
    else 
    {
        sets[set_index].miss(set_index,tag);//获得新的set值
        return sets[set_index].blocks[offset];
    }
}

void Cache::write(long long add,int val)//val是一个byte
{
    int offset;
    int set_index;
    int tag;
    long long addtp=add;
    offset=addtp%8;
    addtp/=8;
    set_index=addtp%8;
    addtp/=8;
    tag=addtp;
    if(sets[set_index].valid&&sets[set_index].tag==tag)//hit
    {
        sets[set_index].blocks[offset]=val;
    }
    else 
    {
        sets[set_index].miss(set_index,tag);//获得新的set值
        sets[set_index].blocks[offset]=val;
    }
    sets[set_index].dirty=1;
}

void Cache::cache_memory()
{
    for(int i=0;i!=8;i++)
    {
        Set tp=sets[i];
        long long add=0;
        add+=tp.tag;
        add=add<<3;
        add+=i;
        add=add<<3;
        for(int j=0;j!=8;j++)
        {
            memory[add+j]=tp.blocks[j];
        }
    }
}

void D::decode()
{
    dstM=NONE;dstE=NONE;srcA=16;srcB=16;//默认一个不修改的寄存器
    rA=dreg.rA;rB=dreg.rB;
    valP=dreg.valP;valC=dreg.valC;
    valA=valB=0;
    icode=dreg.icode;ifun=dreg.ifun;stat=dreg.stat;
    switch(icode)
    {
        case RR:
            valA=reg[rA];
            dstE=rB;
            srcA=rA;
            dstE=rB;break;
        case IR:
            dstE=rB;break; 
        case RM:
            valB=reg[rB];
            srcA=rA;srcB=rB;
            break;
        case MR:
            srcB=rB;
            dstM=rA;
            valB=reg[rB];
            break;
        case OP:
            valA=reg[rA];
            valB=reg[rB];
            srcA=rA;srcB=rB;
            dstE=rB;break;
        case JXX:
            break;
        case CALL:
            valB=reg[RSP];
            srcB=RSP;
            dstE=RSP;break;
        case RET:
            valA=reg[RSP];
            valB=reg[RSP];
            srcA=RSP;srcB=RSP;
            dstE=RSP;break;
        case PUSH:
            valA=reg[rA];
            valB=reg[RSP];
            srcA=rA;srcB=RSP;
            dstE=RSP;break;
        case POP:
            valA=reg[RSP];
            valB=reg[RSP];
            srcA=RSP;srcB=RSP;
            dstE=RSP;dstM=rA;
    }
}

void E::execute()
{
    valA=ereg.valA;valB=ereg.valB;valC=ereg.valC;
    icode=ereg.icode;stat=ereg.stat;ifun=ereg.ifun;
    dstE=ereg.dstE;dstM=ereg.dstM;
    valE=0;
    switch(icode)
    {
        case RR:
            valE=valA;
            switch(ifun)
            {
                case 0: Cnd=1; break;
                case 1: Cnd=(SF^OF)|ZF;break;
                case 2: Cnd=SF^OF;break;
                case 3: Cnd=ZF;break;
                case 4: Cnd=!ZF;break;
                case 5: Cnd=!(SF^OF);break;
                case 6: Cnd=!(SF^OF)&!ZF;break;
                default: Cnd=0;
            }
            if(!Cnd)
            dstE=NONE;
            break;
        case IR:
            valE=valC;break;
        case RM:
        case MR:
            valE=valC+valB;break;
        case OP:
            switch(ifun) 
            {
                case 0:valE=valB+valA;break;
                case 1:valE=valB-valA;break;
                case 2:valE=valB&valA;break;
                case 3:valE=valB^valA;break;
            }
            break;
        case JXX:
            switch(ifun)
            {
                case 0: Cnd=1; break;
                case 1: Cnd=(SF^OF)|ZF;break;
                case 2: Cnd=SF^OF;break;
                case 3: Cnd=ZF;break;
                case 4: Cnd=!ZF;break;
                case 5: Cnd=!(SF^OF);break;
                case 6: Cnd=!(SF^OF)&!ZF;break;
                default: Cnd=0;
            }
            break;
        case CALL:
        case PUSH:
            valE=valB-8;break;
        case RET:
        case POP:
            valE=valB+8;break;
    }
}

bool valid(int i)
{
    return i<16&&i>=0;
}

void F::fetch()
{
    icode=memory[PC]/16;
    ifun=memory[PC]%16;
    int instr_valid=icode<12&&icode>=0;
    if(icode==OP)
    instr_valid=instr_valid&&(ifun<4&&ifun>=0);
    if(icode==JXX)
    instr_valid=instr_valid&&(ifun<7&&ifun>=0);
    if(icode==CM)
    instr_valid=instr_valid&&(ifun<7&&ifun>=0);
    rA=rB=15;//先默认一个不修改的寄存器
    int imem_error=0;long long sum=0;
    switch(icode)
    {
        case OP:
        case PUSH:
        case POP:
        case RR:
            rA=memory[PC+1]/16;
            rB=memory[PC+1]%16;
            valP=PC+2;
            imem_error=!(valid(rA)&&valid(rB));
            break;
        case IR:
        case RM:
        case MR:
            rA=memory[PC+1]/16;
            rB=memory[PC+1]%16;
            for(int i=PC+9;i!=PC+1;i--)
            {
                sum=sum<<8;
                sum+=memory[i];
            }
            valC=sum;
            valP=PC+10;
            imem_error=!(valid(rA)&&valid(rB));
            break;
        case JXX:
        case CALL:
            for(int i=PC+8;i!=PC;i--)
            {
                sum=sum<<8;
                sum+=memory[i];
            }
            valC=sum;
            valP=PC+9;break;
        case RET:
        case HALT:
        case NOP:valP=PC+1;break;
    }
    if(imem_error) stat=ADR;
    else if(!instr_valid) stat=INS;
    else if(icode==HALT) stat=HLT;
    else stat=AOK;
}

void M::memo()
{
    icode=mreg.icode;stat=mreg.stat;
    valE=mreg.valE;valA=mreg.valA;valM=0;
    dstE=mreg.dstE;dstM=mreg.dstM;
    long long sum=0;bool mem_error=0;
    switch(icode)
    {
        case MR:
            if(valE<0)
            {mem_error=1;break;}
            for(int i=7;i!=-1;i--)
            {
                sum=sum<<8;
                sum+=cache.visit(valE+i);
            }
            valM=sum;
            break;
        case RET:
        case POP:
            if(valA<0)
            {mem_error=1;break;}
            for(int i=7;i!=-1;i--)
            {
                sum=sum<<8;
                sum+=cache.visit(valA+i);
            }
            valM=sum;
            break;
        case RM:
        case PUSH:
            if(valE<0)
            {mem_error=1;break;}
            for(int i=0;i!=8;i++)
            {
                cache.write(valE+i,valA&255);
                valA=valA>>8;
            }//这样写回的负数依旧保持补码的形式
            break;
        case CALL:
            if(valE<0)
            {mem_error=1;break;}
            for(int i=0;i!=8;i++)
            {
                cache.write(valE+i,valA&255);
                valA=valA>>8;
            }//这样写回的负数依旧保持补码的形式
            break;
        case NOP:
        case HALT:;
    }
    if(mem_error) stat=ADR;
    else stat=mreg.stat;
}

void SelectPC()
{
    //jxx
    if(mreg.icode==JXX&&!mreg.Cnd)
        PC=mreg.valA;
    //ret
    else if(wreg.icode==RET)
        PC=wreg.valM;
    else PC=freg.predPC;
}

void F::f_pred()
{
    if(icode==CALL||icode==JXX)
        f.predPC=f.valC;
    else f.predPC=f.valP;
}

//这个函数唯一的目的是把读入的文件写入memory并改写为自己适应的形式

int turn(char c)
{
    if(c<='9'&&c>='0')
    return c-'0';
    else return c-'a'+10;
}
int decoder()
{
L1:
    PC=0;
    char c;
    string s;
    cin>>s;
    ifstream yo(s,ios::in);
    if(!yo) {cout<<"Sorry, but the desitination is wrong, would you please try another time?"<<endl;goto L1;}
    while(yo.good())
    {
        c=yo.get();
        if(c==' ')
        {
            while(yo.good()&&((c=yo.get())!='\n'));
            continue;
        }
        int num[21];
        int ct=0,num1,num2,num3;
        yo.get();
        num1=yo.get();num2=yo.get();num3=yo.get();
        PC=turn(num1)*16*16+turn(num2)*16+turn(num3);
        yo.get();yo.get();
        while(yo.good())
        {
            c=yo.get();
            if(c==' ')
            break;
            num[ct]=turn(c);
            ct++;
        }
        for(int i=0;i<ct;i+=2)
        {
            memory[PC+i/2]=num[i]*16+num[i+1];
            //cout<<memory[PC+i/2]<<"  ";
        }
        //cout<<endl;
        while(yo.good()&&(yo.get()!='\n'));
    }
    return 0;
}

//-----------------------------------
//写入流水线寄存器阶段
//这一部分做得所有事情是从上一个流水寄存器中获得值写入下一个
//或者从之前的计算情况，即cons_code的保存值中流出给下一个
//并且通过判断bubble或者stall值来判断修改
//-----------------------------------
        
void run_in_reg()
{
    if(freg.stall);
    else
        freg.predPC=f.predPC;

    //fetch to decode
    if(dreg.stall);//什么也不做
    else if(dreg.bubble) 
    {dreg.icode=1;dreg.stat=AOK;}
    else {
        dreg.icode=f.icode;dreg.ifun=f.ifun;dreg.stat=f.stat;
        dreg.rA=f.rA;dreg.rB=f.rB;
        dreg.valC=f.valC;dreg.valP=f.valP;
    }

    //decode to execute
    if(ereg.bubble) {ereg.icode=1;ereg.stat=AOK;}
    else if(ereg.stall);//什么也不做
    else
    {ereg.stat=d.stat;ereg.icode=d.icode;ereg.ifun=d.ifun;
    ereg.valC=d.valC;ereg.valA=d.valA;ereg.valB=d.valB;
    ereg.srcA=d.srcA;ereg.srcB=d.srcB;
    ereg.dstE=d.dstE;ereg.dstM=d.dstM;}

    //execute to memory
    if(mreg.bubble) {mreg.icode=1;mreg.stat=AOK;}
    else if(mreg.stall);
    else
    {
        mreg.valE=e.valE;mreg.valA=e.valA;
        mreg.dstE=e.dstE;mreg.dstM=e.dstM;
        mreg.icode=e.icode;mreg.stat=e.stat;
        if(set_cc)
        {
            ZF=e.valE==0;
            SF=e.valE<0;
            if(e.ifun!=1)
                if((e.valB<0==e.valA<0)&&(e.valE<0!=e.valB<0))
                    OF=1;
                else OF=0;
            else if(e.ifun==1)
                if((e.valB<0!=e.valA<0)&&(e.valE<0!=e.valB<0))
                    OF=1;
                else OF=0;
        }

        //写入的权限这里也会进行控制
        mreg.Cnd=e.Cnd;
    }

    //memory to write
    if(wreg.bubble) {wreg.icode=1;wreg.stat=AOK;}
    else if(wreg.stall);
    else
    {wreg.icode=m.icode;wreg.stat=m.stat;
    wreg.dstE=m.dstE;wreg.dstM=m.dstM;
    wreg.valM=m.valM;wreg.valE=m.valE;}

    if(wreg.stat==AOK)
    Stat=0;
    else Stat=wreg.stat;
    //write
    if(wreg.icode==NOP)return;
    if(!(Stat==0||Stat==HLT))return;
    reg[wreg.dstE]=wreg.valE;
    reg[wreg.dstM]=wreg.valM;//M的优先级更高
}

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

class Data
{
//这个数据结构接受之前所有的计算结果，并且把之前的结果压缩在栈里面
//通过弹栈来恢复上一个数据，当然这样会消耗巨大的空间，当循环次数太长的时候
    public:
    stack<int> hisR;
    stack<int> hisPC;
    stack<int> hisStat;
    stack<Cache> hisCache;
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

    bool dataGet();//1 for success, 0 for error
    void dataStore();
    void memoryDataStore();
};

bool Data::dataGet()
{
    if(hisR.empty())
    {
        cout<<"No, you can't go further, that's will take you out of this space! Try go ahead!"<<endl;
        return 0;
    }
    cache=hisCache.top();hisCache.pop();
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
    return 1;
}

void Data::dataStore()
{
    hisPC.push(PC);hisStat.push(Stat);
    hisZF.push(ZF);hisSF.push(SF);hisOF.push(OF);
    hisset_cc.push(set_cc);
    hisR.push(r);
    hisCache.push(cache);
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
    else if(mreg.icode==MR&&mreg.valE>=0)
    {
        long long sum=0;
        for(int i=7;i!=-1;i--)
        {
            sum=sum<<8;
            sum+=memory[mreg.valE+i];
        }
        hisMemory.push(pair<int,long long>(mreg.valE,sum));
    }
    else if((mreg.icode==RET||mreg.icode==POP)&&mreg.valA>=0)
    {
        long long sum=0;
        for(int i=7;i!=-1;i--)
        {
            sum=sum<<8;
            sum+=memory[mreg.valA+i];
        }
        hisMemory.push(pair<int,long long>(mreg.valA,sum));
    }
    else hisMemory.push(pair<int,long>(-1,0));
}
string error[]={"AOK","AOK","HLT","ADR","INS"};
Data Path;
//用来控制是否需要后退
bool back=0;
void BACK(){back=1;}//这个函数用来修改back的值用以回退
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
    cout<<"G'Day, nice to meet you! This is a y86-simulator version 1.1's terminal version produced by Runpeng Xie and Mingzhe Zhu. "<<endl;
    cout<<"Hope you have nice time here. Please input the location of your .yo file, and then we can start!"<<endl;
    freg.predPC=0;ZF=1;SF=0;OF=0;//初始化
    decoder();mreg.Cnd=1;
    dreg.icode=wreg.icode=mreg.icode=ereg.icode=1;
    f.icode=d.icode=e.icode=m.icode=0;
    f.stat=dreg.stat=d.stat=e.stat=ereg.stat=m.stat=mreg.stat=wreg.stat=AOK;
    reg[RSP]=0;int flag=0;
    int time_machine;bool flag_time_machine=0;
    while(1)
    {
        if(back)
        {Path.dataGet();back=0;}
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
            Path.dataStore();
            Path.memoryDataStore();
            SelectPC();
            run_in_cons();
            forward();
            bubble_stall_set();
            run_in_reg();
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
        cout<<"   dstE="<<wreg.dstE<<"  Stat="<<dreg.stat<<endl;
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
        else break;
    }
    cout<<"Thanks for using, see you next time! Wish you a good day."<<endl;
    return 0;
}