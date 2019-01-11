#pragma once
#include<string>
#include<vector>
#include<fstream>
#include<iostream>
#include<stack>
using namespace std;
//常量
const int AOK=1,HLT=2,ADR=3,INS=4; 
string registers[]={"rax","rcx","rdx","rbx","rsp","rbp","rsi","rdi","r8","r9","r10","r11","r12","r13","r14","NONE"};
const int HALT=0,NOP=1,RR=2,IR=3,RM=4,MR=5,OP=6,JXX=7,CM=2,CALL=8,RET=9,PUSH=10,POP=11;
const int num_table[]={0,1,2,3,4,5,6,7,8,9,10,11};
bool SIG_F=0,SIG_D=0,SIG_E=0,SIG_M=0,SIG_forward=0,SIG_FB=0,SIG_DB=0,SIG_SET=0,SIG_EB=0,SIG_WB=0,SIG_MB=0;
const int RSP=4,NONE=15;
int ZF,SF,OF,set_cc;//条件寄存器
long long reg[16];//寄存器
int r=0;
int PC=0,Stat;
//PC相当于这个数组的下标
int memory[1024];//地址位需要10位完全确定一个位置
vector<string> historyChange;//修改记录
ofstream outfile;

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

class Data
{
//这个数据结构接受之前所有的计算结果，并且把之前的结果压缩在栈里面
//通过弹栈来恢复上一个数据，当然这样会消耗巨大的空间，当循环次数太长的时候
    public:
    stack<int> hisR;
    stack<int> hisPC;
    stack<int> hisStat;
    stack<Cache> hisCache;
    stack<vector<string> > hisChange;//修改记录
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
}Path;

void FectchBubbleSetAndLoad();
void DecodeBubbleSetAndLoad();
void ExecuteBubbleSetAndLoad();
void WriteBubbleSetAndLoad();
void set_cc_fun();
void MemoryBubbleSetAndLoad();
void to_use_fetch(int id);
void OneCycle();
void SIG_CLEAR();
