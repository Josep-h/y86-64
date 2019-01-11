#pragma once
#include"y86_essence.h"

class Set{//为了方便
//这里的block里面存储的值就直接用long long型代替了
    public:
    bool valid;bool dirty;
    int tag;
    int blocks[8];//1个byte，一个set可以存1个64位数
    Set(){dirty=0;}
    void miss(int set_num,int tag_new);
};

class Cache{
    public:
    Set sets[8];//cache里一个有8个set，每一个set一line，每一line存16个byte
    int visit(long long add);//地址是一个整型值，标注的是一个64位数的开头，返回一个byte
    void write(long long add,int val);//val是一个byte
    void cache_memory();
};

void Set::miss(int set_index,int tag_new)//访问内存并从内存中取出值放入相应的set里面
{
    long long add=0;
    add+=tag;
    add=add<<3;
    add+=set_index;
    add=add<<3;
    //先确定set是否被修改过
    //如果是，需要先写入内存然后再写入set
    if(dirty)
    {
        for(int i=0;i!=8;i++)
        memory[add+i]=blocks[i];
    }
    add=0;
    add+=tag_new;add=add<<3;add+=set_index;add=add<<3;//计算新的地址值
    for(int i=0;i!=8;i++)
    {
        for(int i=0;i!=8;i++)
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
    addtp=addtp>>3;
    set_index=addtp%8;//set_index从000,001,010,011,100,101,110,111共8个
    tag=addtp>>3;//<16
    if(sets[set_index].tag==tag)//hit
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
    long long addtp;
    offset=addtp%8;
    addtp=addtp>>3;
    set_index=addtp%8;
    tag=addtp>>3;
    if(sets[set_index].tag==tag)//hit
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