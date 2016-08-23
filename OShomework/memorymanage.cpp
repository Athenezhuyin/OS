////////////////////////////////////////////////////////////////////////////////////////////////////////
//memory-op.cpp
//�ڴ����ʵϰ
//��������ļ�����ÿ�εĲ���������������뵽out.txt�ļ���
////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <fstream.h>
#include <windows.h>
#include <iostream>
#include <cstring>
#include <string>
using namespace std;

struct Operation
{
	int PID;				//���̱�� 
	int time;			//��ʼʱ��
	int block;			//�ڴ�ҳ��
	int oper;			//����
	int protection;			//Ȩ��	
};

//����ÿһ�η��������ݽṹ--���� 
struct tracePro 
{
	int PID;
	int start;			//��ʼ��ַ
	int block;   			//����Ĵ�С
	int used_block;			//�Ѿ�ʹ�õĿ��� 
};

//�����ַ�������ַ��Ӧ�Ľṹ--ÿ�� 
struct TraceBlock
{
	int PID;
	TraceBlock() :valid(true){}	//����ÿһ���validΪ1 
	int phy;				//ÿһ���Ӧ�������ַ 
	int vir;				//ÿһ���Ӧ�������ַ 
	int valid;			//validΪ��ǰ���Ƿ���� 

};

//ÿ���ļ���Ӧ�������ַ�Ϳ���--�ļ� 
struct FileBlock
{
	int start;			//ÿ���ļ�����ʼ�����ַ 
	int PID;				//ÿ���ļ�������PID
	string filename;			//�ļ��� 
	int block;			//�ļ���Ӧ�Ŀ��� 
	int PhyAddress[100];		//ÿ���ļ���Ӧ�������ַ 
	int oper;			//���� 
};

//�ļ���ͽ��� 
struct P_File	
{
	FileBlock fileblock[100];		//һ�����̶�Ӧ���ļ�	
	int filenum;			//�ļ���Ŀ 
	P_File() :filenum(0){}		//�ļ���Ŀ��ʼΪ0 
};

HANDLE allo, trac; 			//�ź����ľ��
const int MAX_P = 200;			//ά���Ľ��������Ŀ 
tracePro  traceArray[MAX_P];		//���ٽ��̵����� 
TraceBlock traceblock[32];	 		//ά���Ŀ������������û��ռ�Ϊ32�� 
const int PageSize = 1024;			//ÿһҳ�Ĵ�С 
P_File p_file[MAX_P];			//һ���Ľ���������ÿ�����̶�Ӧ���ļ� 
FileBlock fileblock[32];			//�ļ� 

//��������
int filenum = 0;				//�ļ���Ŀ 
int blocknum = 0;				//���� 
int index = -1;				//������ 
int totalviradd = 0;			//��ǰռ�õ������ַ����һ��
 


/**************Tracker����ÿһ������ִ��֮�󣬾���ҳ�����һ��ά��*************/
/**************����allocator�̵߳��ڴ���Ϊ���������Ҫ��Ϣ*********************/

/*void Tracker() 
{
	//����ļ�
	ofstream outfile;			
	outfile.open("out.txt");
		
	cout<<endl<<endl;
	cout<<"*******************************Maintain*********************************************"<<endl;
	
	for (int i = 0; i<32; i++)
	{
		//���ÿ����������ַ�������ַ�Ͷ�Ӧ����������ID 
		outfile << endl << "************************************************************************************" << endl;
		outfile << i << "th virtual block:" << endl;			
		outfile << "\t" << "Process:" << traceblock[i].PID << endl;
		outfile << "\t" << "Virtual Address: " << traceblock[i].vir << endl;
		outfile << "\t" << "Physical Address: " << traceblock[i].phy << endl;
	}
	return ;
}


/*****************************ģ���ڴ�������߳�***************************/

void Allocator(Operation op,FileBlock fileblock)
{
	DWORD temp;
	
	ofstream outfile;
	outfile.open("out.txt");//�����־ 
	int a =p_file[op.PID].filenum+1;
    p_file[op.PID].fileblock[a]=fileblock;//Ϊ��¼�����ļ������鸳ֵ 
    
	//fread(&op, sizeof(operation), 1, file);
	//Sleep(op.time); 		    		//ִ��ʱ�䣬�������ָ��ʱ��ִ�п���ȡ��ע��

	//�����ļ�����ȷ��Ȩ��
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

	//Ϊ���̷����ڴ� 
	switch (op.oper)
	{
		//����һ�����򣬴�������ʱ 
		case 0:
		{
			//��¼���̵�����tracearray��ֵ
  			index++;
		   	traceArray[index].PID = op.PID;		
		   	traceArray[index].block = op.block;
		   	traceArray[index].used_block = 0;
            int pro_block=0;
            int pro_valid=0; 
            int pro_begin=0;
			//���������Ҫ�Ŀ���С���û����ܴ�С 
			if (traceArray[index].block <= 32)		
   			{
		        
                 for (pro_begin=0; pro_begin<32; pro_begin++)
				  {
	   					//��validΪ1��ʱ���ҵ����ʵĿ�������Ϊ�״���Ӧ�� 
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
   				
   			/*	//�û�������ʼ��ַ,��indexΪ0ʱ�����̵���ʼ��ַΪ0 ����һ�����̱�����
			   	if (index == 0)			
			   	{
					traceArray[0].start = 0; 				   //��ʼ�����ַΪ0 
		   			totalviradd = traceArray[0].start + traceArray[0].block; //totalviradd�����ַ����һ�� 
		   		}
           */
		   		//��һ���Ժ�Ľ��̱����������ҽ���ռ��������ռ���û����ܴ�СҪС����indexΪ����ֵʱ�����̵���ʼ��ַΪ��һ�����̵���ʼ��ַ���Ͽ��� 
		   	/*	else if (totalviradd + traceArray[index].block-1<=32)
			   	{*/

					traceArray[index].start = pro_begin;  		   //��ʼ��ַΪ�����ַ����һ�� 
		   		//	totalviradd = pro_begin + traceArray[index].block;     //totalviradd���¸�ֵ 
		   	//	}
		   		
				//�ý��������������򳬹��û����ܴ�С���򲻿ɱ����� 
				/*else
				{
	   				index--;
	   				break;
				}*/

              }
			}
			
   			//һ�����̵Ĵ�С�ʹ����û����ܴ�С���򲻿ɱ����� 
   			else
   			{
  	   			index--;
   				break;
			}
   
			break;
		}

		//�ύһ�����򣬼���������״̬ 
		case 1:
		{
			
  			int flagexist = 0;				//�жϸý����Ƿ񱻴��� 

			//�ҵ���Ӧ��PID����ʼ��ַ 
   			for (int i = 0; i <= index; i++)
   			{
   				if (traceArray[i].PID == op.PID)
		   		{
	   				flagexist = 1;			//֮ǰ�������ˣ���������� 
		   		}	
			}
   				

			if (flagexist == 0)
			{
				return;
	   		}	
			
			int current_filenum= p_file[op.PID].filenum;		//�ļ���Ŀ
			//���������Ҫ���ļ������ļ��������ļ�ÿһ��������ַ�Լ��ļ���Ӧ�Ĳ��� 
			p_file[op.PID].fileblock[current_filenum].filename=fileblock.filename;
			p_file[op.PID].fileblock[current_filenum].block=fileblock.block;
			
			for(int i=0;i<p_file[op.PID].fileblock[current_filenum].block;i++)
			{
				p_file[op.PID].fileblock[current_filenum].PhyAddress[i]=fileblock.PhyAddress[i];	
			}
			
       			p_file[op.PID].fileblock[current_filenum].oper=fileblock.oper;
       			
       			//�ý���ʹ�õ��ļ�������һ 
   			p_file[op.PID].filenum++;          			
   			current_filenum++;
   

			int strategy;
			
			//�������ַ���߼���ַ���ж�Ӧ��ά�� 
			int filecount = 0;//��¼�ļ������е��ļ���Ϣ 
            int p_filecount = 0;//�Ѿ��������ļ���������
		         
   			for (; p_filecount<p_file[op.PID].filenum; p_filecount++)
  			{
		   		//���Խ������Ƿ�����ͬ���ļ� ������������Ҫ�ٴζ��� 
	   			int exist = 0;
	   			for (int i = 0; i<p_filecount; i++)
		   			if (p_file[op.PID].fileblock[i].filename == p_file[op.PID].fileblock[p_filecount].filename)
			   			exist = 1;


				//�����ļ���û�����ڲ������ļ� ������Ҫ���ļ����ж����� 
	   			if (exist == 0 && p_file[op.PID].fileblock[p_filecount].oper == 0)
	   			{
   					if(p_file[op.PID].fileblock[p_filecount].block>traceArray[index].block)
			   			return;
				   	
				   	else{
						int blockcount;		//�����ڽ��̱�����������ַ�ռ���Ѱ���ÿ� 
  						int exist_valid = 0;	//�Ƿ��������Ҫ�����ļ���С�Ŀ��ÿ� 
					   	int countgoon;		//��blockcount�Ļ����Ͻ�����Ѱ���ʴ�С�Ŀ� 
							  
						for (blockcount = traceArray[index].start; blockcount <= traceArray[index].start + traceArray[index].block-1; blockcount++)
						{
	   						//��validΪ1��ʱ���ҵ����ʵĿ�������Ϊ�״���Ӧ�� 
			   				if (traceblock[blockcount].valid == 1)
			   					for (countgoon = blockcount; (countgoon <= blockcount + p_file[op.PID].fileblock[p_filecount].block - 1)
								                              &&(traceblock[countgoon].valid == 1); countgoon++);		
								   if (traceblock[countgoon-1].valid == 1)
								   {
									   exist_valid = 1;
									   break;
								   }
						}
							   
							
						//���ҵ����õĿ��������Ŀ���ʱ��������� 
						if (exist_valid == 1)
						{
							    p_file[op.PID].fileblock[p_filecount].start = blockcount;
								
							 //����ÿһ���飬��д���������ַ�������ַ����ÿ��fileblock��start��ʼ 
		      					for(int i = 0; i<p_file[op.PID].fileblock[p_filecount].block; i++)
							{			
								traceblock[blockcount].phy = p_file[op.PID].fileblock[p_filecount].PhyAddress[i];
  		 						traceblock[blockcount].vir = blockcount;//�����ڵ�ǰ�������ֵ�Ļ��������� 
			 	 	 			traceblock[blockcount].valid = 0;
					   			blockcount++;
   							 }
									   
						}
									
						//���Ҳ������ʵĿ����������滻 
						else
				   		{
								   			
				   			//��0��ʼ���ļ���һֱ�ҵ����Է��µ�					   	
					   		int count_file = 0;			//��Ҫ���ļ����� ����0��ʼ,ָ����һ����Ҫ�滻���ļ�	
							int flag_count_file=0;
							int count_ = p_file[op.PID].fileblock[0].block;//�Ѿ��õ����ļ��飬���ǲ����Է���Ҫ�������ļ�						   
							if (p_file[op.PID].fileblock[count_file].block>traceArray[index].block)//����һ���ļ���ռ������������Ҫ�������ļ� 
					   				flag_count_file=1;
							 	
							count_file++;
					    			
	    		            if(flag_count_file==0)//��һ���ļ������Է���Ҫ�򿪵��ļ� 
				    		{
								for (count_file = 1; count_<p_file[op.PID].fileblock[p_filecount].block; count_file++)
		   						{
						   			count_ = count_ + p_file[op.PID].fileblock[count_file].block;
						   		}
									   
						   		p_file[op.PID].fileblock[p_filecount].start = p_file[op.PID].fileblock[0].start;//��ֵ�����ַ�Ŀ�ʼֵ 	
							}
							  
							for (int i = 0, k = 0; i<count_file; i++)//��ʼ�滻 
					   		{
								for (int j = p_file[op.PID].fileblock[i].start; j<p_file[op.PID].fileblock[i].block + p_file[op.PID].fileblock[i].start; j++, k++)
								{
									if (k<p_file[op.PID].fileblock[p_filecount].block)//��δȫ���滻ΪҪ�������ļ� 
									{
										traceblock[j].phy = p_file[op.PID].fileblock[p_filecount].PhyAddress[k];
										traceblock[j].vir = j;//�����ڵ�ǰ�������ֵ�Ļ��������� 
										traceblock[j].valid = 0;
									 }
									 //�Ѿ�ȫ���滻��ʣ��ռ����Ϊ���� 
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
					   			
					   		filecount=filecount-count_file+1;//filecount��ָ���������±� 
					   		p_file[op.PID].filenum=filecount+1;//filenumΪ�ļ����� 
					   		p_file[op.PID].fileblock[filecount] = p_file[op.PID].fileblock[p_filecount];
						   }
					  }
				   }//end of if 
						  
				    //����������ļ�����Ϊɾ�� 
				   else if (exist == 1 && p_file[op.PID].fileblock[p_filecount].oper == 1)
				   {
			   		int exist_filecount=0;		//�ļ��������е�λ�� 
				   	for (exist_filecount = 0; exist_filecount<p_filecount; exist_filecount++)
		   				//�ҵ��˳� 
			   			if (p_file[op.PID].fileblock[exist_filecount].filename == p_file[op.PID].fileblock[p_filecount].filename)
				   			break;	
							   		
				   	for (int i = p_file[op.PID].fileblock[exist_filecount].start; i < p_file[op.PID].fileblock[exist_filecount].start + p_file[op.PID].fileblock[exist_filecount].block; i++)
				   	{
					   	traceblock[i].valid = 1;//���ļ���λ�þ���Ϊ���� 
				   	}
					
					for (int i = exist_filecount, j = exist_filecount+1; j <= filecount; i++, j++)//������λ 
					{
						p_file[op.PID].fileblock[i] = p_file[op.PID].fileblock[j];
					}
					
					filecount=filecount-1;//filecount��ָ���������±� 
				 	p_file[op.PID].filenum=filecount+1;//filenumΪ�ļ����� 

                   }
				    else if (exist == 0 && p_file[op.PID].fileblock[p_filecount].oper == 1)//��������ڸ��ļ�����Ҫɾ���ļ� 
					return;   
				    
				    else if (exist == 1 && p_file[op.PID].fileblock[p_filecount].oper == 0) //������ڸ��ļ�����Ҫ���ļ� 
				    {
    					if(strategy==0);              //�Ƚ��ȳ����� 
    					else if(strategy==1)          //LRU���� 
    					{
  						 int exist_filecount=0;       //�ļ��������е�λ�� 
						 for (exist_filecount = 0; exist_filecount<p_filecount; exist_filecount++)
						 if (p_file[op.PID].fileblock[exist_filecount].filename == p_file[op.PID].fileblock[p_filecount].filename)
							break;                   //�ҵ��˳� 
						
						p_file[op.PID].fileblock[filecount+10] =p_file[op.PID].fileblock[exist_filecount] ;
			            for (int i = exist_filecount, j = exist_filecount+1; j <= filecount; i++, j++)//������λ 
						{
							p_file[op.PID].fileblock[i] = p_file[op.PID].fileblock[j];
						}
						p_file[op.PID].fileblock[filecount] =p_file[op.PID].fileblock[filecount+10] ; //���ոն������ļ��Ƶ���� 
  					}
				    }
        
			   }
		   break;
		}

                  //�����������ͷ�һ�������ͷű���ҳ�棬��������洢�������ַ�ռ�ȫ���ͷ� 
		case 2:
		{
				  //�ҵ�Ӧ��ɾ����PID��ֵ 
				  int delete_p = -1;
				  for (int i = 0; i <= index; i++)
				  {
					  if (traceArray[i].PID == op.PID)
					  {
						  delete_p = i;
						  break;
					  }
				  }

				  //���Ҳ�����PIDʱ��������
				  if (delete_p == -1)
					  break;

				  //��Ҫɾ����PID���ȫ���Ƶ�ǰ�� 
				  for (int i = traceArray[delete_p].start; i < traceArray[delete_p].start + traceArray[delete_p].block; i++)
				  {
					  traceblock[i].valid = 1;
				  }
				  for (int i = delete_p, j = delete_p+1; j <= index; i++, j++)//������λ 
						{
							p_file[op.PID].fileblock[i] = p_file[op.PID].fileblock[j];
						}
				  index--;

				  //��ӡ��Ϣ 
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
			//ReleaseSemaphore(trac, 1, NULL); 		//�ͷ��ź���֪ͨtracker���Դ�ӡ��Ϣ
			for (int i = 0; i<32; i++)//�������־�ļ��� 
	{
		//���ÿ����������ַ�������ַ�Ͷ�Ӧ����������ID 
		outfile << endl << "************************************************************************************" << endl;
		outfile << i << "th virtual block:" << endl;			
		outfile << "\t" << "Process:" << traceblock[i].PID << endl;
		outfile << "\t" << "Virtual Address: " << traceblock[i].vir << endl;
		outfile << "\t" << "Physical Address: " << traceblock[i].phy << endl;
	}
}// end of allocator


	/*������*/
	int main()
	{
		DWORD dwThread;
		HANDLE handle[2];
		//���������߳�
		handle[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Tracker, NULL, 0, &dwThread);
		handle[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Allocator, NULL, 0, &dwThread);

		//���������ź���
		allo = CreateSemaphore(NULL, 0, 1, "allo");
		trac = CreateSemaphore(NULL, 1, 1, "trac");

		//�ȴ��߳�ִ�е�ִ�н��������˳�
		WaitForMultipleObjects(2, handle, TRUE, INFINITE);
		return 0;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//The End
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
