////////////////////////////////////////////////////////////////////////////////////////////////////////
//memory-op.cpp
//内存管理实习
//将程序从文件读入每次的操作，并将结果输入到out.txt文件中
////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <fstream.h>
#include <windows.h>
#include <iostream>
#include <cstring>
#include <string>
using namespace std;

struct Operation
{
	int PID;				//进程编号 
	int time;			//起始时间
	int block;			//内存页数
	int oper;			//操作
	int protection;			//权限	
};

//跟踪每一次分配活动的数据结构--进程 
struct tracePro 
{
	int PID;
	int start;			//起始地址
	int block;   			//分配的大小
	int used_block;			//已经使用的块数 
};

//物理地址、虚拟地址对应的结构--每块 
struct TraceBlock
{
	int PID;
	TraceBlock() :valid(true){}	//设置每一块的valid为1 
	int phy;				//每一块对应的物理地址 
	int vir;				//每一块对应的虚拟地址 
	int valid;			//valid为当前块是否可用 

};

//每个文件对应的物理地址和块数--文件 
struct FileBlock
{
	int start;			//每个文件的起始虚拟地址 
	int PID;				//每个文件所属的PID
	string filename;			//文件名 
	int block;			//文件对应的块数 
	int PhyAddress[100];		//每个文件对应的物理地址 
	int oper;			//操作 
};

//文件与就进程 
struct P_File	
{
	FileBlock fileblock[100];		//一个进程对应的文件	
	int filenum;			//文件数目 
	P_File() :filenum(0){}		//文件数目初始为0 
};

HANDLE allo, trac; 			//信号量的句柄
const int MAX_P = 200;			//维护的进程最大数目 
tracePro  traceArray[MAX_P];		//跟踪进程的数组 
TraceBlock traceblock[32];	 		//维护的块数——整个用户空间为32块 
const int PageSize = 1024;			//每一页的大小 
P_File p_file[MAX_P];			//一共的进程数——每个进程对应的文件 
FileBlock fileblock[32];			//文件 

//计数变量
int filenum = 0;				//文件数目 
int blocknum = 0;				//块数 
int index = -1;				//进程数 
int totalviradd = 0;			//当前占用的虚拟地址的下一块
 


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

void Allocator(Operation op,FileBlock fileblock)
{
	DWORD temp;
	
	ofstream outfile;
	outfile.open("out.txt");//输出日志 
	int a =p_file[op.PID].filenum+1;
    p_file[op.PID].fileblock[a]=fileblock;//为记录进程文件的数组赋值 
    
	//fread(&op, sizeof(operation), 1, file);
	//Sleep(op.time); 		    		//执行时间，如果想在指定时间执行可以取消注释

	//根据文件内容确定权限
	switch (op.protection) 
	{
		//0-PAGE_READONLY
		case 0:
		{
  			index = -1;
		  	temp = PAGE_READONLY;
  			break;
		}
		//1-PAGE_READWRITE
		case 1:
			temp = PAGE_READWRITE;
			break;
		//2-PAGE_EXECUTE
		case 2:
			temp = PAGE_EXECUTE;
			break;
		//3-PAGE_EXECUTE_READ
		case 3:
			temp = PAGE_EXECUTE_READ;
			break;
		//4-PAGE_EXECUTE_READWRITE	
		case 4:
			temp = PAGE_EXECUTE_READWRITE;
			break;
		default:
			temp = PAGE_READONLY;
			break;
	}

	//为进程分配内存 
	switch (op.oper)
	{
		//保留一个区域，创建进程时 
		case 0:
		{
			//记录进程的数组tracearray赋值
  			index++;
		   	traceArray[index].PID = op.PID;		
		   	traceArray[index].block = op.block;
		   	traceArray[index].used_block = 0;
            int pro_block=0;
            int pro_valid=0; 
            int pro_begin=0;
			//如果进程需要的块数小于用户区总大小 
			if (traceArray[index].block <= 32)		
   			{
		        
                 for (pro_begin=0; pro_begin<32; pro_begin++)
				  {
	   					//当valid为1的时候，找到合适的块数，即为首次适应法 
			   			if (traceblock[pro_begin].valid == 1)
			   				for (pro_block = pro_begin; (pro_block < pro_begin + traceArray[index].block)
								    &&(traceblock[pro_block].valid == 1); pro_block++);		
							if (traceblock[pro_block-1].valid == 1)
							 {
								pro_valid = 1;
								 break;
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

					traceArray[index].start = pro_begin;  		   //起始地址为虚拟地址的下一块 
		   		//	totalviradd = pro_begin + traceArray[index].block;     //totalviradd重新赋值 
		   	//	}
		   		
				//该进程若被创建，则超过用户区总大小，则不可被创建 
				/*else
				{
	   				index--;
	   				break;
				}*/

              }
			}
			
   			//一个进程的大小就大于用户区总大小，则不可被创建 
   			else
   			{
  	   			index--;
   				break;
			}
   
			break;
		}

		//提交一个区域，即进程运行状态 
		case 1:
		{
			
  			int flagexist = 0;				//判断该进程是否被创建 

			//找到对应的PID的起始地址 
   			for (int i = 0; i <= index; i++)
   			{
   				if (traceArray[i].PID == op.PID)
		   		{
	   				flagexist = 1;			//之前被创建了，则可以运行 
		   		}	
			}
   				

			if (flagexist == 0)
			{
				return;
	   		}	
			
			int current_filenum= p_file[op.PID].filenum;		//文件数目
			//读入进程需要的文件名、文件块数、文件每一块的物理地址以及文件对应的操作 
			p_file[op.PID].fileblock[current_filenum].filename=fileblock.filename;
			p_file[op.PID].fileblock[current_filenum].block=fileblock.block;
			
			for(int i=0;i<p_file[op.PID].fileblock[current_filenum].block;i++)
			{
				p_file[op.PID].fileblock[current_filenum].PhyAddress[i]=fileblock.PhyAddress[i];	
			}
			
       			p_file[op.PID].fileblock[current_filenum].oper=fileblock.oper;
       			
       			//该进程使用的文件数量加一 
   			p_file[op.PID].filenum++;          			
   			current_filenum++;
   

			int strategy;
			
			//将物理地址和逻辑地址进行对应并维护 
			int filecount = 0;//记录文件数组中的文件信息 
            int p_filecount = 0;//已经操作的文件个数计数
		         
   			for (; p_filecount<p_file[op.PID].filenum; p_filecount++)
  			{
		   		//测试进程中是否有相同的文件 ，若存在则不需要再次读入 
	   			int exist = 0;
	   			for (int i = 0; i<p_filecount; i++)
		   			if (p_file[op.PID].fileblock[i].filename == p_file[op.PID].fileblock[p_filecount].filename)
			   			exist = 1;


				//假如文件中没有现在操作的文件 ，并且要对文件进行读操作 
	   			if (exist == 0 && p_file[op.PID].fileblock[p_filecount].oper == 0)
	   			{
   					if(p_file[op.PID].fileblock[p_filecount].block>traceArray[index].block)
			   			return;
				   	
				   	else{
						int blockcount;		//用于在进程被分配的虚拟地址空间搜寻可用块 
  						int exist_valid = 0;	//是否存在满足要操作文件大小的可用块 
					   	int countgoon;		//在blockcount的基础上进行搜寻合适大小的块 
							  
						for (blockcount = traceArray[index].start; blockcount <= traceArray[index].start + traceArray[index].block-1; blockcount++)
						{
	   						//当valid为1的时候，找到合适的块数，即为首次适应法 
			   				if (traceblock[blockcount].valid == 1)
			   					for (countgoon = blockcount; (countgoon <= blockcount + p_file[op.PID].fileblock[p_filecount].block - 1)
								                              &&(traceblock[countgoon].valid == 1); countgoon++);		
								   if (traceblock[countgoon-1].valid == 1)
								   {
									   exist_valid = 1;
									   break;
								   }
						}
							   
							
						//在找到可用的空闲数量的块数时，进行填充 
						if (exist_valid == 1)
						{
							    p_file[op.PID].fileblock[p_filecount].start = blockcount;
								
							 //对于每一个块，填写它的物理地址和虚拟地址；从每个fileblock的start开始 
		      					for(int i = 0; i<p_file[op.PID].fileblock[p_filecount].block; i++)
							{			
								traceblock[blockcount].phy = p_file[op.PID].fileblock[p_filecount].PhyAddress[i];
  		 						traceblock[blockcount].vir = blockcount;//必须在当前虚拟最大值的基础上增加 
			 	 	 			traceblock[blockcount].valid = 0;
					   			blockcount++;
   							 }
									   
						}
									
						//当找不到合适的块数，进行替换 
						else
				   		{
								   			
				   			//从0开始找文件，一直找到可以放下的					   	
					   		int count_file = 0;			//需要的文件个数 ，从0开始,指向下一个将要替换的文件	
							int flag_count_file=0;
							int count_ = p_file[op.PID].fileblock[0].block;//已经得到的文件块，但是不足以放置要操作的文件						   
							if (p_file[op.PID].fileblock[count_file].block>traceArray[index].block)//若第一个文件所占块数即可满足要操作的文件 
					   				flag_count_file=1;
							 	
							count_file++;
					    			
	    		            if(flag_count_file==0)//第一个文件不足以放置要打开的文件 
				    		{
								for (count_file = 1; count_<p_file[op.PID].fileblock[p_filecount].block; count_file++)
		   						{
						   			count_ = count_ + p_file[op.PID].fileblock[count_file].block;
						   		}
									   
						   		p_file[op.PID].fileblock[p_filecount].start = p_file[op.PID].fileblock[0].start;//赋值虚拟地址的开始值 	
							}
							  
							for (int i = 0, k = 0; i<count_file; i++)//开始替换 
					   		{
								for (int j = p_file[op.PID].fileblock[i].start; j<p_file[op.PID].fileblock[i].block + p_file[op.PID].fileblock[i].start; j++, k++)
								{
									if (k<p_file[op.PID].fileblock[p_filecount].block)//若未全部替换为要操作的文件 
									{
										traceblock[j].phy = p_file[op.PID].fileblock[p_filecount].PhyAddress[k];
										traceblock[j].vir = j;//必须在当前虚拟最大值的基础上增加 
										traceblock[j].valid = 0;
									 }
									 //已经全部替换，剩余空间则变为可用 
			       						else
	   		 						{
								   		traceblock[j].valid = 1;
									}

								}
							}
						  
  							for (int i = count_file, j = 0; i <= filecount; i++, j++)
   							{
			   					p_file[op.PID].fileblock[j] = p_file[op.PID].fileblock[i];
				   			}
					   			
					   		filecount=filecount-count_file+1;//filecount是指数组最后的下标 
					   		p_file[op.PID].filenum=filecount+1;//filenum为文件数量 
					   		p_file[op.PID].fileblock[filecount] = p_file[op.PID].fileblock[p_filecount];
						   }
					  }
				   }//end of if 
						  
				    //假如存在且文件操作为删除 
				   else if (exist == 1 && p_file[op.PID].fileblock[p_filecount].oper == 1)
				   {
			   		int exist_filecount=0;		//文件在数组中的位置 
				   	for (exist_filecount = 0; exist_filecount<p_filecount; exist_filecount++)
		   				//找到退出 
			   			if (p_file[op.PID].fileblock[exist_filecount].filename == p_file[op.PID].fileblock[p_filecount].filename)
				   			break;	
							   		
				   	for (int i = p_file[op.PID].fileblock[exist_filecount].start; i < p_file[op.PID].fileblock[exist_filecount].start + p_file[op.PID].fileblock[exist_filecount].block; i++)
				   	{
					   	traceblock[i].valid = 1;//该文件的位置均置为可用 
				   	}
					
					for (int i = exist_filecount, j = exist_filecount+1; j <= filecount; i++, j++)//数组移位 
					{
						p_file[op.PID].fileblock[i] = p_file[op.PID].fileblock[j];
					}
					
					filecount=filecount-1;//filecount是指数组最后的下标 
				 	p_file[op.PID].filenum=filecount+1;//filenum为文件数量 

                   }
				    else if (exist == 0 && p_file[op.PID].fileblock[p_filecount].oper == 1)//如果不存在该文件并且要删除文件 
					return;   
				    
				    else if (exist == 1 && p_file[op.PID].fileblock[p_filecount].oper == 0) //如果存在该文件并且要打开文件 
				    {
    					if(strategy==0);              //先进先出策略 
    					else if(strategy==1)          //LRU策略 
    					{
  						 int exist_filecount=0;       //文件在数组中的位置 
						 for (exist_filecount = 0; exist_filecount<p_filecount; exist_filecount++)
						 if (p_file[op.PID].fileblock[exist_filecount].filename == p_file[op.PID].fileblock[p_filecount].filename)
							break;                   //找到退出 
						
						p_file[op.PID].fileblock[filecount+10] =p_file[op.PID].fileblock[exist_filecount] ;
			            for (int i = exist_filecount, j = exist_filecount+1; j <= filecount; i++, j++)//数组移位 
						{
							p_file[op.PID].fileblock[i] = p_file[op.PID].fileblock[j];
						}
						p_file[op.PID].fileblock[filecount] =p_file[op.PID].fileblock[filecount+10] ; //将刚刚读过的文件移到最后 
  					}
				    }
        
			   }
		   break;
		}

                  //进程死亡，释放一个区域，释放备用页面，即将物理存储和虚拟地址空间全部释放 
		case 2:
		{
				  //找到应该删除的PID的值 
				  int delete_p = -1;
				  for (int i = 0; i <= index; i++)
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
				  }
				  for (int i = delete_p, j = delete_p+1; j <= index; i++, j++)//数组移位 
						{
							p_file[op.PID].fileblock[i] = p_file[op.PID].fileblock[j];
						}
				  index--;

				  //打印信息 
				  cout << endl << "*************************************************************************" << endl;
				  cout << "Release." << endl;
				  cout << "PID:" << traceArray[index].PID << endl;
				  cout << "Starting address:" << traceArray[index].start << endl;
				  cout << "Block:" << traceArray[index].block << endl;
				  cout << "*************************************************************************" << endl;
				  break;
		}// end of case 2

		default:
			cout << "error" << endl;
			break;
		}// end of switch
			//ReleaseSemaphore(trac, 1, NULL); 		//释放信号量通知tracker可以打印信息
			for (int i = 0; i<32; i++)//输出到日志文件中 
	{
		//输出每个块的虚拟地址、物理地址和对应的所属进程ID 
		outfile << endl << "************************************************************************************" << endl;
		outfile << i << "th virtual block:" << endl;			
		outfile << "\t" << "Process:" << traceblock[i].PID << endl;
		outfile << "\t" << "Virtual Address: " << traceblock[i].vir << endl;
		outfile << "\t" << "Physical Address: " << traceblock[i].phy << endl;
	}
}// end of allocator


	/*主函数*/
	int main()
	{
		DWORD dwThread;
		HANDLE handle[2];
		//生成两个线程
		handle[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Tracker, NULL, 0, &dwThread);
		handle[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Allocator, NULL, 0, &dwThread);

		//生成两个信号量
		allo = CreateSemaphore(NULL, 0, 1, "allo");
		trac = CreateSemaphore(NULL, 1, 1, "trac");

		//等待线程执行的执行结束后，再退出
		WaitForMultipleObjects(2, handle, TRUE, INFINITE);
		return 0;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//The End
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
