///////////////////////////////////////////////////////////////////////////////////
//File Name: AUXSampleTask.h
//Version:		V1.0
//Date:				2013/11/18
//Description:
//						辅助普通采样任务处理头文件
//Others:      dirui
//History：

///////////////////////////////////////////////////////////////////////////////////
#ifndef _AUXSAMPLETASK_
#define _AUXSAMPLETASK_
class AUX_Sample_4_task
{
public:
    static UInt16            Priority;
    static UInt16            TaskID;
public:

    static void Init(void);
    static void DoWork(void);
    static void EndTask(void);


private:


};
#endif
