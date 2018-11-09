#include<string>
#include<map>
using namespace std;
int ZF=0,SF=0,OF=0;//条件值
long long reg[16];//寄存器数组，下标代表寄存器的名字，比如0代表rax内的值
//为了方便reg保存的就是真实的值，而在memory李保存的是以8位为一个值的数字。
//这样的代价是，当你从memory中取值或者往memory中写值时需要将reg中的值转化为8字节的形式
int PC,state;//地址与状态值,PC本质上是使用一个map去维护的，对于每一个PC值存在一个与之对应的cons_code结构
//PC相当于这个数组的下标
int memory[2000000];//模拟内存，里面有模拟的栈和模拟的指令集，所有的数字都不超过16*16以使每一个数组中的元素模拟都一个字节
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
    int dest;
    int valP;//PC的更新值
    bool Cnd;
    long long valC;//从命令取出的值
    int stat;
    long long valA;//从rA取得的值
    long long valB;//从rB取得的值
    long long valE;//通过execu获得的值
    long long valM;//从memory中获得的值
    int dstM;
    int dstE;
    //rrmovq
    void rrmovq_fetch(cons_code &code);
    void rrmovq_decode(cons_code &code);
    void rrmovq_execute(cons_code &code);
    void rrmovq_write(cons_code &code);
    void rrmovq_PC(cons_code &code);

    //irmovq
    void irmovq_fetch(cons_code &code);
    //without int irmovq_decode(cons_code &code);
    void irmovq_execute(cons_code &code);
    void irmovq_write(cons_code &code);
    void irmovq_PC(cons_code &code);

    //mrmovq
    void mrmovq_fetch(cons_code &code);
    void mrmovq_decode(cons_code &code);
    void mrmovq_execute(cons_code &code);
    void mrmovq_memo(cons_code &code);
    void mrmovq_write(cons_code &code);
    void mrmovq_PC(cons_code &code);

    //rmmovq
    void rmmovq_fetch(cons_code &code);
    void rmmovq_decode(cons_code &code);
    void rmmovq_execute(cons_code &code);
    void rmmovq_memo(cons_code &code);
    //without void rmmovq_write(cons_code &code);
    void rmmovq_PC(cons_code &code);

    //Op
    void Op_fetch(cons_code &code);
    void Op_decode(cons_code &code);
    void Op_execute(cons_code &code);
    void Op_write(cons_code &code);
    void Op_PC(cons_code &code);

    //push
    void push_fetch(cons_code &code);
    void push_decode(cons_code &code);
    void push_execute(cons_code &code);
    void push_memo(cons_code &code);
    void push_write(cons_code &code);
    void push_PC(cons_code &code);

    //pop
    void pop_fetch(cons_code &code);
    void pop_decode(cons_code &code);
    void pop_execute(cons_code &code);
    void pop_memo(cons_code &code);
    void pop_write(cons_code &code);
    void pop_PC(cons_code &code);

    //jxx
    void jxx_fetch(cons_code &code);
    void jxx_decode(cons_code &code);
    void jxx_execute(cons_code &code);
    void jxx_memo(cons_code &code);
    void jxx_write(cons_code &code);
    void jxx_PC(cons_code &code);

    //call
    void call_fetch(cons_code &code);
    void call_decode(cons_code &code);
    void call_execute(cons_code &code);
    void call_memo(cons_code &code);
    void call_write(cons_code &code);
    void call_PC(cons_code &code);

    //ret
    void ret_fetch(cons_code &code);
    void ret_decode(cons_code &code);
    void ret_execute(cons_code &code);
    void ret_memo(cons_code &code);
    void ret_write(cons_code &code);
    void ret_PC(cons_code &code);

    //cmov
    void cmov_fetch(cons_code &code);
    void cmov_decode(cons_code &code);
    void cmov_execute(cons_code &code);
    void cmov_memo(cons_code &code);
    void cmov_write(cons_code &code);
    void cmov_PC(cons_code &code);
};



