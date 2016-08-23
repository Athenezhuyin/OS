#ifndef MEMORY_H
#define MEMORY_H


#include <fstream>
//#include <windows.h>
#include <iostream>
#include <cstring>
#include <string>

using namespace std;

struct Operation
{
    int PID;				//进程编号
    int time;				//起始时间
    int block;				//内存页数
    int oper;				//操作
    int protection;			//权限
};

//跟踪每一次分配活动的数据结构--进程
struct trace
{
    int PID;
    int start;				//起始地址
    int block;   			//分配的大小
    int used_block;			//已经使用的块数
    trace() :PID(-1){}
};



//物理地址、虚拟地址对应的结构--每块
struct TraceBlock
{
    int PID;
    int phy;
    int vir;
    int valid;
    TraceBlock() :valid(1),PID(-1){}//初始化
};

//每个文件对应的物理地址和块数--文件
struct FileBlock
{
    int start;
    int PID;
    string filename;
    int block;
    int PhyAddress[100];
    int oper;
};

struct P_File//文件与就进程
{
    FileBlock fileblock[100];
    //int PID;
    int filenum;
    P_File() :filenum(0){}
};


#endif // MEMORY_H
