#include<string>
#include<map>
using namespace std;
long long reg[16];//寄存器数组，下标代表寄存器的名字，比如0代表rax内的值
//为了方便reg保存的就是真实的值，而在memory李保存的是以8位为一个值的数字。
//这样的代价是，当你从memory中取值或者往memory中写值时需要将reg中的值转化为8字节的形式
int PC,state,f_pc,F_predPC=0,f_predPC;//地址与状态值,PC本质上是使用一个map去维护的，对于每一个PC值存在一个与之对应的cons_code结构
//PC相当于这个数组的下标
int ZF,SF,OF;
int memory[200000];//模拟内存，里面有模拟的栈和模拟的指令集，所有的数字都不超过16*16以使每一个数组中的元素模拟都一个字节
//里面的数字都以8个字节为一个单位存储。刚好和命令的最长长度相同。并且保留补码的形式

class cons_code
{
//这是一个命令，里面包含有流水线的每一步需要的值
//流水线的每一步需要传递的就是这一个值
    public:
    int ifun;
    int icode;
    int rA;
    int rB;
    int valP;//PC的更新值
    int dstE;
    int srcA;
    bool Cnd;
    long long valC;//从命令取出的值
    int stat;
    long long valA;//从rA取得的值
    long long valB;//从rB取得的值
    long long valE;//通过execu获得的值
    long long valM;//从memory中获得的值

    void fetch();
    void decode();
    void execute();
    void memo();
    void write();
    void PCchange();
    void f_pred();
};
cons_code f,d,e,m,w;//fetch阶段等的数据保留
//一下四个是流水线寄存器
class Wreg{
    public:
    int stat,icode,dstE,dstM;
    long long valE,valM;
}wreg;

class Mreg{
    public:
    int stat,icode,dstE,dstM;
    long long valE,valA;
    int  Cnd;

}mreg;

class Ereg{
    public:
    int stat,icode,ifun,dstE,dstM,scrA,scrB;
    long long valB,valA,valC;
}ereg;

class Dreg{
    public:
    int stat,icode,ifun,rA,rB;
    long long valC,valP;
}dreg;
