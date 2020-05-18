#ifndef FIFO_H
#define FIFO_H


class fifo
{
public:
    fifo();
    bool exist(int a,int page[],int pageIndx);
    void work(int a,int page [],int pagesize,int & pageIndx,int & lackpage);
};

#endif // FIFO_H
