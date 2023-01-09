#pragma once

class AuxCheckLimit8_task
{
public:
    static UInt16      TaskID;                                                                       //任务ID
    static UInt16      Priority;                                                                     //优先级
public:
    static void        DoWork(void);                                                                 //任务执行函数
    static void        MyInit(void);                                                                 //任务初始化
    static void        EndTask(void);                                                                //任务结束
};


