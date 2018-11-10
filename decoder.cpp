<<<<<<< HEAD
#include"y86_essence.h"
#include<fstream>
#include<iostream>
//这个函数唯一的目的是把读入的文件写入memory并改写为自己适应的形式
using namespace std;

int decoder()
{
    PC=0;
    fstream yo("C:\\cpparea\\pj_y86\\test.txt",ios::in);
    char c;
    if(!yo)
    cout<<"wrong"<<endl;
    while((c=yo.get())!=-1)
    {
        if(c=='\n')
        continue;
        int num[21];
        while((c=yo.get())!=':');
        int ct=0;
        while((c=yo.get())!='|')
        {
            if(c==' ')
            continue;
            if(c<='9'&&c>='0')
            num[ct]=c-'0';
            else num[ct]=c-'a'+10;
            ct++;
        }
        for(int i=0;i<ct;i+=2)
        {
            memory[PC+i]=num[i]*16+num[i+1];
            cout<<memory[PC+i]<<" ";
        }
        PC+=(ct-1)/2;
    }
    return 0;
=======
#include"y86_essence.h"
#include<fstream>
#include<iostream>
//这个函数唯一的目的是把读入的文件写入memory并改写为自己适应的形式
using namespace std;

int decoder()
{
    PC=0;
    fstream yo("C:\\cpparea\\pj_y86\\test.txt",ios::in);
    char c;
    if(!yo)
    cout<<"wrong"<<endl;
    while((c=yo.get())!=-1)
    {
        if(c=='\n')
        continue;
        int num[21];
        while((c=yo.get())!=':');
        int ct=0;
        while((c=yo.get())!='|')
        {
            if(c==' ')
            continue;
            if(c<='9'&&c>='0')
            num[ct]=c-'0';
            else num[ct]=c-'a'+10;
            ct++;
        }
        for(int i=0;i<ct;i+=2)
        {
            memory[PC+i]=num[i]*16+num[i+1];
            cout<<memory[PC+i]<<" ";
        }
        PC+=(ct-1)/2;
    }
    return 0;
>>>>>>> 7eed4ef345740a6efc3975095d6010242c561537
}