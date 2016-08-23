#include "process.h"
#include <time.h>
#include <stdlib.h>
//#include <QDebug>
#include <fstream>
#include <QDebug>

using namespace std;
/*-----------------------创建PCB-------------------------------*/
///type为PCB类型
//type=1 按钮创建的进程，具有创建其他进程的能力
//type=2 由type1进程创建的进程,不具备创建子进程能力

void PCB::PCB_random_create(int _pId,int _type,int _ppId,bool random_ttl)
{

    pId=_pId;//进程号

    state=1;//1: ready、 2:running、 3:waiting、 4:terminated

    if(random_ttl==true)
    {
        srand(time(NULL));//产生随机数
        ttl=rand()%20+3;//总运行时间片数(随机生成1~20)
    }
    else
    {
        ttl=rand()%10+3;
    }

    srand(time(NULL));//产生随机数
    memory_request=rand()%5+1;//申请占用内存数(随机生成1~5)

    srand(time(NULL));//产生随机数
    priority=rand()%5+1;//优先级

    type=_type;
    create_child=false;

    cpu=rand()%(ttl-1) + 1;//需要占用cpu的时间片数
    pre_io = rand()%cpu + 1;
    io=ttl-cpu;//需要占用io设备的时间片数

    p_pID=_ppId; //父进程号,取决于创建方式。按钮创建为0，其他进程创建则为父进程Id

    //打开文件列表



    qDebug()<<"pId:"<<pId<<",state:"<<state<<",ttl:"<<ttl<<",memory_request:"<<memory_request<<",priority:"<<priority<<",cpu:"<<cpu<<",io:"<<io<<",p_pID:"<<p_pID<<",create_child"<<create_child<<endl;
}



void PCB::PCB_CPU_create(int _pId,int _type,int _ppId,bool random_ttl)
{

    pId=_pId;//进程号

    state=1;//1: ready、 2:running、 3:waiting、 4:terminated

    if(random_ttl==true)
    {
        srand(time(NULL));//产生随机数
        ttl=rand()%20+3;//总运行时间片数(随机生成1~20)
    }
    else
    {
        ttl=rand()%10+3;
    }

    srand(time(NULL));//产生随机数
    memory_request=rand()%5+1;//申请占用内存数(随机生成1~5)

    srand(time(NULL));//产生随机数
    priority=rand()%5+1;//优先级

    type=_type;
    create_child=false;

    cpu=ttl-1;//需要占用cpu的时间片数()
    pre_io = rand()%(cpu - 1) + 1;
    io=1;//需要占用io设备的时间片数

    p_pID=_ppId; //父进程号,取决于创建方式。按钮创建为0，其他进程创建则为父进程Id

    //打开文件列表



    qDebug()<<"pId:"<<pId<<",state:"<<state<<",ttl:"<<ttl<<",memory_request:"<<memory_request<<",priority:"<<priority<<",cpu:"<<cpu<<",io:"<<io<<"p_pID:"<<p_pID<<endl;
}

void PCB::PCB_IO_create(int _pId,int _type,int _ppId,bool random_ttl)
{

    pId=_pId;//进程号

    state=1;//1: ready、 2:running、 3:waiting、 4:terminated

    srand(time(NULL));//产生随机数

    if(random_ttl==true)
    {
        srand(time(NULL));//产生随机数
        ttl=rand()%20+2;//总运行时间片数(随机生成1~20)
    }
    else
    {
        ttl=rand()%10+3;
    }

    srand(time(NULL));//产生随机数
    memory_request=rand()%3+3;//申请占用内存数(随机生成3~5)

    srand(time(NULL));//产生随机数
    priority=rand()%5+1;//优先级

    type=_type;
    create_child=false;

    cpu = 2;//需要占用cpu的时间片数()
    pre_io = 1;
    io=ttl-2;//需要占用io设备的时间片数

    p_pID=_ppId; //父进程号,取决于创建方式。按钮创建为0，其他进程创建则为父进程Id

    //打开文件列表

    qDebug()<<"pId:"<<pId<<",state:"<<state<<",ttl:"<<ttl<<",memory_request:"<<memory_request<<",priority:"<<priority<<",cpu:"<<cpu<<",io:"<<io<<"p_pID:"<<p_pID<<endl;
}

void PCB::PCB_priority_create(int _pId,int _type,int _ppId,int _priority,bool random_ttl)
{



    pId=_pId;//进程号

    state=1;//1: ready、 2:running、 3:waiting、 4:terminated

    if(random_ttl==true)
    {
        srand(time(NULL));//产生随机数
        ttl=rand()%20+3;//总运行时间片数(随机生成1~20)
    }
    else
    {
        ttl=rand()%10+3;
    }

    srand(time(NULL));//产生随机数
    memory_request=rand()%5+1;//申请占用内存数(随机生成1~5)


    priority=_priority;//优先级

    type=_type;
    create_child=false;

    cpu=rand()%(ttl - 1) + 1;//需要占用cpu的时间片数
    pre_io = rand()%cpu + 1;
    io=ttl-cpu;//需要占用io设备的时间片数

    p_pID=_ppId; //父进程号,取决于创建方式。按钮创建为0，其他进程创建则为父进程Id

    //打开文件列表



    qDebug()<<"pId:"<<pId<<",state:"<<state<<",ttl:"<<ttl<<",memory_request:"<<memory_request<<",priority:"<<priority<<",cpu:"<<cpu<<",io:"<<io<<"p_pID:"<<p_pID<<endl;
}

void PCB::PCB_dump()
{
    ofstream out;
    out.open("./process_management.txt",std::ios::app);

    out<<"PID: "<<pId<<", state:"<<state<<", pPID: "<<p_pID<<", priority:"<<priority<<", type:"<<type<<", child flag:"<<create_child
            <<", ttl:"<<ttl<<", memory_request:"<<memory_request<<", cpu time:"<<cpu<<", io time:"<<io<<endl;

    out.close();
}



bool isShorter(const PCB& p1,const PCB& p2)
{
    return p2.cpu > p1.cpu;
}

bool isShorter_io(const PCB& p1,const PCB& p2)
{
    return p2.io > p1.io;
}

bool isHigher(const PCB& p1,const PCB& p2)
{
    return p1.priority > p2.priority;
}

