#pragma once
#include<string>
#include<vector>
#include"stack.cpp"
#include<fstream>
#include<iostream>
using namespace std;
//常量
const int AOK=1,HLT=2,ADR=3,INS=4; 
string registers[]={"rax","rcx","rdx","rbx","rsp","rbp","rsi","rdi","r8","r9","r10","r11","r12","r13","r14","NONE"};
const int HALT=0,NOP=1,RR=2,IR=3,RM=4,MR=5,OP=6,JXX=7,CM=2,CALL=8,RET=9,PUSH=10,POP=11;
const int RSP=4,NONE=15;
int ZF,SF,OF,set_cc;//条件寄存器
long long reg[16];//寄存器
int r=0;
int PC=0,Stat;
Data Path;
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
