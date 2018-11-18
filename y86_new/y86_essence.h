#pragma once
#include<string>
using namespace std;
//常量
const int AOK=1,HLT=2,ADR=3,INS=4; 
const int HALT=0,NOP=1,RR=2,IR=3,RM=4,MR=5,OP=6,JXX=7,CM=2,CALL=8,RET=9,PUSH=10,POP=11;
const int RSP=4,NONE=15;
int ZF,SF,OF,set_cc;//条件寄存器
long long reg[16];//寄存器

int PC=0,Stat;
//PC相当于这个数组的下标
int memory[20000];
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