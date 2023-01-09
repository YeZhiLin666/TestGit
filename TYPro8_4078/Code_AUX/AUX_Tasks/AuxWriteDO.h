#pragma once


class Aux_WriteDO
{
public:
    static unsigned short                 Priority;
    static unsigned short                 TaskID;
    static void (*m_WriteDO)(void);
    static void (*m_InitDO)(void);

private:

public:
    static void Init(void);
    static void DoWork(void);

};
