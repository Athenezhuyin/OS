#ifndef FILE_H
#define FILE_H

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<string>
using namespace std;

//文件元素可供操作的权限
typedef enum
{
    readonly,   //只读
    readwrite    //可写
}FileAccess;

typedef enum
{
    file,   //文件
    dir   //目录
}FileType;

typedef enum
{
    closed,
    opened,
    writing
}FileState;

//一个文件索引的结构
typedef struct
{
    int indexnum;  //索引序号
    char filename[20];   //文件名
    char parentfilename[20];   //父节点文件名
    int filelevel;   //文件所在层级
    int effect;  //文件有效性
    int filetype;   //文件属性
    struct fse* fcb;   //指向自身的FCB
} FileIndexElement;

//文件系统索引结构
typedef struct
{
    FileIndexElement index[100];  //索引数组
    int indexcount;   //现有的索引数量
} FileIndex;

//文件系统中的元素结构，包括文件和目录
typedef struct fse
{
    int flevel;  //文件所在的层次，层+文件元素名为一个文件的逻辑位置
    char fname[20];  //文件名
    char parent[20];  //父文件名
    int blocknum[50];  //文件所在的物理块编号
    int flen;  //文件长度
    FileType ftype;  //文件类型
    FileAccess facc;  //文件权限
    int feffect;  //表征文件是否被删除，0表示无效文件，1表示有效
    char createtime[20]; //创建时间
    char lastmodtime[20]; //最后一次修改时间
    FileState fstate;  //文件当前状态
    char child[10][20];  //如果该文件为目录，则数组中存放其后继的子文件；如果是普通文件，则该数组为空
    int childnum;  //子文件数目
} FCB;
//系统当前状态
typedef struct
{
    int filelevel;  //当前所在文件系统层
    FCB* currparent;  //当前层的父节点
    char* currentpath;  //当前路径
} CState;

const int FileSysSize = 1024*512;   //文件系统物理空间大小 512K
const int BlockSize = 1024;   //块大小1K
const int BlockCount = 512;   //块数量512,物理空间512K
const int BitMapLen = 512;   //位示图长度
const int CommandLen = 50;   //用户命令最大长度
const int CommandCount = 8;   //预设用户命令数


extern int bitmap[512];   //位示图，0表示空闲，1表示占用
extern char* filesys ;    //文件系统物理空间地址
extern CState CS;  //当前系统状态
extern FileIndex FI;  //系统索引
extern FILE* fmlog;  //日志文件
extern char logtime[20];  //日志时间


#endif // FILE_H
