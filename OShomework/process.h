#ifndef PROCESS_H
#define PROCESS_H



class PCB
{
public:
    PCB(){pId=-1;ttl=-1;cpu=-1;io=-1;}
    void PCB_random_create(int _pId,int type,int _ppId,bool random_ttl);
    void PCB_CPU_create(int _pId,int type,int _ppId,bool random_ttl);
    void PCB_IO_create(int _pId,int type,int _ppId,bool random_ttl);
    void PCB_priority_create(int _pId,int _type,int _ppId,int _priority,bool random_ttl);
    void PCB_dump();

    int pId;//进程号
    int state;//1: ready、 2:running、 3:waiting、 4:terminated
    int p_pID; //父进程号
    int priority;//优先级(1~5)数值大的优先级高
    int type;//进程种类，1表示由按钮创建，2表由其他进程创建
    bool create_child;//是否已创建过子进程
    /*资源列表*/
    int ttl;//总运行时间片数
    int memory_request;//申请占用内存数
    int cpu;//需要占用cpu的时间片数
    int pre_io;
    int io;//需要占用io设备的时间片数

    //打开文件列表

    PCB& operator= (const PCB &p1)
    {
         if (this == &p1)
         {
             return *this;
         }

         pId = p1.pId;
         state = p1.state;
         p_pID = p1.p_pID;
         priority = p1.priority;
         type = p1.type;
         create_child = p1.create_child;

         ttl = p1.ttl;
         memory_request = p1.memory_request;
         cpu = p1.cpu;
         pre_io = p1.pre_io;
         io = p1.io;

        return *this;

    }
};



#endif // PROCESS_H
