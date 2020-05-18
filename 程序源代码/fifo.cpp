#include "fifo.h"

fifo::fifo()
{

}

bool fifo::exist(int a,int page[],int pageIndx)
    // 判断page数组中是否有a,pageIndx不会超过pagesize
{
    for(int i=0;i<pageIndx;i++)
    {
        if(a==page[i])
            return true;
    }
    return false;

}
void fifo::work(int a,int page [],int  pagesize,int & pageIndx,int & lackpage)
        //(序列中的当前值），(内存块中的值)，(内存块数),(page数组索引),(缺页数)
{
    if(!exist(a,page,pageIndx)) // 如果序列当前值不在page数组中
    {
        lackpage++;
        if(pageIndx<pagesize) // 如果索引小于pagesize,则说明page中存在 -1 也就是空页！！
        {
            page[pageIndx]=a;
            pageIndx++;
        }
        else
        {
            for(int j=0;j<pagesize-1;j++)
            {
                page[j]=page[j+1];
            }
            page[pagesize-1]=a;
            //   算法设计 把不符合该算法的值(也就是page数组的第一个移除),把当前值加入数组。
        }
    }
}
