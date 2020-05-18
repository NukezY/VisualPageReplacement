#ifndef LRU_H
#define LRU_H


class lru
{
public:
    lru();
    bool exist(int a,int page[],int pageIndx);
    void work(int a,int page [],int pagesize,int & pageIndx,int & lackpage);
};

#endif // LRU_H
