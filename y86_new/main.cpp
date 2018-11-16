#include"y86_essence.h"
#include"decode.cpp"
#include"execute.cpp"
#include"fetch.cpp"
#include"memory.cpp"
#include"PC.cpp"
#include"write.cpp"
#include"decoder.cpp"
#include"run_in_cons.cpp"
#include"run_in_reg.cpp"
#include"middle.cpp"
#include"bubble_stall_set.cpp"

#include<string>

using namespace std;

int main()
{
    PC=0;
    freg.predPC=0;//初始化
    decoder();
    reg[4]=10000;
    while(1)
    {
        //SelectPC
        SelectPC();

        //这一部分是将流水寄存器中的值取出但是并不写入下一个流水寄存器
        //而是写到一个中间变量即cons_code中
        run_in_cons(r);

        //转发的部分
        middle();

        //特殊情况控制
        //设置异常状态值
        bubble_stall_set();
        
        //时钟上升沿触发
        run_in_reg();
        wreg.write();
    }
}