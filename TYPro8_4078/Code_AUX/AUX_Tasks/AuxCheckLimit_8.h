#pragma once

class AuxCheckLimit8_task
{
public:
    static UInt16      TaskID;                                                                       //����ID
    static UInt16      Priority;                                                                     //���ȼ�
public:
    static void        DoWork(void);                                                                 //����ִ�к���
    static void        MyInit(void);                                                                 //�����ʼ��
    static void        EndTask(void);                                                                //�������
};


