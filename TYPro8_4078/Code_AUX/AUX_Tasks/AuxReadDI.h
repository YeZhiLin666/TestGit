#pragma once


class Aux_ReadDI
{
public:
    static unsigned short                 Priority;
    static unsigned short               TaskID;
    static void (*m_ReadDI)(void);
    static void (*m_InitDI)(void);
private:

public:
    static void Init(void);
    static void DoWork(void);

};
