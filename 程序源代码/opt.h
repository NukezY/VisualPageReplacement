#ifndef OPT_H
#define OPT_H


class opt
{
public:
    opt();
    int exist(int a,int page[],int startid,int endid);
    int choose(int taskid , int page [] ,int task [],int tasksize,int pagesize );
    void work(int task[],int taskid,int page [],int tasksize,int pagesize,int & pageIndx,int & lackpage);
};

#endif // OPT_H
