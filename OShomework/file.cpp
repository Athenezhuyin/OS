
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<string>
#include <qDebug>
#include "file.h"
#include "memory.h"

using namespace std;


const char* Commandset[] = {"create","open","write","close","del","cd","dir","exit"};

//位示图，用来存储当前文件块占用情况
int bitmap[512];   //位示图，0表示空闲，1表示占用
char* filesys ;    //文件系统物理空间地址
CState CS;  //当前系统状态
FileIndex FI;  //系统索引
FILE* fmlog;  //日志文件
char logtime[20];  //日志时间

void IniFMSys();  //文件系统初始化
int Createfile(FileAccess acc,const char* fname,FileType type,int size);  //创建文件,权限、文件名、类型、父节点
int FindBlankBlockid();  //寻找空闲文件块
void UpdateBitMap(int slg,int blockid);  //更新位示图 ,slg为0：添加；slg为1：删除
void GetCurTime(char* currtime); //获取当前时间
int PathAnalysis(const char* name,const char* path);  //对路径拆解分析
int Search(const char *name, const char *path);   //查找指定文件
void AddIndex(const char *filename, FileType type, FCB* fcb);  //添加索引
int Create(int pid,string fname,string fpath,int acc,int type,int size);  //创建文件 1111
void Open(int pid,string fname,string fpath);   //打开文件2222
int Write(int pid,string fname,string fpath,int size);   //写文件33333
void Close(int pid,string fname,string fpath);  //关闭文件 4444
void Delete(int pid,string fname,string fpath);   //删除文件55555
void printfminfo();   //打印已有信息


//内存管理
void Allocator(Operation op,FileBlock fileblock);

//初始化
void IniFMSys()  //系统初始化
{
    qDebug()<<"nima";
    int flag;
    fmlog = fopen("filemanagementlog.txt","w");  //打开日志文件
    fprintf(fmlog,"=======================================================\n");
    fprintf(fmlog,"           OS File System management log               \n");
    GetCurTime(logtime);
    fprintf(fmlog,"           start time:%s                                \n",logtime);
    fprintf(fmlog,"           author:  Li Wei  Liang Zi                    \n");
    fprintf(fmlog,"=======================================================\n");


    memset(bitmap,0,BitMapLen);   //将位示图初始化为全0
    FI.indexcount = 0;  //文件索引数目为0
    CS.filelevel = 0;
    CS.currparent = NULL;
    CS.currentpath = (char*)calloc(50,sizeof(char));   //动态分配1000个字符的空间存放路径
    CS.currentpath[0] = '\0';
    //printf("CS.currentpath:%s 5555\n",CS.currentpath);


    flag = Createfile(readwrite,"root",dir,10);   //创建目录文件，权限、文件名、类型、父节点
    CS.filelevel = 1;
    CS.currparent = FI.index[0].fcb;
    CS.currentpath[0] = '/';

    printfminfo();
}
int Create(int pid,string fname,string fpath,int acc,int type,int size) //创建文件
{

    const char* name;
    const char* path;
    name = fname.c_str();
    path = fpath.c_str();
    fprintf(fmlog,"create:   filename:%s   path:%s\n",name,path);
    int slg;
    //qDebug()<<"nima1.1";
    FCB* parent;
    int flag;
    int i,j;
    Operation op;
    FileBlock fb[32];
    if(strcmp(name," ")==0)
    {
        fprintf(fmlog,"对不起，文件名不能为空。\n");
        qDebug()<<"对不起，文件名不能为空.";
        return 0;
    }
    else
    {
        //qDebug()<<"nima1.1.1";
        slg=PathAnalysis(name,path);
        //qDebug()<<"nima1.2";
        if(slg == 0)   //找到目录路径并且没有重名文件
        {
             parent = CS.currparent;
             flag = Createfile((FileAccess)acc,name,(FileType)type,size);
             if(flag == 1)
             {
                fprintf(fmlog,"FCB空间分配失败!\n");
                qDebug()<<"FCB空间分配失败!"<<endl;
                return 0;
             }
             else
                 if(flag == 2)
                 {
                    fprintf(fmlog,"对不起，物理空间已满，无法创建新的文件！\n");
                    qDebug()<<"对不起，物理空间已满，无法创建新的文件！"<<endl;
                    return 0;
                 }
                 else
                 {
                     //qDebug()<<"nima1.3";
                     op.PID = pid;
                     op.time = 0;
                     op.oper = 1;
                     op.protection = FI.index[FI.indexcount-1].fcb->facc;
                     i = FI.index[FI.indexcount-1].fcb->flen/1024;
                     j = FI.index[FI.indexcount-1].fcb->flen%1024;
                     if(j!=0)
                        i++;
                     op.block = i;
                     fb[0].start = 0;
                     fb[0].PID = pid;
                     fb[0].filename = name;
                     fb[0].block = i;
                     fb[0].oper = 0;
                     for(j=0;j<i;j++)
                     {
                         fb[0].PhyAddress[j] = FI.index[FI.indexcount-1].fcb->blocknum[j];
                         qDebug()<<FI.index[FI.indexcount-1].fcb->blocknum[j];
                     }
                     //qDebug()<<"nima1.4";
                     Allocator(op,fb[0]);

                     fprintf(fmlog,"Create file successfully!\n");
                     qDebug()<<"文件创建成功!";
                     qDebug()<<"indexnum:"<<FI.indexcount<<endl;
                 }
        }
        else if(slg == 1)
        {
            qDebug()<<"~~~~"<<slg;
            fprintf(fmlog,"该路径下有重名文件!\n");
            qDebug()<<"该路径下有重名文件!";
            return 0;
        }
        else
        {
            fprintf(fmlog,"该路径不存在!\n");
            qDebug()<<"该路径不存在!";
            return 0;
        }
    }
    qDebug()<<"当前路径:"<<CS.currentpath;
    for(int i=0;i<CS.currparent->childnum;i++)
        qDebug()<<i<<":"<<CS.currparent->child[i]<<endl;
    return 1;
}

int PathAnalysis(const char *name, const char *path)  //对路径拆解分析
{
    int i=0;
    char slice[10][10];
    int j=1;
    int t;
    int cur = 0;
    memset(slice,'\0',10);
//	printf("path:%s\n",path);
//	for(int k=0;k<strlen(path);k++)
//	   printf("%c\n",path[k]);
    qDebug()<<"nima1.1.1.1";
    if(strcmp(path,"/")==0)
    {
        strcpy(slice[0],"root");
        qDebug()<<"nima1.1.1.1********"<<FI.index[0].fcb->childnum;
        for(t=0;t<FI.index[0].fcb->childnum;t++)
        {
             if(strcmp(FI.index[0].fcb->child[t],name)==0)
             {
                   return 1;   //发现该目录下的重名文件
             }
        }
        qDebug()<<"nima1.1.1.1";

        if(t==FI.index[0].fcb->childnum)  //路径及文件名均符合要求，将文件系统当前状态进行修改
        {
              /*
              CS.filelevel = 1;
              CS.currparent = FI.index[0].fcb;
              strcpy(CS.currentpath,path);
              */
              return 0;
        }

    }
    else
    {
        strcpy(slice[0], "root");
        i++;
    }
    qDebug()<<"nima1.1.1.2";
    while(path[i]!='\0')   //将路径分解，存入slice数组中
    {
        t = 0;
        while(path[i]!='/' && path[i]!='\0')
        {

            slice[j][t] = path[i];
            t++;
            i++;
        }

        slice[j][t] = '\0';
        if(path[i]=='\0')
            break;
        else
        {
            i++;
            j++;   //用j可以表示路径的层级
        }
    }
    //qDebug()<<"nima1.1.1.1";
 //   printf("path:%d\n",j);
 //   for(int k=0;k<=j;k++)
 //      printf("%d:%s\n",k,slice[k]);

    //qDebug()<<"~~~~~~~!!!!!!!";
    for(t=0;t<=j;t++)   //层级
    {
        //qDebug()<<"~~~~~~~!!!!!!!" << t;
        for(i=cur;i<FI.indexcount;i++)
        {
            //qDebug()<<"2222~~~~~~~!!!!!!!" << i;
            if((strcmp(FI.index[i].filename,slice[t])==0)&& (FI.index[i].filelevel == t) && (FI.index[i].filetype == dir) && (FI.index[i].effect== 1))
            {
                if(t!= 0)   //不是根目录
                {
                    if(strcmp(FI.index[i].parentfilename,slice[t-1])==0) //自身和父节点均吻合，跳出该循环，进行下一级查找
                    {
                        cur = i;
                        break;
                    }
                }
                else    //根目录，跳出该循环，进行下一级查找
                {
                    cur = i;
                    break;
                }
            }
        }
        if(i == FI.indexcount)
        {
            //qDebug()<<"3333~~~~~~~!!!!!!!" << i;
            return 0;
        }
    }
    //此时的i标注了对应的目录索引号，j对应了其所在层级+1

    qDebug()<<"~~~~~~~!!!!!!!~~~~~~~~~~";
    for(t=0;t<FI.index[i].fcb->childnum;t++)
    {
         qDebug()<<"~~~~~~"<< FI.index[i].fcb->child[t];
         qDebug()<<"~~!!!!~~"<< name;
         if(strcmp(FI.index[i].fcb->child[t],name)==0)
         {

               return 1;   //发现该目录下的重名文件
         }
    }

    if(t==FI.index[i].fcb->childnum)  //路径及文件名均符合要求，将文件系统当前状态进行修改
    {
        /*
        CS.filelevel = j+1;
        CS.currparent = FI.index[i].fcb;
        strcpy(CS.currentpath,path);
        */
        return 0;
    }

}

int Createfile(FileAccess acc,const char* filename,FileType type,int filesize)  //创建文件,权限、文件名、类型、父节点
{
    int blocknum;  //用来存放文件所需的文件块数目
    int i;
    int t;
    int block[50];  //用来存放分配的文件块列表
    FCB* filecb;
    memset(block,0,10);  //初始化为0

    //if(strcmp(filename,"root")!= 0)
    //   printf("CS.currparent->facc:%d\n",CS.currparent->facc);

    if( strcmp(filename,"root")==0 || CS.currparent->facc != 0 )
    {

        filecb = (FCB*)malloc(sizeof(FCB));
        if(filecb == NULL)
        {
           return 1;  //FCB空间分配失败
        }
        else   //分配FCB空间成功
        {
            filecb->fname[0]='\0';
            filecb->parent[0]='\0';

            strcpy(filecb->fname,filename);

            //printf("fnamelen:%d  filenamelen:%d\n",strlen(filecb->fname),strlen(filename));
            //printf("fname:%s  filename:%s\n",filecb->fname,filename);
            if(strcmp(filename,"root")==0)  //如果是根目录
            {
                filecb->parent[0] ='\0';
                filecb->flevel = 0;
            }
            else   //普通文件
            {
                filecb->flevel = CS.filelevel;
                strcpy(filecb->parent,CS.currparent->fname);
            }
            //printf("filecb->parent:%s\n",filecb->parent);
            qDebug()<<"filecb->parent:"<<filecb->parent;
            filecb->ftype = type;
            filecb->facc = acc;
            filecb->fstate = closed;
            memset(filecb->child,'\0',10);
            filecb->childnum = 0;

            GetCurTime(filecb->createtime);
            strcpy(filecb->lastmodtime,filecb->createtime);

            if(filecb->ftype == file)
            {

                filecb->flen = filesize;
                i = filesize/1024;   //计算文件所需物理块
                t = filesize%1024;
                if(t != 0)
                    i++;
                //printf("************************i:%d\n",i);
                for(t=0;t<i;t++)  //依次为文件分配物理块
                {
                    block[t] = FindBlankBlockid();
                    //printf("block[%d]:%d\n",t,block[t]);
                    if(block[t]>=BlockCount)
                    {
                        return 1;
                    }
                    else
                    {
                        UpdateBitMap(0,block[t]);
                    }
                }
                if(t==i)
                {
                    for(t=0;t<i;t++)
                        filecb->blocknum[t] = block[t];
                }
            }
            else
            {
                filecb->flen = 10;
                memset(filecb->blocknum,0,10);
                //CS.filelevel++;
                /*
                if(filename == "root")
                {
                    CS.currentpath[0] = '/';   //意为根目录
                }
                else
                {
                    if(strcmp(filecb->parent,"root")==0)
                    {
                        strcat(CS.currentpath,filename);
                    }
                    else
                    {
                        strcat(CS.currentpath,"/");
                        strcat(CS.currentpath,filename);
                    }
                }
                */
            }

            if(filecb->parent[0] != '\0')  //不是根目录
            {
                qDebug()<<"CS.currparent->childnum:"<<CS.currparent->childnum<<endl;
                strcpy(CS.currparent->child[CS.currparent->childnum],filename);
                // printf("CS.currparent->child[CS.currparent->childnum]:%s\n",CS.currparent->child[CS.currparent->childnum]);
                CS.currparent->childnum++;
            }
            AddIndex(filename,(FileType)type,filecb);
            /*
            if(type == dir)
            {
                 CS.currparent = filecb;
            }
            */
         }
    }
    else
    {
        fprintf(fmlog,"该目录为只读目录，不可在此创建文件！\n");
        qDebug()<<"该目录为只读目录，不可在此创建文件！"<<endl;
    }
    qDebug()<<"*&&&&&&&&&&&&&&&&&&&&&&&&"<<FI.index[0].fcb->childnum;
    return 0;
}

int FindBlankBlockid()  //寻找空闲文件块
{
    int i;
    for(i=0;i<BlockCount;i++)
    {
        if(bitmap[i]==0)
          break;
    }
    return i;
}
void AddIndex(const char* name,FileType type,FCB* filecb)  //添加索引
{
    FI.index[FI.indexcount].indexnum = FI.indexcount;
    strcpy(FI.index[FI.indexcount].filename,name);
    if(CS.currparent == NULL)
        FI.index[FI.indexcount].parentfilename[0] = '\0';
    else
        strcpy(FI.index[FI.indexcount].parentfilename,CS.currparent->fname);
    if(type == dir)
    {
        //FI.index[FI.indexcount].filelevel = CS.filelevel-1;
        FI.index[FI.indexcount].filelevel = CS.filelevel;
    }
    else
    {
        FI.index[FI.indexcount].filelevel = CS.filelevel;
    }
    FI.index[FI.indexcount].effect = 1;
    FI.index[FI.indexcount].filetype = type;
    FI.index[FI.indexcount].fcb = filecb;
    FI.index[FI.indexcount].fcb->feffect = 1;

    FI.indexcount++;
    for(int i =0;i<FI.indexcount;i++)
    {
        qDebug()<<FI.index[i].indexnum<<"index:"<<FI.index[i].filename;
    }
}
void UpdateBitMap(int slg,int blockid)  //更新位示图,slg为0：添加，slg为1：删除
{
    if(slg == 0)  //添加
        bitmap[blockid] = 1;
    else
        bitmap[blockid] = 0;   //删除
}

void GetCurTime(char* currtime) //获取当前时间
{
    /*
    char dbuffer[9];
    char tbuffer[9];
    _strdate(dbuffer);
    _strtime(tbuffer);
    strcpy(currtime,dbuffer);
    strcat(currtime," ");
    strcat(currtime,tbuffer);
    */

    char buffer[30];
    time_t now = time(NULL);
    strftime(buffer, 20, "%Y-%m-%d %I:%M:%S", localtime(&now));
    strcpy(currtime,buffer);
}

int Search(const char* name,const char* path)   //查找指定文件
{
    int i=0;
    char slice[10][10];  //用来存放分解路径
    int j=1;
    int t;
    int cur = 0;
    memset(slice,'\0',10);
    fse* fparent;

    if(strcmp(path,"/")==0)  //根目录下
    {
        strcpy(slice[0],"root");
        int k;
        for(k=0;k<FI.indexcount;k++)
        {
            /*
            cout << "!!!!" <<FI.index[k].parentfilename << endl;
            cout << "!!!" <<FI.index[k].filename << endl;
            cout << "!!" <<FI.index[k].filelevel << endl;
            cout << "!" <<FI.index[k].effect << endl;
            */
            if((strcmp(FI.index[k].parentfilename,"root")==0) && (strcmp(FI.index[k].filename,name)==0) && (FI.index[k].filelevel ==1) &&(FI.index[k].effect == 1))
            {

                /*
                CS.filelevel = 1;
                CS.currparent = FI.index[0].fcb;
                CS.currentpath[0] = '/';
                */
                return k;
            }
        }
        if(k == FI.indexcount)   //表示该文件不存在
            return -1;
    }
    else
    {
        strcpy(slice[0],"root");
        i++;
    }

    while(path[i]!='\0')   //将路径分解，存入slice数组中
    {
        t = 0;
        while (path[i]!='/' && path[i]!='\0')
        {
            slice[j][t] = path[i];
            t++;
            i++;
        }
        slice[j][t] = '\0';

        if(path[i]=='\0')

            break;
        else
        {
             i++;
             j++;   //用j可以表示路径的层级
        }
    }

    j--;
    qDebug()<<"path:"<<j<<endl;
    for(int k=0;k<=j;k++)
       qDebug()<<k<<":"<<slice[k]<<endl;


    for(t=0;t<=j;t++)
    {
        for(i=cur;i<FI.indexcount;i++)
        {
            cout << "search~~~!" << FI.index[i].filelevel <<endl;
            if((strcmp(FI.index[i].filename,slice[t])==0)&& (FI.index[i].filelevel == t) && (FI.index[i].filetype == dir) && (FI.index[i].effect== 1))
            {
                if(t!= 0)
                {
                    if(strcmp(FI.index[i].parentfilename,slice[t-1])==0)  //每找到一个跳出小循环，继续下一个层级的寻找
                    {
                        cur = i;
                        break;
                    }
                }
                else
                    break;
            }
        }
        if(i == FI.indexcount)
        {
            return -2;   //表示目录错误
            //return i;
        }
        cur = 0;
    }

    qDebug()<<"index["<<i<<"].filename:"<<endl;
    fparent = FI.index[i].fcb;
    //printf("fparent:%s   %d   indexcount:%d  level:%d\n",fparent->fname,i,FI.indexcount,j);
    //此时的i标注了对应的目录索引号，j对应了其所在层级
    for(;i<FI.indexcount;i++)
    {
         //路径及文件名均符合要求，将文件系统当前状态进行修改
         qDebug()<<"index["<<i<<"].filename:"<<FI.index[i].filename<<endl;
         //if((strcmp(FI.index[i].filename,name)==0)&& (FI.index[i].filelevel == j+1) && (FI.index[i].filetype == file) && (FI.index[i].effect== 1) && (strcmp(FI.index[i].parentfilename,fparent->fname)==0))
         if((strcmp(FI.index[i].filename,name)==0)&& (FI.index[i].filelevel == j+1) && (FI.index[i].effect== 1) && (strcmp(FI.index[i].parentfilename,fparent->fname)==0))
         {
                /*
                CS.filelevel = j;
                CS.currparent = fparent;
                strcpy(CS.currentpath,path);
                */
                return i;
         }
    }
    if(i == FI.indexcount)
       return -1;   //表示该文件不存在
}

void Open(int pid,string fname,string fpath)   //打开文件
{
    int slg;
    int i,j;
    const char* name;
    const char* path;

    name = fname.c_str();
    path = fpath.c_str();
    fprintf(fmlog,"Open:   filename:%s   path:%s\n",name,path);

    Operation op;
    FileBlock fb[32];
    if(strcmp(name," ")==0)
    {
        qDebug()<<"对不起，文件名不能为空.";
        fprintf(fmlog,"对不起，文件名不能为空.\n");
    }
    else
    {

        slg=Search(name,path);

        if(slg == -1)
        {
            qDebug()<<"该文件不存在!";
            fprintf(fmlog,"该文件不存在!\n");
        }
        else if(slg == -2)
        {
            qDebug()<<"目录错误!";
            fprintf(fmlog,"目录错误!\n");
        }
        else
        {
            FI.index[slg].fcb->fstate = opened;
             op.PID = pid;
             op.time = 0;
             op.oper = 1;
             op.protection = FI.index[FI.indexcount-1].fcb->facc;
             i = FI.index[FI.indexcount-1].fcb->flen/1024;
             j = FI.index[FI.indexcount-1].fcb->flen%1024;
             if(j!=0)
                 i++;
             op.block = i;
             fb[0].start = 0;
             fb[0].PID = pid;
             fb[0].filename = name;
             fb[0].block = i;
             fb[0].oper = 0;
             for(j=0;j<i;j++)
             {
                  fb[0].PhyAddress[j] =FI.index[FI.indexcount-1].fcb->blocknum[j];
                   qDebug()<<FI.index[FI.indexcount-1].fcb->blocknum[j];
             }
             Allocator(op,fb[0]);
        }
    }
    qDebug()<<"当前路径:"<<CS.currentpath<<endl;
    qDebug()<<FI.index[slg].fcb->fname<<"  "<<FI.index[slg].fcb->fstate<<endl;
}


int Write(int pid,string fname,string fpath,int size)   //写文件
{

    int slg;
    int t1,t2,y1,y2,c,i,j;
    int block;
    const char* name;
    const char* path;

    name = fname.c_str();
    path = fpath.c_str();
    fprintf(fmlog,"Write:   filename:%s   path:%s\n",name,path);

    Operation op;
    FileBlock fb[32];
    if(strcmp(name," ")==0)
    {
        fprintf(fmlog,"对不起，文件名不能为空.\n");
        qDebug()<<"对不起，文件名不能为空.";
        return 0;
    }
    else
    {
        slg=Search(name,path);
        if(slg == -1)
        {
            fprintf(fmlog,"该文件不存在@\n");
            qDebug()<<"该文件不存在!";
            return 0;
        }
        else if(slg == -2)
        {
            fprintf(fmlog,"目录错误!\n");
            qDebug()<<"目录错误!";
            return 0;
        }
        else
        {
            if(FI.index[slg].fcb->facc != 0)
            {
                 qDebug()<<"write1.1~~~~~~"<<FI.index[slg].fcb->facc;
                 FI.index[slg].fcb->fstate = writing;
                 t1 = FI.index[slg].fcb->flen/1024;
                 y1 = FI.index[slg].fcb->flen%1024;
                 if(y1!=0)
                     t1++;   //原文件占物理块大小
                 t2 = size/1024;
                 y2 = size%1024;
                 if(y2!=0)
                     t2++;   //修改文件占物理块大小
                 if(t1>t2)  //模拟文件中部分内容被删除
                 {
                     for(i=t2;i<t1;i++)
                     {
                         UpdateBitMap(1,FI.index[slg].fcb->blocknum[i]);  //删除部分占用的物理块
                     }
                 }
                 else if(t1<t2)
                {
                     for(i=t1;i<t2;i++)
                     {
                         block = FindBlankBlockid();
                         if(block>=BlockCount)
                         {
                             fprintf(fmlog,"物理块空间不足，写文件失败！\n");
                             qDebug()<<("物理块空间不足，写文件失败！\n");
                             return 0;
                         }
                         else
                         {
                             FI.index[slg].fcb->blocknum[i] = block;
                             UpdateBitMap(0,block);   //添加部分占用的物理块
                         }
                     }
                 }
                 FI.index[slg].fcb->flen = size;
                 GetCurTime(FI.index[slg].fcb->lastmodtime);

                 op.PID = pid;
                 op.time = 0;
                 op.oper = 1;
                 op.protection = FI.index[FI.indexcount-1].fcb->facc;

                 op.block = t2;
                 fb[0].start = 0;
                 fb[0].PID = pid;
                 fb[0].filename = name;
                 fb[0].block = i;
                 fb[0].oper = 0;
                 for(j=0;j<t2;j++)
                     fb[0].PhyAddress[j] =FI.index[FI.indexcount-1].fcb->blocknum[j];
                 Allocator(op,fb[0]);
            }
            else
            {
                fprintf(fmlog,"该文件为只读文件，不可进行写操作!\n");
                qDebug()<<"该文件为只读文件，不可进行写操作!";
                return 0;
            }

        }
    }
    qDebug()<<"当前路径:"<<CS.currentpath<<endl;
    return 1;
    //qDebug()<<QString(FI.index[slg].fcb->fname)<<"  "<<FI.index[slg].fcb->flen<"   "<<FI.index[slg].fcb->fstate<<endl;
}
void Close(int pid,string fname,string fpath)  //关闭文件
{

    int slg;
    int i,j;
    Operation op;
    FileBlock fb[32];
    const char* name;
    const char* path;

    name = fname.c_str();
    path = fpath.c_str();

     fprintf(fmlog,"Close:   filename:%s   path:%s\n",name,path);

    if(strcmp(name," ")==0)
    {
        fprintf(fmlog,"对不起，文件名不能为空.\n");
        qDebug()<<"对不起，文件名不能为空.";
    }
    else
    {
        slg=Search(name,path);
        if(slg == -1)
        {
            fprintf(fmlog,"该文件不存在!\n");
            qDebug()<<"该文件不存在!";
        }
        else if(slg == -2)
        {
            fprintf(fmlog,"目录错误!\n");
            qDebug()<<"目录错误!";
        }
        else
        {
            qDebug()<<"closed~~当前路径:"<<CS.currentpath<<endl;
            FI.index[slg].fcb->fstate = closed;
            op.PID = pid;
            op.time = 0;
            op.oper = 1;
            op.protection = FI.index[FI.indexcount-1].fcb->facc;
            i = FI.index[FI.indexcount-1].fcb->flen/1024;
            j = FI.index[FI.indexcount-1].fcb->flen%1024;
            if(j!=0)
                 i++;
            op.block = i;
            fb[0].start = 0;
            fb[0].PID = pid;
            fb[0].filename = name;
            fb[0].block = i;
            fb[0].oper = 1;
            for(j=0;j<i;j++)
                fb[0].PhyAddress[j] =FI.index[FI.indexcount-1].fcb->blocknum[j];
            Allocator(op,fb[0]);
        }
    }
    qDebug()<<"当前路径:"<<CS.currentpath<<endl;
    qDebug()<<FI.index[slg].fcb->fname<<FI.index[slg].fcb->fstate<<endl;
}
void Delete(int pid,string fname,string fpath)   //删除文件（其索引还在，但是表示为无效）
{
    qDebug()<<"nima5.0";
    int i,j;
    int k;
    int slg;
    int x,y;
    Operation op;
    FileBlock fb[32];
    const char* name;
    const char* path;

    name = fname.c_str();
    path = fpath.c_str();

    fprintf(fmlog,"Delete:   filename:%s   path:%s\n",name,path);
    qDebug()<<"nima5.1";
    if(strcmp(name," ")==0)
    {
        fprintf(fmlog,"对不起，文件名不能为空.\n");
        qDebug()<<"对不起，文件名不能为空."<<endl;
    }
    else
    {
        slg=Search(name,path);
        if(slg == -1)
        {
            fprintf(fmlog,"该文件不存在!\n");
            qDebug()<<"该文件不存在!";
        }
        else if(slg == -2)
        {
            fprintf(fmlog,"目录错误!\n");
            qDebug()<<"目录错误!";
        }
        else
        {
            qDebug()<<"nima5.2";
            if(CS.currparent->facc != 0)
            {
                FI.index[slg].effect = 0;
                FI.index[slg].fcb->feffect = 0;
                x = FI.index[slg].fcb->flen/1024;
                y = FI.index[slg].fcb->flen%1024;
                if(y!=0)
                     x++;
                for(j=0;j<x;j++)
                {
                     UpdateBitMap(1,FI.index[slg].fcb->blocknum[j]);
                }
                qDebug()<<"nima5.3";
                for(j=0;j<CS.currparent->childnum;j++)
                {
                     if(CS.currparent->child[j] == name)
                         break;
                }
                //qDebug()<<"##################"<<FI.index[0].fcb->childnum;
                for(;j<CS.currparent->childnum;j++)  //j dagnqian wenjian index xiabiao
                    for(k=0;k<20;k++)
                       CS.currparent->child[j][k]=CS.currparent->child[j+1][k];
                CS.currparent->childnum--;
                //qDebug()<<"*****************"<<FI.index[0].fcb->childnum;
                //qDebug()<<"nima5.4";
                FI.index[slg].fcb->fstate = closed;
                op.PID = pid;
                op.time = 0;
                op.oper = 1;
                op.protection = FI.index[ FI.indexcount-1].fcb->facc;

                if(j!=0)
                     i++;
                op.block = x;
                fb[0].start = 0;
                fb[0].PID = pid;
                fb[0].filename = name;
                fb[0].block = x;
                fb[0].oper = 1;
                for(j=0;j<x;j++)
                     fb[0].PhyAddress[j] =FI.index[ FI.indexcount-1].fcb->blocknum[j];
                //qDebug()<<"nima5.5";
                Allocator(op,fb[0]);
            }
            else
            {
                fprintf(fmlog,"该目录为只读目录，不可删除文件!\n");
                qDebug()<<"该目录为只读目录，不可删除文件!"<<endl;
            }
        }
    }
    qDebug()<<"当前路径:"<<CS.currentpath<<endl;
}

void printfminfo()  //打印已有信息
{
        fprintf(fmlog,"===================================================\n");
        qDebug()<<"===================================================="<<endl;
        fprintf(fmlog,"当前文件系统内容：\n");
        qDebug()<<"当前文件系统内容:"<<endl;
        for(int i=0;i<FI.indexcount;i++)
        {

            qDebug()<<"FileIndex:"<<endl;
            qDebug()<<"indexnum:"<<FI.index[i].indexnum<<endl;
            qDebug()<<"filename:"<<FI.index[i].filename<<endl;
            qDebug()<<"parentfilename:"<<FI.index[i].parentfilename<<endl;
            qDebug()<<"filelevel:"<<FI.index[i].filelevel<<endl;
            qDebug()<<"effect:"<<FI.index[i].effect<<endl;
            qDebug()<<"filetype:"<<FI.index[i].filetype<<endl;
            qDebug()<<"--------------------------"<<endl;

            fprintf(fmlog,"--------------------------\n");
            fprintf(fmlog,"FCB:\n");
            fprintf(fmlog,"flevel:%d\n",FI.index[i].fcb->flevel);
            fprintf(fmlog,"fname:%s\n",FI.index[i].fcb->fname);
            fprintf(fmlog,"parent:%s\n",FI.index[i].fcb->parent);
            fprintf(fmlog,"block:");
            if(FI.index[i].fcb->flen%1024==0)
            {
                for(int j=0;j<FI.index[i].fcb->flen/1024;j++)
                      fprintf(fmlog,"%d ",FI.index[i].fcb->blocknum[j]);
            }
            else
            {
                for(int j=0;j<(FI.index[i].fcb->flen/1024)+1;j++)
                      fprintf(fmlog,"%d ",FI.index[i].fcb->blocknum[j]);
            }
            fprintf(fmlog,"\nflen:%d\n",FI.index[i].fcb->flen);
            fprintf(fmlog,"ftype:%d\n",FI.index[i].fcb->ftype);
            fprintf(fmlog,"facc:%d\n",FI.index[i].fcb->facc);
            fprintf(fmlog,"feffect:%d\n",FI.index[i].fcb->feffect);
            fprintf(fmlog,"createtime:%s\n",FI.index[i].fcb->createtime);
            fprintf(fmlog,"lastmodtime:%s\n",FI.index[i].fcb->lastmodtime);
            fprintf(fmlog,"fstate:%d\n",FI.index[i].fcb->fstate);
            fprintf(fmlog,"childnum:%d\n",FI.index[i].fcb->childnum);
            fprintf(fmlog,"child:\n");
            for(int j=0;j<FI.index[i].fcb->childnum;j++)
                  fprintf(fmlog,"%d:%s\n",j,FI.index[i].fcb->child[j]);
        }
        fprintf(fmlog,"=====================================================\n");
        qDebug()<<"----------------------------------------------------"<<endl;
        qDebug()<<"当前层级:"<<CS.filelevel<<endl;
        qDebug()<<"当前父节点:"<<CS.currparent->fname<<endl;
        qDebug()<<"当前路径:"<<CS.currentpath<<endl;
        qDebug()<<"----------------------------------------------------"<<endl;
}
