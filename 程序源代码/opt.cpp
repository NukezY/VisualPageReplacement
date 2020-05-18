#include "opt.h"
#include <iostream>
using namespace std;
opt::opt()
{

}

int opt::exist(int a, int page[], int startid, int endid)
{
    for(int i=startid;i<endid;i++)
    {
        if(a==page[i])
        {
            return i; // 返回在访问串中的索引
        }
    }
    return -1;
}
int opt::choose(int taskid, int page[], int task[],int tasksize,int pagesize)
            // (访问串当前值的索引)，(page数组),(访问串数组),(访问串长度),(内存块数)
{
    int *time  = new int [pagesize];
    int max = 999;
    int tid;
    int t = 0 , position = 0;
    for(int i = 0 , j = 0 ; i <pagesize ; i ++ , j ++){
        tid=exist(page[i], task, taskid ,tasksize); //  从访问串当前值开始到访问串结束
        // 找出page数组中的值在访问串中下从出现的位置
        if(tid == -1){
            time[j] = max;
            // 如果当前值在后续访问串中不再出现，则把时间值记为最大；
            // 即不再访问的页
        }
        else{
            time[j] = tid;
            // 出现的话，记录下次出现的在访问串中的索引
        }
        // 找出page数组的值在访问串下从出现的索引，并记录
    }
    for(int i = 0 ; i <pagesize ; i ++){
  //      cout<<"time[i]:"<<time[i]<<" ";   调试用
        if(time[i] > t){
            t = time[i];
            position = i;
            // 找出page数组中下从出现的索引最长的哪一个，并把它在page数组中的索引记录
        }
    }
 //   cout<<"position:"<<position<<" "<<endl;   调试用
    return position;

}

void opt::work(int task[], int taskid, int page[],int tasksize ,int pagesize, int &pageIndx, int &lackpage)
        // (访问串数组),(当前值在访问串中的索引),(page数组),(访问串长度),(内存块数
{
    if(exist(task[taskid], page,0, pageIndx)==-1)  //缺页，如果序列当前值不在page数组中
    {
        lackpage++;
        if(pageIndx<pagesize)
        {
            page[pageIndx]=task[taskid];
            pageIndx++;
        }
        else
        {
            int position=choose(taskid,page,task,tasksize,pagesize); // 根据算法找出page数组中应该替换的值的索引
            page[position]=task[taskid]; // 替换值
        }
    }
}
