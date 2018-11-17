#include"y86_essence.h"
#include<fstream>
#include<iostream>
//这个函数唯一的目的是把读入的文件写入memory并改写为自己适应的形式
using namespace std;

int decoder()
{
    PC=0;
    ifstream yo("C:\\y86-64\\yo\\prog5.yo",ios::in);
    char c;
    if(!yo)
    cout<<"wrong"<<endl;
    while(yo.good()&&(c=yo.get())!=':');
    while(yo.good())
    {
        c=yo.get();
        if(c=='\n')
        continue;
        int num[21];
        while(yo.good()&&(c=yo.get())!=':');
        int ct=0;
        while(yo.good()&&(c=yo.get())!='|')
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
            memory[PC+i/2]=num[i]*16+num[i+1];
        }
        PC+=ct/2;
    }

    return 0;
}