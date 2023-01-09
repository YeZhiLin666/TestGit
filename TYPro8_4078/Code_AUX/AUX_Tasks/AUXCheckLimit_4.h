#pragma once

#define     LIMITCHECK_COUNT           4
#define     IFANY_LIMITCHECK_COUNT     1

typedef struct
{
    unsigned char m_IVunitBufIndex;  //防止和真正的IV_unitID混淆
    unsigned char m_IVch;
    unsigned char m_limitIdx;
} ST_LIMIT_POINTER;

class AuxCheckLimit4_task
{
public:
    static UInt16            Priority;
    static UInt16            TaskID;

    static void              DoWork(void);
    static void              MyInit(void);
    static void              EndTask(void);
    static void              Reset(void);
    static void              Limit_Check(unsigned char MaxLimitCount);
    static void              IfAny_Limit_Check(unsigned char MaxLimitCount);

private:
    static int               m_CheckedLimitCount;
    static int               m_CheckedAnyLimitCount;
    static ST_LIMIT_POINTER  m_PresentLimit;
    static ST_LIMIT_POINTER  m_PresentAnyLimit;
    static signed char       FindNextLimit(void);
    static signed char       FindNextAnyLimit(void);
};

