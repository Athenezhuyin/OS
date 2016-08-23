//#part1# 负责内存管理信息可视化以及相关UI设计，并负责总体UI设计
//#part2# 负责新增四种可选IO调度策略以及相关UI设计
//#part3# 负责CPU->IO->CPU调度实现（中期版本为CPU->IO）以及相关代码重构
//#part4# 负责文件系统文件创建、删除功能实现以及相关UI设计
//#part5# 负责文件系统文件打开、关闭功能以及目录后退功能实现以及相关UI设计
//#part6# 负责文件系统文件查看、修改功能实现以及文件系统部分调试

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <list>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <QDebug>
#include <stdio.h>

#include "memory.h"
#include "file.h"

using namespace std;

bool isShorter(const PCB& p1,const PCB& p2);
bool isShorter_io(const PCB& p1,const PCB& p2);
bool isHigher(const PCB& p1,const PCB& p2);

void Allocator(Operation op,FileBlock fileblock);  //内存分配
int AllocatorVir(int index);//#part1#
int AllocatorPhy(int index);//#part1#
int AllocatorPID(int index);//#part1#
FileBlock MemoryFileList(int index);
int Create(int pid,string fname,string fpath,int acc,int type,int size);  //创建文件 1111 //#part4#
void Open(int pid,string fname,string fpath);   //打开文件2222 //#part5#
int Write(int pid,string fname,string fpath,int size);   //写文件33333 //#part6#
void Close(int pid,string fname,string fpath);  //关闭文件 4444 //#part5#
void Delete(int pid,string fname,string fpath);   //删除文件55555/ /#part4#
int Search(const char *name, const char *path);   //查找指定文件 //#part6#
void IniFMSys();  //文件系统初始化
void printfminfo();

/*---------------------------------队列---------------------------------*/
list<PCB> ready_list;//就绪队列

list<PCB> waiting_list;//阻塞队列

list<PCB> waiting_cpu_list;//阻塞队列

/*---------------------------------全局变量---------------------------------*/

extern int index_num;
extern int filenum;

int process_sum;//当前进程总数

int process_id;//当前最大进程ID

int process_to_delete;//要删除的进程

int schedule_strategy;//调度策略
int schedule_strategy_io;//调度策略

int priority;//指定进程优先级

int cpu_time;//CPU运行时间

int io_time;//IO运行时间

bool create_child_p;//是否自动创建子进程

bool random_ttl;//是否采用随机时间片长度

char slice[10][10];  //路径分析


PCB cur_PCB;//当前占用cpu的pcb
PCB cur_IO_PCB;//当前占用IO设备的pcb

extern TraceBlock traceblock[32];

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //全局变量初始化

    process_sum=0;//进程总数初始化为0
    process_id=0;//当前最大进程ID初始化为0
    process_to_delete=-1;//待删除进程初始化
    schedule_strategy=0;//初始化调度策略为FCFS
    create_child_p=false;//默认不自动创建子进程
    random_ttl=false;//默认不采用随机时间片长度
    priority=1;
    cpu_time=0;
    io_time=0;

    IniFMSys();  //文件系统初始化

    //ui初始化
    ui->radioButton_off->setChecked(true);//默认不自动创建子进程
    ui->radioButton_fixed->setChecked(true);//默认不采用随机时间片长度


    /*计时器初始化*/
     timer = new QTimer(this);
     connect(timer,SIGNAL(timeout()),this,SLOT(work_routine()));
     timer->start(1000);


     file_count = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}


/*-------------------------工作流程---------------------------------*/
void MainWindow::work_routine()
{
//当前正在服务进程创建子进程

    //qDebug()<<"cur_PCB.type"<<cur_PCB.type <<"cur_PCB.create_child"<<cur_PCB.create_child;
    if(cur_PCB.type==1&&cur_PCB.create_child==false&&cur_PCB.pId!=-1&&create_child_p==true)
    {
        qDebug()<<"child!";

        for(int i=0;i<5;i++)//创建5个子进程
        {
        process_sum++;
        process_id++;

        PCB pcb;
        pcb.PCB_random_create(process_id,2,cur_PCB.pId,random_ttl);
        ready_list.push_back(pcb);
        }
       cur_PCB.create_child=true;
    }

/*CPU*/
    if(cur_PCB.cpu-1>=0&&cur_PCB.pId!=-1)
    {
    cur_PCB.cpu--;//当前占用cpu的进程属性cpu--
    cur_PCB.pre_io--;
    cur_PCB.ttl--;//生命周期减1
    cpu_time++;
    }

/*IO*/
   if(cur_IO_PCB.io-1>=0&&cur_IO_PCB.pId!=-1)
   {
    cur_IO_PCB.io--;//当前占用IO设备的进程属性io--
    cur_IO_PCB.ttl--;//生命周期减1
    io_time++;
   }

/*file management*/
//file_manage();
/*调度*/
   schedule(schedule_strategy, schedule_strategy_io);
/*删除生命周期结束的进程*/
   for(list<PCB>::iterator it = ready_list.begin();it!=ready_list.end();)
   {

       if((*it).ttl==0)
       {
          //memory management,delete
          Operation op;
          FileBlock fileblock;
          op.PID = (*it).pId;
          op.block = (*it).memory_request;
          op.oper = 2;//0create,2delete
          Allocator(op,fileblock);

          it = ready_list.erase(it);
          process_sum--;

       }
       else
       {
          it++;
       }
   }
   for(list<PCB>::iterator it = waiting_list.begin();it!=waiting_list.end();)
   {

       if((*it).ttl==0)
       {
          //memory management,delete
          Operation op;
          FileBlock fileblock;
          op.PID = (*it).pId;
          op.block = (*it).memory_request;
          op.oper = 2;//0create,2delete
          Allocator(op,fileblock);

          it = waiting_list.erase(it);
          process_sum--;

       }
       else
       {
          it++;
       }

    }
/*显示刷新*/
    show_ready_queque();
    show_waiting_queque();
    show_process_sum();
    show_cpu();
    show_io();

    show_cur_dir();
    show_memory_block();
}



/*-------------------------创建进程---------------------------------*/
void MainWindow::on_pushButton_create_process_clicked()//创建随机属性的进程
{
   process_sum++;
   process_id++;

   PCB pcb;
   pcb.PCB_random_create(process_id,1,0,random_ttl);
   ready_list.push_back(pcb);
   
   ofstream out;
   out.open("./process_management.txt",std::ios::app);
   out<<"Create random process:"<<endl;
   out<<"Ready_list:"<<endl;
   for(list<PCB>::iterator iter = ready_list.begin(); iter != ready_list.end(); iter++)
		(*iter).PCB_dump();
    out<<"Waiting list:"<<endl;
	for(list<PCB>::iterator iter = waiting_list.begin(); iter != waiting_list.end(); iter++)
		(*iter).PCB_dump();
    out<<"---------------------------------------------------------------------------------------------------"<<endl;
    out.close();

   //memory management,create
   Operation op;
   FileBlock fileblock;
   op.PID = pcb.pId;
   op.block = pcb.memory_request;
   op.oper = 0;//0create,2delete
   Allocator(op,fileblock);

   show_ready_queque();
   show_process_sum();
}


void MainWindow::on_pushButton_create_process_CPU_clicked()//创建CPU为主的进程
{
    process_sum++;
    process_id++;

    PCB pcb;
    pcb.PCB_CPU_create(process_id,1,0,random_ttl);
    ready_list.push_back(pcb);

	ofstream out;
    out.open("./process_management.txt",std::ios::app);
    out<<"Create CPU process:"<<endl;
    out<<"Ready_list:"<<endl;
    for(list<PCB>::iterator iter = ready_list.begin(); iter != ready_list.end(); iter++)
         (*iter).PCB_dump();
     out<<"Waiting list:"<<endl;
     for(list<PCB>::iterator iter = waiting_list.begin(); iter != waiting_list.end(); iter++)
         (*iter).PCB_dump();
    out.close();


    //memory management,create
    Operation op;
    FileBlock fileblock;
    op.PID = pcb.pId;
    op.block = pcb.memory_request;
    op.oper = 0;//0create,2delete
    Allocator(op,fileblock);
	
    show_ready_queque();
    show_process_sum();
}

void MainWindow::on_pushButton_create_process_IO_clicked()//创建IO为主的进程
{
    process_sum++;
    process_id++;

    PCB pcb;
    pcb.PCB_IO_create(process_id,1,0,random_ttl);
    ready_list.push_back(pcb);

	ofstream out;
    out.open("./process_management.txt",std::ios::app);
    out<<"Create IO process:"<<endl;
    out<<"Ready_list:"<<endl;
    for(list<PCB>::iterator iter = ready_list.begin(); iter != ready_list.end(); iter++)
         (*iter).PCB_dump();
     out<<"Waiting list:"<<endl;
     for(list<PCB>::iterator iter = waiting_list.begin(); iter != waiting_list.end(); iter++)
         (*iter).PCB_dump();
    out.close();

	
    //memory management,create
    Operation op;
    FileBlock fileblock;
    op.PID = pcb.pId;
    op.block = pcb.memory_request;
    op.oper = 0;//0create,2delete
    Allocator(op,fileblock);

    show_ready_queque();
    show_process_sum();
}

void MainWindow::on_pushButton_create_process_p_clicked()//创建指定优先级的进程
{
    QString mess;
    mess=ui->lineEdit_priority->text();
    ui->lineEdit_priority->clear();
    priority=mess.toInt();

    if(priority < 1 || priority >5)
        priority = rand()%5 + 1;

    process_sum++;
    process_id++;

    PCB pcb;
    pcb.PCB_priority_create(process_id,1,0,priority,random_ttl);
    ready_list.push_back(pcb);

	ofstream out;
    out.open("./process_management.txt",std::ios::app);
    out<<"Create specific priority process::"<<endl;
    out<<"Ready_list:"<<endl;
    for(list<PCB>::iterator iter = ready_list.begin(); iter != ready_list.end(); iter++)
         (*iter).PCB_dump();
     out<<"Waiting list:"<<endl;
     for(list<PCB>::iterator iter = waiting_list.begin(); iter != waiting_list.end(); iter++)
         (*iter).PCB_dump();
    out.close();

	

    //memory management,create
    Operation op;
    FileBlock fileblock;
    op.PID = pcb.pId;
    op.block = pcb.memory_request;
    op.oper = 0;//0create,2delete
    Allocator(op,fileblock);

    show_ready_queque();
    show_process_sum();
}

void MainWindow::on_lineEdit_priority_editingFinished()//获取优先级
{
    /*
      QString mess;
      mess=ui->lineEdit_priority->text();
      priority=mess.toInt();
     */
}

/*-------------------------进程调度------------------------------*/

void MainWindow::schedule_fcfs()
{
    if(cur_PCB.pId > 0 && cur_PCB.pre_io == 0 && cur_PCB.cpu >= 0){  //no preemption
        cur_PCB.state = STATE_WAITING;
        waiting_list.push_back(cur_PCB);
        cur_PCB.pId = -1;

		schedule_dump();
    }
    else if(cur_PCB.pId > 0 && cur_PCB.ttl <= 0 && cur_PCB.cpu <= 0){

        cur_PCB.state = STATE_TERMINATED;
        cur_PCB.pId = -1;
        process_sum--;

        schedule_dump();
    }
    else if(cur_PCB.pId < 0 && !ready_list.empty()){
        cur_PCB = ready_list.front();
        cur_PCB.state = STATE_RUNNING;
        ready_list.pop_front();
        schedule_dump();
    }
}

void MainWindow::schedule_np_sjf()
{
    if(cur_PCB.pId > 0 && cur_PCB.pre_io == 0 && cur_PCB.cpu >= 0){  //no preemption
        cur_PCB.state = STATE_WAITING;
        waiting_list.push_back(cur_PCB);
        cur_PCB.pId = -1;

        schedule_dump();
    }
    else if(cur_PCB.pId > 0 && cur_PCB.ttl <= 0 && cur_PCB.cpu <= 0){

        cur_PCB.state = STATE_TERMINATED;
        cur_PCB.pId = -1;
        process_sum--;

        schedule_dump();
    }
    else if(cur_PCB.pId < 0 && !ready_list.empty()){
        ready_list.sort(isShorter);
        cur_PCB = ready_list.front();
        cur_PCB.state = STATE_RUNNING;
        ready_list.pop_front();
        schedule_dump();
    }
}

void MainWindow::schedule_np_priority()
{
    if(cur_PCB.pId > 0 && cur_PCB.pre_io == 0 && cur_PCB.cpu >= 0){  //no preemption
        cur_PCB.state = STATE_WAITING;
        waiting_list.push_back(cur_PCB);
        cur_PCB.pId = -1;

        schedule_dump();
    }
    else if(cur_PCB.pId > 0 && cur_PCB.ttl <= 0 && cur_PCB.cpu <= 0){

        cur_PCB.state = STATE_TERMINATED;
        cur_PCB.pId = -1;
        process_sum--;

        schedule_dump();
    }
    else if(cur_PCB.pId < 0 && !ready_list.empty()){
        ready_list.sort(isHigher);
        cur_PCB = ready_list.front();
        cur_PCB.state = STATE_RUNNING;
        ready_list.pop_front();
        schedule_dump();
    }
}

void MainWindow::schedule_rr()
{
    if(cur_PCB.pId > 0 && cur_PCB.pre_io == 0 && cur_PCB.cpu >= 0){
        cur_PCB.state = STATE_WAITING;
        waiting_list.push_back(cur_PCB);
        cur_PCB.pId = -1;

        schedule_dump();
    }
    else if(cur_PCB.pId > 0 && cur_PCB.ttl <= 0 && cur_PCB.cpu <= 0){

        cur_PCB.state = STATE_TERMINATED;
        cur_PCB.pId = -1;
        process_sum--;
        schedule_dump();
    }

    else   //preemption
    {
        cur_PCB.state = STATE_READY;
        ready_list.push_back(cur_PCB);
		
    }

    if(cur_PCB.pId < 0 && !ready_list.empty())
    {
        cur_PCB = ready_list.front();
        cur_PCB.state = STATE_RUNNING;
        ready_list.pop_front();
		
		schedule_dump();
    }

}


void MainWindow::schedule_io_fcfs()
{
    if(cur_IO_PCB.pId < 0 && !waiting_list.empty()){
        cur_IO_PCB = waiting_list.front();
        waiting_list.pop_front();
		
		schedule_dump();
    }
    else if(cur_IO_PCB.pId >= 0 && cur_IO_PCB.io <= 0){
        if(cur_IO_PCB.cpu != 0){
            cur_IO_PCB.state = STATE_WAITING;
            ready_list.push_back(cur_IO_PCB);
        }
        else{
            cur_IO_PCB.state = STATE_TERMINATED;
            process_sum--;
        }
        cur_IO_PCB.pId = -1;
        schedule_dump();
    }
}


void MainWindow::schedule_io_np_sjf()
{
    if(cur_IO_PCB.pId < 0 && !waiting_list.empty()){
        waiting_list.sort(isShorter_io);
        cur_IO_PCB = waiting_list.front();
        waiting_list.pop_front();

        schedule_dump();
    }
    else if(cur_IO_PCB.pId >= 0 && cur_IO_PCB.io <= 0){
        if(cur_IO_PCB.cpu != 0){
            cur_IO_PCB.state = STATE_WAITING;
            ready_list.push_back(cur_IO_PCB);
        }
        else{
            cur_IO_PCB.state = STATE_TERMINATED;
            process_sum--;
        }
        cur_IO_PCB.pId = -1;
        schedule_dump();
    }
}


void MainWindow::schedule_io_np_priority()
{
    if(cur_IO_PCB.pId < 0 && !waiting_list.empty()){
        waiting_list.sort(isHigher);
        cur_IO_PCB = waiting_list.front();
        waiting_list.pop_front();

        schedule_dump();
    }
    else if(cur_IO_PCB.pId >= 0 && cur_IO_PCB.io <= 0){
        if(cur_IO_PCB.cpu != 0){
            cur_IO_PCB.state = STATE_WAITING;
            ready_list.push_back(cur_IO_PCB);
        }
        else{
            cur_IO_PCB.state = STATE_TERMINATED;
            process_sum--;
        }
        cur_IO_PCB.pId = -1;
        schedule_dump();
    }
}

void MainWindow::schedule_io_rr()
{
    if(cur_IO_PCB.pId >= 0 && cur_IO_PCB.io <= 0){
        if(cur_IO_PCB.cpu != 0){
            cur_IO_PCB.state = STATE_WAITING;
            ready_list.push_back(cur_IO_PCB);
        }
        else{
            cur_IO_PCB.state = STATE_TERMINATED;
            process_sum--;
        }
        cur_IO_PCB.pId = -1;
        schedule_dump();
    }
    else{
        cur_IO_PCB.state = STATE_READY;
        ready_list.push_back(cur_IO_PCB);
    }

    if(cur_IO_PCB.pId < 0 && !waiting_list.empty()){
        cur_IO_PCB = waiting_list.front();
        cur_IO_PCB.state = STATE_RUNNING;
        waiting_list.pop_front();

        schedule_dump();
    }
}


void MainWindow::schedule(int strategy, int strategy_io)
{

    if(strategy == SCHEDULE_FCFS)
        schedule_fcfs();
    else if(strategy == SCHEDULE_NP_SJF)
        schedule_np_sjf();
    else if(strategy == SCHEDULE_NP_PRIORITY)
        schedule_np_priority();
    else if(strategy == SCHEDULE_RR)
        schedule_rr();

    schedule_io_fcfs();
    /*
    if(strategy_io == SCHEDULE_FCFS)
        schedule_io_fcfs();
    else if(strategy_io == SCHEDULE_NP_SJF)
        schedule_io_np_sjf();
    else if(strategy_io == SCHEDULE_NP_PRIORITY)
        schedule_io_np_priority();
    else if(strategy_io == SCHEDULE_RR)
        schedule_io_rr();
    */
}


void MainWindow::schedule_dump()
{
	ofstream out;
    out.open("./process_management.txt",std::ios::app);
	
    out<<"Process management!:"<<endl;
    out<<"CPU serving process:"<<endl;
	cur_PCB.PCB_dump();
    out<<"Ready list:"<<endl;
    for(list<PCB>::iterator iter = ready_list.begin(); iter != ready_list.end(); iter++)
		(*iter).PCB_dump();
		
    out<<"IO serving process:"<<endl;
	cur_IO_PCB.PCB_dump();
    out<<"Waiting list:"<<endl;
	for(list<PCB>::iterator iter = waiting_list.begin(); iter != waiting_list.end(); iter++)
		(*iter).PCB_dump();

    out<<"---------------------------------------------------------------------------------------------------"<<endl;
    out.close();
}


/*-------------------------删除进程------------------------------*/
void MainWindow::on_lineEdit_process_delete_editingFinished()//获取待删除进程id
{
        /*
     QString mess;
     mess=ui->lineEdit_process_delete->text();
     qDebug() << "~~~~~~~~~~~~~~_mess_process_to_delete~~~~~" << mess <<endl;
     process_to_delete=mess.toInt();
     cout << "~~~~~~~~~~~~~~process_to_delete~~~~~" << process_to_delete <<endl;
     //qDebug()<<process_to_delete<<endl;
     */
}

void MainWindow::on_pushButton_delete_prcess_clicked()//删除进程
{
    QString mess;
    mess=ui->lineEdit_process_delete->text();
    //qDebug() << "~~~~~~~~~~~~~~_mess_process_to_delete~~~~~" << mess <<endl;
    process_to_delete=mess.toInt();
    //cout << "~~~~~~~~~~~~~~process_to_delete~~~~~" << process_to_delete <<endl;
    //qDebug()<<process_to_delete<<endl;
    ui->lineEdit_process_delete->clear();

    for(list<PCB>::iterator it = waiting_list.begin();it!=waiting_list.end();){
        //cout << "~~~~~~~~~~~~~~process_to_delete~~~~~" << process_to_delete <<endl;
        if((*it).pId == process_to_delete){
            //memory management,delete
            Operation op;
            FileBlock fileblock;
            op.PID = (*it).pId;
            op.block = (*it).memory_request;
            op.oper = 2;//0create,2delete
            Allocator(op,fileblock);
            it = waiting_list.erase(it);
            process_sum--;
        }
        else{
           it++;
        }
    }

    for(list<PCB>::iterator it = ready_list.begin();it!=ready_list.end();){
        if((*it).pId==process_to_delete){
            //memory management,delete
            Operation op;
            FileBlock fileblock;
            op.PID = (*it).pId;
            op.block = (*it).memory_request;
            op.oper = 2;//0create,2delete
            Allocator(op,fileblock);
            it = ready_list.erase(it);
            process_sum--;
        }
        else{
           it++;
        }
    }

    //删除正在服务的PCB
    if(process_to_delete == cur_PCB.pId)
    {
        cur_PCB.pId = -1;
        cur_PCB.cpu=0;
        cur_PCB.ttl=0;
        cur_PCB.io=0;
    }
    if(process_to_delete == cur_IO_PCB.pId)
    {
        cur_IO_PCB.pId = -1;
        cur_IO_PCB.cpu=0;
        cur_IO_PCB.ttl=0;
        cur_IO_PCB.io=0;
    }
    /*显示刷新*/
        show_ready_queque();
        show_waiting_queque();
        show_process_sum();
}

void MainWindow::on_pushButton_clicked()//restart
{
    //从队列中删除
    for(list<PCB>::iterator it = ready_list.begin();it!=ready_list.end();){

        it = ready_list.erase(it);

    }
    for(list<PCB>::iterator it = waiting_list.begin();it!=waiting_list.end();){

        it = waiting_list.erase(it);


    }

    //删除正在服务的PCB

    cur_PCB.pId=-1;

    cur_IO_PCB.pId=-1;


    process_sum=0;//当前进程总数

    process_id=0;//当前最大进程ID

    process_to_delete=-1;//要删除的进程

    cpu_time=0;//CPU运行时间

    io_time=0;//IO运行时间



    /*显示刷新*/
    show_ready_queque();
    show_waiting_queque();
    show_process_sum();
    ui->label_iotime->setText("0");
    ui->label_cputime->setText("0");


}
/*-------------------------显示刷新函数------------------------------*/
void MainWindow::show_ready_queque()
{
    QString mess;
    ui->textBrowser_ready_queue->clear();

    for(list<PCB>::iterator it = ready_list.begin();it!=ready_list.end();it++){

        mess=QString("[进程%1] p:%2 ttl:%3 cpu:%4 io:%5").arg((*it).pId).arg((*it).priority).arg((*it).ttl).arg((*it).cpu).arg((*it).io);
        ui->textBrowser_ready_queue->append(mess);
         }
}
void MainWindow::show_waiting_queque()
{
    QString mess;
    ui->textBrowser_waiting_queue->clear();

    for(list<PCB>::iterator it = waiting_list.begin();it!=waiting_list.end();it++){

        mess=QString("[进程%1] p:%2 ttl:%3 cpu:%4 io:%5").arg((*it).pId).arg((*it).priority).arg((*it).ttl).arg((*it).cpu).arg((*it).io);
        ui->textBrowser_waiting_queue->append(mess);
    }
}
void MainWindow::show_memory_block()
{
    QString mess;
    ui->textBrowser_memory->clear();
    int memory_pid, memory_vir, memory_phy, temp_pid = -1;

    /*
    memory_pid = AllocatorPID(index_num);
    memory_vir = AllocatorVir(index_num);
    memory_phy = AllocatorPhy(index_num);
    //输出每个块的虚拟地址、物理地址和对应的所属进程ID
    //mess=QString("[virtual block %1]\n   Process ID: %2\n   Virtual Address: %3\n   Physical Address: %4 io:%5").arg(i).arg(traceblock[i].PID).arg(traceblock[i].vir).arg(traceblock[i].phy);
    if(memory_pid != -1 && memory_pid != temp_pid){
        mess=QString("[TraceArray ID %1]\n   Process ID: %2\n   Starting Address: %3\n   Used Block: %4\n").arg(index_num).arg(memory_pid).arg(memory_vir).arg(memory_phy);
        ui->textBrowser_memory->append(mess);
    }
    */


    for (int i = 0; i <= index_num ; i++){
        memory_pid = AllocatorPID(i);
        memory_vir = AllocatorVir(i);
        memory_phy = AllocatorPhy(i);
        //输出每个块的虚拟地址、物理地址和对应的所属进程ID
        //mess=QString("[virtual block %1]\n   Process ID: %2\n   Virtual Address: %3\n   Physical Address: %4 io:%5").arg(i).arg(traceblock[i].PID).arg(traceblock[i].vir).arg(traceblock[i].phy);
        //if(memory_pid != -1 && memory_pid != temp_pid)
        if(memory_pid != -1){
            mess=QString("[TraceArray ID %1]\n   Process ID: %2\n   Starting Address: %3\n   Used Block: %4\n").arg(i).arg(memory_pid).arg(memory_vir).arg(memory_phy);
            ui->textBrowser_memory->append(mess);
        }
        //temp_pid = memory_pid;
    }

    /*
    for (int i = 0; i <= filenum ; i++){
        FileBlock temp_file_block = MemoryFileList(i);
        //输出每个块的虚拟地址、物理地址和对应的所属进程ID
        //mess=QString("[virtual block %1]\n   Process ID: %2\n   Virtual Address: %3\n   Physical Address: %4 io:%5").arg(i).arg(traceblock[i].PID).arg(traceblock[i].vir).arg(traceblock[i].phy);
        //if(memory_pid != -1 && memory_pid != temp_pid)
        if(memory_pid != -1){
            mess=QString("File Name %1\n   Starting Address: %2\n   Used Block: %3\n Process ID:%4\n").arg(temp_file_block.filename.data()).arg(temp_file_block.start).arg(temp_file_block.block).arg(temp_file_block.PID);
            ui->textBrowser_memory->append(mess);
        }
        //temp_pid = memory_pid;
    }
    */
}
void MainWindow::show_process_sum()
{
    QString mess;
    mess=QString("%1").arg(process_sum);
    ui->label_p_sum->setText(mess);
}

void MainWindow::show_cpu()
{
    ui->textBrowser_CPU->clear();
    if(cur_PCB.pId!=-1)
    {
    QString mess;
    mess=QString("[进程%1] p:%2 ttl:%3 cpu:%4 io:%5").arg(cur_PCB.pId).arg(cur_PCB.priority).arg(cur_PCB.ttl).arg(cur_PCB.cpu).arg(cur_PCB.io);

    ui->textBrowser_CPU->setText(mess);
    }

    QString mess;
    mess=QString("%1").arg(cpu_time);
    ui->label_cputime->setText(mess);
}

void MainWindow::show_io()
{
    ui->textBrowser_IO->clear();
    if(cur_IO_PCB.pId!=-1)
    {
    QString mess;
    mess=QString("[进程%1] p:%2 ttl:%3 cpu:%4 io:%5").arg(cur_IO_PCB.pId).arg(cur_IO_PCB.priority).arg(cur_IO_PCB.ttl).arg(cur_IO_PCB.cpu).arg(cur_IO_PCB.io);

    ui->textBrowser_IO->setText(mess);
    }

    QString mess;
    mess=QString("%1").arg(io_time);
    ui->label_iotime->setText(mess);
}

/*-------------------------策略选择------------------------------*/
void MainWindow::on_comboBox_currentIndexChanged(int index)//CPU策略选择
{
    /*
   switch (index) {
    case 1://SCHEDULE_FCFS

        break;
    case 2://SCHEDULE_NP_SJF

        break;
    case 3://SCHEDULE_NP_PRIORITY

        break;
    case 4://SCHEDULE_RR

        break;

    default:
        break;
    }
*/
    schedule_strategy=index;
}

void MainWindow::on_comboBox_io_currentIndexChanged(int index)//IO策略选择
{
    schedule_strategy_io = index;
}



void MainWindow::on_radioButton_on_clicked()//自动创建子进程开启
{
    create_child_p=true;
}

void MainWindow::on_radioButton_off_clicked()//自动创建子进程关闭
{
    create_child_p=false;
}

void MainWindow::on_radioButton_fixed_clicked()//定长时间片
{
    random_ttl=false;
}

void MainWindow::on_radioButton_random_clicked()//变长时间片
{
    random_ttl=true;
}

void MainWindow::show_cur_dir()//显示当先目录下所有文件以及目录地址
{
    QString mess;
    ui->textBrowser_file->clear();

    //cout << "show~~~~~" << endl;
    mess=QString("当前路径: %1\n").arg(CS.currentpath);
    ui->textBrowser_file->setText(mess);


    if(strcmp(CS.currentpath,"/") == 0){
        //cout << "show1~~~~" <<FI.indexcount<<endl;
        /*
        for(int i = 0; i < FI.index[0].fcb->childnum; i++){

            if(FI.index[i + 1].fcb->feffect != 0){
                if(FI.index[i + 1].fcb->ftype == 1)
                    mess=QString("%1 dir \n").arg(FI.index[i + 1].fcb->fname);
                else
                    mess=QString("%1 file \n").arg(FI.index[i + 1].fcb->fname);
                ui->textBrowser_file->append(mess);
            }

        }
        */
        for(int i = 1; i < FI.indexcount && (FI.index[i].filelevel == CS.filelevel); i++){
            //cout << "show1.1~~~~" <<FI.index[i].fcb->feffect<<endl;
            if(FI.index[i].fcb->feffect != 0){
                /*
                cout << "show1.2~~~~" <<FI.index[i].fcb->ftype<<endl;
                cout << "show1.3~~~~" <<FI.index[i].filelevel<<endl;
                cout << "show1.4~~~~" <<CS.filelevel<<endl;
                cout << "show1.5~~~~" <<FI.index[i].fcb->fname<<endl;
                */
                if(FI.index[i].fcb->ftype == 1)
                    mess=QString("%1 dir \n").arg(FI.index[i].fcb->fname);
                else if(FI.index[i].fcb->ftype == 0)
                    mess=QString("%1 file \n").arg(FI.index[i].fcb->fname);

                ui->textBrowser_file->append(mess);
            }

        }
    }
    else{
        for(int i = 0; i < FI.indexcount; i++){
            /*
            qDebug()<<"show~~~~~~~!!!!!!!" << CS.currparent->fname;
            qDebug()<<"show~~~~~~~!!!!!!!" << FI.index[i].filename << "FI~~~~~" << FI.index[i].filelevel;
            */
            if(FI.index[i].fcb->feffect != 0 && (strcmp(FI.index[i].parentfilename,CS.currparent->fname) == 0) && (FI.index[i].filelevel == CS.filelevel)){

                if(FI.index[i].fcb->ftype == 1)
                    mess=QString("%1 dir \n").arg(FI.index[i].fcb->fname);
                else
                    mess=QString("%1 file \n").arg(FI.index[i].fcb->fname);
                ui->textBrowser_file->append(mess);
            }
        }
    }

}

void MainWindow::on_pushButton_back_clicked()//返回上一级目录
{
    string back_path(CS.currentpath);

    //qDebug()<<"back1~~~~~~~!!!!!!!" << CS.currparent->fname;
    CState CS_temp = CS;
    //qDebug()<<"back1.1~~~~~~~!!!!!!!" << CS.currparent->fname;
    /*
    strcpy(CS_temp.currentpath, CS.currentpath);
    qDebug()<<"back1.2~~~~~~~!!!!!!!" << CS.currparent->fname;
    CS_temp.currparent = CS.currparent;
    CS_temp.filelevel = CS.filelevel;
    */

    int back_flag = 1;

    if(strcmp(CS.currentpath,"/") != 0){
        char *path;
        strcpy(path, back_path.data());
        int short_pos = 0;
        for(int i = strlen(path) - 1; i >= 0; i--){
            //qDebug()<<"back1.7~~~~~~~!!!!!!!" << path[i];
            if(path[i] == '/' && i != (strlen(path) - 1)){
                short_pos = i;
                break;
            }
        }
        //qDebug()<<"back1~~~~~~~!!!!!!!" << CS.currparent->fname;
        back_path = back_path.substr(0, short_pos + 1);
        //qDebug()<<"back1.6~~~~~~~!!!!!!!" << CS.currparent->fname;
        strcpy(CS.currentpath, back_path.data());
        //qDebug()<<"back1.6.1~~~~~~~!!!!!!!" << CS.currparent->fname;
        CS.filelevel--;
        CS.currparent->fstate = closed;

        if(strcmp(CS.currentpath,"/") == 0){
            //qDebug()<<"back1.6.2~~~~~~~!!!!!!!" << CS.currparent->fname;
            CS.currparent = FI.index[0].fcb;
            //qDebug()<<"back1.6.3~~~~~~~!!!!!!!" << CS.currparent->fname;
         }

        else{
            strcpy(path, back_path.data());
            int path_index = 1, file_level = 1, cur_dir = 0, dir_index;

            //char slice[10][10];  //用来存放分解路径

            memset(slice,'\0',10);
            strcpy(slice[0],"root");
            while(path[path_index] != '\0') {  //将路径分解，存入slice数组中
                //qDebug()<<"back1.4~~~~~~~!!!!!!!" << CS.currparent->fname;
                dir_index = 0;
                while (path[path_index] != '/' && path[path_index] != '\0'){
                    //qDebug()<<"back1.5~~~~~~~!!!!!!!" << CS.currparent->fname;
                    slice[file_level][dir_index] = path[path_index];
                    dir_index++;
                    path_index++;
                }
                slice[file_level][dir_index] = '\0';

                if(path[path_index] == '\0')
                    break;
                else{
                    path_index++;
                    file_level++;   //用j可以表示路径的层级
                }
            }

            file_level--;
            /*
        qDebug()<<"path:"<<j<<endl;
        for(int k=0;k<=j;k++)
           qDebug()<<k<<":"<<slice[k]<<endl;
        */

            //qDebug()<<"back1.3~~~~~~~!!!!!!!" << CS.currparent->fname;
            int fi_index;

            for(int i = 0; i <= file_level; i++){
                for(int j = cur_dir; j < FI.indexcount; j++){
                    //cout << "search~~~!" << FI.index[i].filelevel <<endl;
                    if((strcmp(FI.index[j].filename,slice[i])==0)&& (FI.index[j].filelevel == i) && (FI.index[j].filetype == dir) && (FI.index[j].effect== 1))
                    {
                        if(i!= 0)
                        {
                            if(strcmp(FI.index[j].parentfilename,slice[i-1])==0)  //每找到一个跳出小循环，继续下一个层级的寻找
                            {
                                cur_dir = j;
                                fi_index = j;
                                break;
                            }
                        }
                        else
                            break;
                    }
                }
                if(fi_index == FI.indexcount) back_flag = 0;
            }

            //qDebug()<<"index["<<i<<"].filename:"<<endl;
            CS.currparent = FI.index[fi_index].fcb;

        }
    }

    if(back_flag){
        //cout<<"back1.9~~~~~~~!!!!!!!" << CS.filelevel;
        show_cur_dir();
    }
    else{
        CS = CS_temp;
        CS.currparent->fstate = opened;
        /*
        strcpy(CS.currentpath, CS_temp.currentpath);
        CS.currparent = CS_temp.currparent;
        CS.filelevel = CS_temp.filelevel;
        */
    }

}

void MainWindow::on_pushButton_file_open_clicked()//打开文件操作相关进程
{
    int file_process_num = 200, slg;//规定进程号200起始为打开文件

    QString mess;
    mess=ui->lineEdut_file_display->text();
    ui->lineEdut_file_display->clear();
    QByteArray b_mess = mess.toLatin1();
    char *mess_text = b_mess.data();
    string open_file_name(mess_text);
    string open_current_path(CS.currentpath);

    slg=Search(open_file_name.c_str(),open_current_path.c_str());
    //cout << "open~~~"<< slg << "~~~~slg~~~~~" << endl;
    if(slg > 0 && FI.index[slg].fcb->feffect != 0){
        Open(file_process_num + process_id + 1, open_file_name, open_current_path);
        file_use_process(file_process_num);

        if(FI.index[slg].fcb->ftype == dir){
            strcat(CS.currentpath,open_file_name.c_str());
            strcat(CS.currentpath,"/");
            CS.filelevel++;
            CS.currparent = FI.index[slg].fcb;
        }
    }

    //cout << CS.currentpath << "~~~~~" << endl;
    show_cur_dir();
}

void MainWindow::on_pushButton_file_close_clicked()//关闭文件操作相关进程
{
    int file_process_num = 500, slg;//规定进程号500起始为打开文件

    QString mess;
    mess=ui->lineEdut_file_display->text();
    ui->lineEdut_file_display->clear();
    QByteArray b_mess = mess.toLatin1();
    char *mess_text = b_mess.data();
    string open_file_name(mess_text);
    string open_current_path(CS.currentpath);

    slg=Search(open_file_name.c_str(),open_current_path.c_str());
    if(slg > 0 && FI.index[slg].fcb->feffect != 0 && FI.index[slg].fcb->ftype == file){
        cout << "close1~~~"<< slg << "~~~~slg~~~~~" << endl;
        Close(file_process_num + process_id + 1, open_file_name, open_current_path);
        file_use_process(file_process_num);
    }

    cout << CS.currentpath << "  close~~~~~" << endl;
    show_cur_dir();
}

void MainWindow::on_pushButton_file_look_clicked()//查看文件信息操作相关进程
{
    int file_process_num = 400, slg;//规定进程号400起始为查看文件

    QString mess;
    mess=ui->lineEdut_file_display->text();
    ui->lineEdut_file_display->clear();
    ui->textBrowser_file_info->clear();
    QByteArray b_mess = mess.toLatin1();
    char *mess_text = b_mess.data();
    string open_file_name(mess_text);
    string open_current_path(CS.currentpath);

    slg=Search(open_file_name.c_str(),open_current_path.c_str());
    if(slg > 0 && FI.index[slg].fcb->feffect != 0){

        mess=QString("%1\n%2").arg(FI.index[slg].fcb->fname).arg(FI.index[slg].fcb->ftype);
        ui->textBrowser_file_info->append(mess);

        if(FI.index[slg].fcb->ftype == file)
            mess=QString("file");
        else if(FI.index[slg].fcb->ftype == dir)
            mess=QString("dir");
        else
            mess=QString("error!");
        ui->textBrowser_file_info->append(mess);

        if(FI.index[slg].fcb->fstate == closed)
            mess=QString("closed");
        else if(FI.index[slg].fcb->fstate == opened)
            mess=QString("opened");
        else if(FI.index[slg].fcb->fstate == writing)
            mess=QString("writing");
        else
            mess=QString("error!");
        ui->textBrowser_file_info->append(mess);

        mess=QString("len: %1").arg(FI.index[slg].fcb->flen);
        ui->textBrowser_file_info->append(mess);

        if(FI.index[slg].fcb->facc == readonly)
            mess=QString("readonly");
        else if(FI.index[slg].fcb->facc == readwrite)
            mess=QString("readwrite");
        else
            mess=QString("error!");
        ui->textBrowser_file_info->append(mess);

        mess=QString("childnum: %1\nlast modified\n%2\ncreate\n%3\n").arg(FI.index[slg].fcb->childnum).arg(FI.index[slg].fcb->lastmodtime).arg(FI.index[slg].fcb->createtime);
        ui->textBrowser_file_info->append(mess);

        file_use_process(file_process_num);

    }

    show_cur_dir();
}


void MainWindow::on_pushButton_file_create_clicked()
{
    int file_size = 100;
    int file_process_num = 100;//规定进程号100起始为创建文件

    QString mess;
    mess=ui->lineEdut_file_display->text();
    ui->lineEdut_file_display->clear();
    QByteArray b_mess = mess.toLatin1();
    char *mess_text = b_mess.data();
    string create_file_text(mess_text);

    string create_file_name, create_file_dir, create_file_type, create_file_other;
    string create_file_path(CS.currentpath);
    istringstream find_space(create_file_text);
    find_space >> create_file_name >> create_file_dir >> create_file_type >> create_file_other;

    //qDebug() << create_file_path.data() << "~~~~~~~~~";

    int create_file_flag = 1;
    if(strcmp(create_file_dir.data(), "dir") == 0)
        create_file_flag = Create(file_process_num + process_id + 1, create_file_name, create_file_path, readwrite, dir, file_size);
    else if(strcmp(create_file_dir.data(), "file") == 0){
        if(strcmp(create_file_type.data(), "readonly") == 0)
            create_file_flag = Create(file_process_num + process_id + 1, create_file_name, create_file_path, readonly, file, file_size);
        else
            create_file_flag = Create(file_process_num + process_id + 1, create_file_name, create_file_path, readwrite, file, file_size);
    }
    else create_file_flag = 0;

    if(create_file_flag) file_use_process(file_process_num);

    show_cur_dir();
}

void MainWindow::on_pushButton_file_write_clicked()//写文件
{
    int file_process_num = 600, slg;//规定进程号600起始为写文件

    QString mess;
    mess=ui->lineEdut_file_display->text();
    ui->lineEdut_file_display->clear();
    QByteArray b_mess = mess.toLatin1();
    char *mess_text = b_mess.data();
    string write_file_text(mess_text);

    string write_file_name, write_file_size, write_file_other;
    string write_file_path(CS.currentpath);
    istringstream find_space(write_file_text);
    find_space >> write_file_name >> write_file_size >> write_file_other;

    stringstream change_type;
    int write_file_size_int = 0;
    change_type << write_file_size;
    change_type >> write_file_size_int;

    //qDebug() << create_file_path.data() << "~~~~~~~~~";

    int write_file_flag = 1;
    slg=Search(write_file_name.c_str(),write_file_path.c_str());
    if(slg > 0 && write_file_size_int > 0){
        qDebug() << slg << "write~~~~~~~~~"<< write_file_size_int;
        write_file_flag = Write(file_process_num + process_id + 1,write_file_name,write_file_path,write_file_size_int);

    }
    else write_file_flag = 0;

    if(write_file_flag) file_use_process(file_process_num);

    show_cur_dir();
}


void MainWindow::on_pushButton_file_delete_clicked()
{
    int file_process_num = 300, slg;//规定进程号300起始为删除文件

    QString mess;
    mess=ui->lineEdut_file_display->text();
    ui->lineEdut_file_display->clear();
    QByteArray b_mess = mess.toLatin1();
    char *mess_text = b_mess.data();
    string delete_file_name(mess_text);

    string delete_file_path(CS.currentpath);

    slg=Search(delete_file_name.c_str(),delete_file_path.c_str());
    cout << slg << "slg~~~~~delete" << endl;
    if(slg > 0){
        Delete(file_process_num + process_id + 1, delete_file_name, delete_file_path);
        file_use_process(file_process_num);

    }

    show_cur_dir();
}

void MainWindow:: file_use_process(int file_process_num)//创建文件操作相关进程
{
    process_sum++;
    process_id++;

    PCB pcb;
    process_id += file_process_num;
    pcb.PCB_random_create(process_id,1,0,random_ttl);
    process_id -= file_process_num;
    ready_list.push_back(pcb);

    ofstream out;
    out.open("./process_management.txt",std::ios::app);
    out<<"Create random process:"<<endl;
    out<<"Ready_list:"<<endl;
    for(list<PCB>::iterator iter = ready_list.begin(); iter != ready_list.end(); iter++)
         (*iter).PCB_dump();
     out<<"Waiting list:"<<endl;
     for(list<PCB>::iterator iter = waiting_list.begin(); iter != waiting_list.end(); iter++)
         (*iter).PCB_dump();
     out<<"---------------------------------------------------------------------------------------------------"<<endl;
     out.close();

    //memory management,create
    Operation op;
    FileBlock fileblock;
    op.PID = pcb.pId;
    op.block = pcb.memory_request;
    op.oper = 0;//0create,2delete
    Allocator(op,fileblock);

    show_ready_queque();
    show_process_sum();
}


void MainWindow::file_manage()
{
    QString mess;

    qDebug()<<"当前路径11111:"<<CS.currentpath;
    //mess=QString("当前路径: %1\n").(CS.currentpath);
    //ui->textBrowser_file->setText(mess);

    mess=QString("111");
    //ui->textBrowser_file->append(mess);
    ui->textBrowser_file->setText(mess);

    /*
    qDebug()<<"当前路径:"<<CS.currentpath;
    textBrowser_file
    ui->textBrowser_IO->setText(mess);
    mess=QString("111");
    ui->textBrowser_ready_queue->append(mess);
    */
}





