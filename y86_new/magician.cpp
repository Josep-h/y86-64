#include"y86_essence.h"
#include<iostream>
#include<string>
using namespace std;

void magician_register(int id, long long num)
{
    if(id>15||id<0) {cout<<"Wrong instruciton!"<<endl;return;}
    long long before=reg[id];
    reg[id]=num;
    string putout="The register ";
    putout+=registers[id];
    putout+=" has been change from ";
    putout+=to_string(before);
    putout+=" to ";
    putout+=to_string(num);
    historyChange.push_back(putout);
    Path.dataStore();
}

void magician_cache(int id, long long num)
{
    if(id>7||id<0) {cout<<"Wrong instruciton!"<<endl;return;}
    long long before=0;
    for(int i=7;i!=-1;i--)
    {
        before=before<<8;
        before+=cache.sets[id].blocks[i];
    }//保留之前的数
    for(int i=0;i!=8;i++)
    {
        cache.sets[id].blocks[i]=num&256;
        num=num>>8;
    }//写入新的cache

    string putout="The cache ";
    putout+=to_string(id);
    putout+=" has been change from ";
    putout+=to_string(before);
    putout+=" to ";
    putout+=to_string(num);
    historyChange.push_back(putout);//这里需要改成十六进制
    Path.dataStore();
}

void magician_memory(int add, long long num)
{
    if(add<0) {cout<<"Wrong instruciton!"<<endl;return;}
    long long before=0;
    for(int i=7;i!=-1;i--)
    {
        before=before<<8;
        before+=memory[add+i];
    }//保留之前的数
    for(int i=0;i!=8;i++)
    {
        memory[add+i]=num&256;
        num=num>>8;
    }//写入新的cache

    string putout="The memory with address 0x";

    putout+=to_string(add);
    putout+=" has been change from ";
    putout+=to_string(before);
    putout+=" to ";
    putout+=to_string(num);
    historyChange.push_back(putout);
    Path.dataStore();
}

void magician_cons(char stage, string s, long long num)
{
    string putout="The stage ";
    long long before;
    if(stage=='F')
    {
        if(num<0) {cout<<"Wrong instruciton!"<<endl;return;}
        putout+="F's "; 
        putout+="predPC has been changed from ";
        before=f.predPC;
        putout+=to_string(before)+" to ";
        putout+=to_string(num);
        freg.predPC=num;
    }
    if(stage=='D')
    {
        putout+="D's "; 
        if(s=="icode")
        {
            if(num>11||num<0) {cout<<"Wrong instruciton!"<<endl;return;}
            putout+="icode has been changed from ";
            before=dreg.icode;
            putout+=to_string(before)+" to ";
            putout+=to_string(num);
            dreg.icode=num;
        }
        if(s=="ifun")
        {
            putout+="ifun has been changed from ";
            before=dreg.ifun;
            putout+=to_string(before)+" to ";
            putout+=to_string(num);
            dreg.ifun=num;
        }
        if(s=="rA")
        {
            if(num>15||num<0) {cout<<"Wrong instruciton!"<<endl;return;}
            putout+="rA has been changed from ";
            before=dreg.rA;
            putout+=to_string(before)+" to ";
            putout+=to_string(num);
            dreg.rA=num;
        }
        if(s=="rB")
        {
            if(num>15||num<0) {cout<<"Wrong instruciton!"<<endl;return;}
            putout+="rB has been changed from ";
            before=dreg.rB;
            putout+=to_string(before)+" to ";
            putout+=to_string(num);
            dreg.rB=num;
        }
        if(s=="valC")
        {
            putout+="valC has been changed from ";
            before=dreg.valC;
            putout+=to_string(before)+" to ";
            putout+=to_string(num);
            dreg.valC=num;
        }
        if(s=="valP")
        {
            putout+="valP has been changed from ";
            before=dreg.valP;
            putout+=to_string(before)+" to ";
            putout+=to_string(num);
            dreg.valP=num;
        }
    }
    if(stage=='E')
    {
        putout+="E's "; 
        if(s=="dstE")
        {
            if(num>15||num<0) {cout<<"Wrong instruciton!"<<endl;return;}
            putout+="dstE has been changed from ";
            before=ereg.dstE;
            putout+=to_string(before)+" to ";
            putout+=to_string(num);
            ereg.dstE=num;
        }
        if(s=="dstM")
        {
            if(num>15||num<0) {cout<<"Wrong instruciton!"<<endl;return;}
            putout+="dstM has been changed from ";
            before=ereg.dstM;
            putout+=to_string(before)+" to ";
            putout+=to_string(num);
            ereg.dstM=num;
        }
        if(s=="valA")
        {
            putout+="valA has been changed from ";
            before=ereg.valA;
            putout+=to_string(before)+" to ";
            putout+=to_string(num);
            ereg.valA=num;
        }
        if(s=="valB")
        {
            putout+="valB has been changed from ";
            before=ereg.valB;
            putout+=to_string(before)+" to ";
            putout+=to_string(num);
            ereg.valB=num;
        }
        if(s=="valC")
        {
            putout+="valC has been changed from ";
            before=ereg.valC;
            putout+=to_string(before)+" to ";
            putout+=to_string(num);
            ereg.valC=num;
        }
        if(s=="srcA")
        {
            if(num>15||num<0) {cout<<"Wrong instruciton!"<<endl;return;}
            putout+="srcA has been changed from ";
            before=ereg.srcA;
            putout+=to_string(before)+" to ";
            putout+=to_string(num);
            ereg.srcA=num;
        }
        if(s=="srcB")
        {
            if(num>15||num<0) {cout<<"Wrong instruciton!"<<endl;return;}
            putout+="srcB has been changed from ";
            before=ereg.srcB;
            putout+=to_string(before)+" to ";
            putout+=to_string(num);
            ereg.srcB=num;
        }
    }
    historyChange.push_back(putout);//这里需要改成十六进制
    Path.dataStore();
}