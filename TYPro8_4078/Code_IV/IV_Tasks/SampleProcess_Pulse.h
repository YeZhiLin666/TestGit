#pragma once

typedef struct
{
    unsigned char   StartIdx;
    unsigned char   EndIdx;
} St_SampPtr;

class SampleProcess_Pulse
{
public:
    static UInt16 Priority;
    static UInt16 TaskID;

    static bool       NewCodeSampled;

    static void   Init(void);
    static void   DoWork(void);
    static void   ConvertPulseStageData(void);
    static void   EndTask(void);
};

