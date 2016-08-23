////////////////////////////////////////////////////////////////////////////////////////////////////////
//memory-op.cpp
//内存管理实习
//将程序从文件读入每次的操作，并将结果输入到out.txt文件中
////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <fstream>
//#include <windows.h>
#include <iostream>
#include <cstring>
#include <string>

#include "memory.h"
using namespace std;

const int MAX_P = 200;				//维护的进程最大数目
const int MAX_FILE = 100;          //
trace traceArray[MAX_P];
TraceBlock traceblock[128];


const int PageSize = 1024;			//每一页的大小
P_File p_file[MAX_P];				//一共的进程数
FileBlock file_list[100];

//计数变量
int filenum = 0;
int blocknum = 0;
int index_num = -1;
int totalviradd = 0;
int process_temp_id[200];

/**************Tracker——每一个进程执行之后，均对页表进行一次维护*************/
/**************跟踪allocator线程的内存行为，并输出必要信息*********************/

/*void Tracker()
{
    //输出文件
    ofstream outfile;
    outfile.open("out.txt");

    cout<<endl<<endl;
    cout<<"*******************************Maintain*********************************************"<<endl;

    for (int i = 0; i<32; i++)
    {
        //输出每个块的虚拟地址、物理地址和对应的所属进程ID
        outfile << endl << "************************************************************************************" << endl;
        outfile << i << "th virtual block:" << endl;
        outfile << "\t" << "Process:" << traceblock[i].PID << endl;
        outfile << "\t" << "Virtual Address: " << traceblock[i].vir << endl;
        outfile << "\t" << "Physical Address: " << traceblock[i].phy << endl;
    }
    return ;
}


/*****************************模拟内存分配活动的线程***************************/

int AllocatorPID(int index)
{
    if(index < 200 && index >= 0)
        return traceArray[index].PID;
    else
        return -1;
}

int AllocatorVir(int index)
{
    if(index < 200 && index >= 0)
        return traceArray[index].start;
    else
        return -1;
}

int AllocatorPhy(int index)
{
    if(index < 200 && index >= 0)
        return traceArray[index].block;
    else
        return -1;
}

FileBlock MemoryFileList(int index)
{
    return file_list[index];
}

void Allocator(Operation op,FileBlock fileblock) //模拟内存分配活动的线程
{
    ofstream outfile;
    outfile.open("out.txt",std::ios::app);//输出日志


    //DWORD
    unsigned long temp;

                            //执行时间，如果想在指定时间执行可以取消注释
        /*
        switch (op.protection) //根据文件内容确定权限
        {
            case 0:
            {
                //  index = -1;
                  temp = PAGE_READONLY;
                  break;
            }
            case 1:
                temp = PAGE_READWRITE;
            break;
            case 2:
                temp = PAGE_EXECUTE;
            break;
            case 3:
                temp = PAGE_EXECUTE_READ;
            break;
            case 4:
                temp = PAGE_EXECUTE_READWRITE;
            break;
            default:
                temp = PAGE_READONLY;
                break;
        }
        */

    //为进程分配内存
    switch (op.oper)
    {
    case 0://保留一个区域，创建进程时
    {
        //记录进程的数组tracearray赋值
        index_num++;
        traceArray[index_num].PID = op.PID;
        traceArray[index_num].block = op.block;
        traceArray[index_num].used_block = 0;
        int pro_block=0;
        int pro_valid=0;
        int pro_begin=0;
        //如果进程需要的块数小于用户区总大小
        if (traceArray[index_num].block <= 32)
        {

            for (pro_begin=0; pro_begin<128; pro_begin++)
            {
                //当valid为1的时候，找到合适的块数，即为首次适应法
                if (traceblock[pro_begin].PID == -1)
                {
                    for (pro_block = pro_begin; (pro_block < pro_begin + traceArray[index_num].block)
                         &&(traceblock[pro_block].PID == -1); pro_block++);
                    if (traceblock[pro_block-1].PID == -1)
                    {
                        pro_valid = 1;
                        break;
                    }
                }

            }
            if(pro_valid==0)
                return;
            else
            {

                /*	//用户区域起始地址,当index为0时，进程的起始地址为0 ，第一个进程被创建
                if (index == 0)
                {
                    traceArray[0].start = 0; 				   //起始虚拟地址为0
                    totalviradd = traceArray[0].start + traceArray[0].block; //totalviradd虚拟地址的下一块
                }
           */
                //第一个以后的进程被创建，并且进程占的总虚拟空间比用户区总大小要小，当index为其他值时，进程的起始地址为上一个进程的起始地址加上块数
                /*	else if (totalviradd + traceArray[index].block-1<=32)
                {*/

                traceArray[index_num].start = pro_begin;  		   //起始地址为虚拟地址的下一块
                for(int d=pro_begin;d<pro_begin+traceArray[index_num].block;d++)
                {
                    traceblock[d].PID=op.PID;
                }
                //	totalviradd = pro_begin + traceArray[index].block;     //totalviradd重新赋值
                //	}

                //该进程若被创建，则超过用户区总大小，则不可被创建
                /*else
                {
                    index--;
                    break;
                }*/

                //创建文件时将文件信息记录

                if(fileblock.PID >= 100 && fileblock.PID <= 200 && filenum < MAX_FILE){
                    int find_file = 0;
                    for(int i = 0; i < filenum; i++){
                        if(fileblock.PID == file_list[i].PID && file_list[i].filename == fileblock.filename){
                            find_file = 1;
                            break;
                        }

                    }
                    if(!find_file){
                        file_list[filenum].filename = fileblock.filename;
                        file_list[filenum].block = fileblock.block;
                        file_list[filenum].PID = fileblock.PID;
                        for(int i=0;i < fileblock.block;i++){
                            file_list[filenum].PhyAddress[i]=fileblock.PhyAddress[i];
                        }
                        filenum++;
                    }
                }

                cout << endl << "*************************************************************************" << endl;
                //cout << "Ready." << endl;
                cout << "PID:" << traceArray[index_num].PID << endl;
                cout << "Starting Address:" << traceArray[index_num].start << endl;
                cout << "Block:" << traceArray[index_num].block << endl;
                cout << "oper:" << op.oper<<endl;
                cout<<"index"<<index_num<<endl;
                cout << "*************************************************************************" << endl;

            }
        }
        //一个进程的大小就大于用户区总大小，则不可被创建
        else
        {
            index_num--;
            break;
        }
        //输出当前进程的信息


        break;
    }

    case 1://提交一个区域，即进程运行状态
    {
        //输出当前进程的信息

        int flagexist = 0;//判断该进程是否被创建

        //找到对应的PID的起始地址
        for (int i = 0; i <= index_num; i++)
            if (traceArray[i].PID == op.PID)
            {
                flagexist = 1;//之前被创建了，则可以运行
                break;
            }

        if (flagexist == 0)
        {
            cout << "之前未被创建了，不可以运行ERROR.\n";//否则，不可运行
            break;
        }


        //利用小黑框输入,以a结尾
        int current_filenum= p_file[op.PID].filenum;
        p_file[op.PID].fileblock[current_filenum].filename=fileblock.filename;
        p_file[op.PID].fileblock[current_filenum].block=fileblock.block;
        p_file[op.PID].fileblock[current_filenum].PID=fileblock.PID;
        for(int i=0;i<p_file[op.PID].fileblock[current_filenum].block;i++)
        {
            p_file[op.PID].fileblock[current_filenum].PhyAddress[i]=fileblock.PhyAddress[i];
        }

        p_file[op.PID].fileblock[current_filenum].oper=fileblock.oper;

        p_file[op.PID].filenum++;
        int operation_pid=p_file[op.PID].filenum;


        //p_file[op.PID].filenum--;
        //   current_filenum--;
        /*for (int i = 0; i <=p_file[op.PID].filenum; i++)
                                       {
                                           cout<<"dududustart:"<<p_file[op.PID].fileblock[i].start<<endl<<"filename:"<<cout<<p_file[op.PID].fileblock[i].filename
                                           <<endl<<"block:"<<cout<<p_file[op.PID].fileblock[i].block<<"filenum"<<p_file[op.PID].filenum<<endl;
                                          // filecount--;
                                       }*/

        int strategy=0;


        //将物理地址和逻辑地址进行对应并维护
        int filecount = p_file[op.PID].filenum-1;//记录文件数组中的文件信息 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        int p_filecount = p_file[op.PID].filenum-1;//已经操作的文件个数计数
        //totalviradd=traceArray[index].start;//整体虚拟地址起始地址，即已占用的虚拟地址最大值
        int index0=0;
        while(traceArray[index0].PID!=fileblock.PID)
            index0++;

        for (; p_filecount<operation_pid; p_filecount++)
        {
            //测试进程中是否有相同的文件 ，若存在则不需要再次读入
            int exist = 0;
            for (int i = 0; i<p_filecount; i++)
                if (p_file[op.PID].fileblock[i].filename == p_file[op.PID].fileblock[p_filecount].filename)
                    exist = 1;


            //假如文件中没有现在操作的文件 ，并且要对文件进行读操作
            if (exist == 0 && p_file[op.PID].fileblock[p_filecount].oper == 0)
            {//end ofif
                if(p_file[op.PID].fileblock[p_filecount].block>traceArray[index0].block)
                    cout<<"文件太大！空间不可用！！"<<endl;
                else{
                    int blockcount;//用于在进程被分配的虚拟地址空间搜寻可用块
                    int exist_valid = 0;//是否存在满足要操作文件大小的可用块
                    int countgoon;//在blockcount的基础上进行搜寻合适大小的块
                    // cout<<"^^^^^^^"<<endl;
                    for (blockcount = traceArray[index0].start; blockcount <= traceArray[index0].start + traceArray[index0].block-1; blockcount++)
                    {//
                        //当valid为1的时候，找到合适的块数，即为首次适应法
                        if (traceblock[blockcount].valid == 1)
                            for (countgoon = blockcount; (countgoon <= blockcount + p_file[op.PID].fileblock[p_filecount].block - 1)&&(traceblock[countgoon].valid == 1); countgoon++);


                        if (traceblock[countgoon-1].valid == 1)
                        {//
                            exist_valid = 1;
                            break;
                        }//

                    }//

                    //在找到可用的空闲数量的块数时，进行填充
                    if (exist_valid == 1)
                    {//
                        p_file[op.PID].fileblock[p_filecount].start = blockcount;
                        //  cout<<"找到可用的空闲数量的块数时，进行填充"<<endl;
                        //对于每一个块，填写它的物理地址和虚拟地址；从每个fileblock的start开始
                        for(int i = 0; i<p_file[op.PID].fileblock[p_filecount].block; i++)
                        {//

                            traceblock[blockcount].phy = p_file[op.PID].fileblock[p_filecount].PhyAddress[i];
                            traceblock[blockcount].vir = blockcount;//必须在当前虚拟最大值的基础上增加
                            traceblock[blockcount].valid = 0;
                            blockcount++;
                            //	cout<<"filename"<<p_file[op.PID].fileblock[p_filecount].filename<<endl;
                            //	cout<<"--viraddress----phyaddress    "<<"----"<<traceblock[blockcount-1].vir<<"----"<<traceblock[blockcount-1].phy<<endl;
                        }//
                        filecount++;
                        //  cout<<"!!filecount"<<filecount<<endl;
                        //p_file[op.PID].filenum;//filenum为文件数量
                        //p_file[op.PID].fileblock[filecount] = p_file[op.PID].fileblock[p_filecount];
                    }//

                    //当找不到合适的块数，进行替换
                    else
                    {
                        // cout<<"当找不到合适的块数，进行替换"<<endl;
                        //从0开始找文件，一直找到可以放下的

                        int count_file = 0;//需要的文件个数 ，从0开始,指向下一个将要替换的文件
                        int flag_count_file=0;
                        int count_ = p_file[op.PID].fileblock[0].block;//已经得到的文件块，但是不足以放置要操作的文件
                        if (p_file[op.PID].fileblock[count_file].block>traceArray[index0].block)//若第一个文件所占块数即可满足要操作的文件
                            flag_count_file=1;
                        count_file++;
                        if(flag_count_file==0)
                        {//
                            for (count_file = 1; count_<p_file[op.PID].fileblock[p_filecount].block; count_file++)
                            {
                                count_ = count_ + p_file[op.PID].fileblock[count_file].block;
                            }
                            //count_file--;
                            p_file[op.PID].fileblock[p_filecount].start = p_file[op.PID].fileblock[0].start;//赋值虚拟地址的开始值
                        }//
                        //cout<<"第0个文件开始的虚拟地址"<< p_file[op.PID].fileblock[0].start<<endl;

                        //cout<<"被替换的文件，从0到"<< count_file-1<<endl;
                        for (int i = 0, k = 0; i<count_file; i++)//开始替换
                        {
                            for (int j = p_file[op.PID].fileblock[i].start; j<p_file[op.PID].fileblock[i].block + p_file[op.PID].fileblock[i].start; j++, k++)
                            {
                                //  cout<<"开始虚拟地址："<<p_file[op.PID].fileblock[i].start<<endl;
                                //  cout<<j<<"--";
                                if (k<p_file[op.PID].fileblock[p_filecount].block)//若未全部替换为要操作的文件
                                {
                                    traceblock[j].phy = p_file[op.PID].fileblock[p_filecount].PhyAddress[k];
                                    traceblock[j].vir = j;//必须在当前虚拟最大值的基础上增加
                                    traceblock[j].valid = 0;
                                    //	   cout<<"filename"<<p_file[op.PID].fileblock[p_filecount].filename<<endl;
                                    //	cout<<"viraddress----phyaddress    "<<traceblock[j].vir<<"----"<<traceblock[j].phy<<endl;

                                }
                                else//已经全部替换，剩余空间则变为可用
                                {
                                    // cout<<"变为可用！virtual address"<<j<<endl;
                                    traceblock[j].valid = 1;
                                }

                            }
                        }
                        //filecount++;

                        for (int i = count_file, j = 0; i <= filecount; i++, j++)
                        {
                            p_file[op.PID].fileblock[j] = p_file[op.PID].fileblock[i];
                            // filecount--;
                        }

                        filecount=filecount-count_file+1;//filecount是指数组最后的下标
                        p_file[op.PID].fileblock[filecount] = p_file[op.PID].fileblock[p_filecount];

                        /*	for(int i = filecount+1, j = filecount+2; j < p_file[op.PID].filenum; i++, j++)//数组移位
                                       {
                                           p_file[op.PID].fileblock[i] = p_file[op.PID].fileblock[j];
                                          // filecount--;
                                       }*/


                        p_file[op.PID].filenum=p_file[op.PID].filenum-count_file+1;//filenum为文件数量
                    }
                }
            }//end of if
            //假如存在且文件操作为删除
            else if (exist == 1 && p_file[op.PID].fileblock[p_filecount].oper == 1)
            {
                int exist_filecount=0;//文件在数组中的位置
                for (exist_filecount = 0; exist_filecount<p_filecount; exist_filecount++)
                    if (p_file[op.PID].fileblock[exist_filecount].filename == p_file[op.PID].fileblock[p_filecount].filename)
                        break;//找到退出

                for (int i = p_file[op.PID].fileblock[exist_filecount].start; i < p_file[op.PID].fileblock[exist_filecount].start + p_file[op.PID].fileblock[exist_filecount].block; i++)
                {
                    traceblock[i].valid = 1;//该文件的位置均置为可用
                    cout<<"virtual address "<<i<<" is valiad"<<endl;
                }
                for (int i = exist_filecount, j = exist_filecount+1; j <= filecount; i++, j++)//数组移位
                {
                    p_file[op.PID].fileblock[i] = p_file[op.PID].fileblock[j];
                    // filecount--;
                }
                filecount=filecount-1;//filecount是指数组最后的下标
                p_file[op.PID].filenum--;//filenum为文件数量

            }
            else if (exist == 0 && p_file[op.PID].fileblock[p_filecount].oper == 1)
                return;
            else if (exist == 1 && p_file[op.PID].fileblock[p_filecount].oper == 0)
            {
                //	cout<<"要读入的文件已经读入！！"<<endl;
                if(strategy==1)
                {
                    int exist_filecount=0;//文件在数组中的位置
                    for (exist_filecount = 0; exist_filecount<=p_filecount; exist_filecount++)
                        if (p_file[op.PID].fileblock[exist_filecount].filename == p_file[op.PID].fileblock[p_filecount].filename)
                        {
                            cout<<"!!!find"<<endl;
                            break;//找到退出
                        }


                    p_file[op.PID].fileblock[filecount+10] =p_file[op.PID].fileblock[exist_filecount] ;
                    //	cout<<"!!!filecount"<<filecount<<endl;
                    for (int i = exist_filecount, j = exist_filecount+1; j <= filecount; i++, j++)//数组移位
                    {
                        p_file[op.PID].fileblock[i] = p_file[op.PID].fileblock[j];
                        // filecount--;
                    }
                    filecount--;
                    /*for(int i = filecount+1, j = filecount+2; j < p_file[op.PID].filenum; i++, j++)//数组移位
                                       {
                                           p_file[op.PID].fileblock[i] = p_file[op.PID].fileblock[j];
                                          // filecount--;
                                       }*/
                    p_file[op.PID].filenum--;
                    // p_filecount--;
                    //	p_file[op.PID].fileblock[filecount] =p_file[op.PID].fileblock[filecount+10] ;

                }


            }

            // filecount++;
        }//end of for
        // }
        cout << endl << "*************************************************************************" << endl;
        cout << "Running." << endl;
        cout << "*************************************************************************" << endl;
        break;
    }

        //进程死亡，释放一个区域，释放备用页面，即将物理存储和虚拟地址空间全部释放
    case 2:
    {
        //找到应该删除的PID的值
        int delete_p = -1;
        for (int i = 0; i <= index_num; i++)
        {
            if (traceArray[i].PID == op.PID)
            {
                delete_p = i;
                break;
            }
        }

        //当找不到该PID时，则跳出
        if (delete_p == -1)
            break;

        //将要删除的PID后的全部移到前面
        for (int i = traceArray[delete_p].start; i < traceArray[delete_p].start + traceArray[delete_p].block; i++)
        {
            traceblock[i].valid = 1;
            traceblock[i].PID=-1;
        }
        for (int i = delete_p, j = delete_p+1; j <= index_num; i++, j++)//数组移位
        {
            traceArray[i] = traceArray[j];
        }
        index_num--;


        break;
    }

    default:
        //	cout << "error111" << endl;
        break;
    }

    //	ReleaseSemaphore(trac, 1, NULL); 		//释放信号量通知tracker可以打印信息
    outfile << endl << "************************************************************************************" << endl;
    for (int i = 0; i<32; i++)//输出到日志文件中
    {
        //输出每个块的虚拟地址、物理地址和对应的所属进程ID
        outfile << i << "th virtual block:" << endl;
        outfile << "\t" << "Process:" << traceblock[i].PID << endl;
        outfile << "\t" << "Virtual Address: " << traceblock[i].vir << endl;
        outfile << "\t" << "Physical Address: " << traceblock[i].phy << endl;
    }
    outfile << "************************************************************************************" << endl;

    outfile.close();


}




    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //The End
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
