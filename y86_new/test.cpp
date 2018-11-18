#include<iostream>
using namespace std;
int main()
{
    int ans1=0;
    long long ans2=0;
    __int64 ans3=0;

    for(int i=0;i!=8;i++)
    {
        ans1=ans1<<8;ans2=ans2<<8;ans3=ans3<<8;
        ans1+=255;ans2+=255;ans3+=255;
        cout<<ans1<<" "<<ans2<<" "<<ans3<<" "<<endl;
    }
    
    for(;;);
    return 0;
}