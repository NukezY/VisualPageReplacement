#include "lru.h"

lru::lru()
{

}

bool lru::exist(int a,int page[],int pageIndx)
     // 判断page数组中是否有a,pageIndx不会超过pagesize
{
    for(int i=0;i<pageIndx;i++)
    {
        if(a==page[i])
        {
            int temp=page[i];
            for(int j=i;j<pageIndx-1;j++)
            {
                page[j]=page[j+1];
            }
            page[pageIndx-1]=temp;
            // 如果访问串当前值在page数组中纯在，则把当前值在的page数组的位置移至最后。也把最长时间未使用的值移到了最开始。
            return true;
        }
    }
    return false;

}
void lru::work(int a,int page [],int  pagesize,int & pageIndx,int & lackpage)
        //(序列中的当前值），(内存块中的值)，(内存块数),(page数组索引),(缺页数)
{
    if(!exist(a,page,pageIndx)) // 如果序列当前值不在page数组中
    {
        lackpage++;
        if(pageIndx<pagesize)   // 如果索引小于pagesize,则说明page中存在 -1 也就是空页！！
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
