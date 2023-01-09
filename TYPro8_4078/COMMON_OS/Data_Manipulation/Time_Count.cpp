#include "includes.h"

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x03E80000
#endif

#if THREAD_TASK_TIME_STATISTIC == 1
struct task_time_count_t thread_time_count_st[MAX_TASK_COUNT];
struct task_time_count_t function_time_count_st[MAX_FUNCTION_COUNT];
unsigned char time_count_reset_flag;//���ú�ʱͳ�Ʊ�־
volatile unsigned long long timer_overflow_count;//����ͳ�ƶ�ʱ����(�ж�)������
#endif

#pragma arm section //��������������


#if THREAD_TASK_TIME_STATISTIC == 1
/**
  * ���ܣ����������ʱ�ͺ�����ʱͳ��
	*	��������
  */
void Time_Count::time_reset(void)
{
    int i;

    memset(thread_time_count_st,0,sizeof(thread_time_count_st));
    memset(function_time_count_st,0,sizeof(function_time_count_st));

    for(i = 0; i < MAX_TASK_COUNT; i++)
    {
        thread_time_count_st[i].us_min = 0xffff;
    }

    for(i = 0; i < MAX_FUNCTION_COUNT; i++)
    {
        function_time_count_st[i].us_min = 0xffff;
    }
}

/**
  * ���ܣ������ʱͳ�Ƴ�ʼ�������������������
	*	������task_num �����
  */
void Time_Count::thread_time_init(unsigned char task_num)
{
    thread_time_count_st[task_num].timer_last_count = TIMER_COUNT_REGISER;
    thread_time_count_st[task_num].timer_overflow_count_start = timer_overflow_count;
}

/**
  * ���ܣ�������ʱͳ�Ƴ�ʼ�����������ں������
	*	������task_num �Զ�������function_time_count_st�±�
  */
void Time_Count::function_time_init(unsigned char task_num)
{
    function_time_count_st[task_num].timer_last_count = TIMER_COUNT_REGISER;
    function_time_count_st[task_num].timer_overflow_count_start = timer_overflow_count;
}

/**
  * ���ܣ������ʱͳ�ƺ�������������ĩβ
	*	������task_num �����
  */
void Time_Count::thread_time(unsigned char task_num)
{
    if(task_num >= MAX_TASK_COUNT || task_num < 0)
        return;

    thread_time_count_st[task_num].timer_overflow_count_updata = timer_overflow_count;

    long timer_updata_us_count = TIMER_COUNT_REGISER + (thread_time_count_st[task_num].timer_overflow_count_updata - thread_time_count_st[task_num].timer_overflow_count_start) * (TIMER_COUNT_REGISER_MAX - 1);
    long temp_time = timer_updata_us_count - thread_time_count_st[task_num].timer_last_count;

    if(temp_time < 0)
    {
        temp_time += TIMER_COUNT_REGISER_MAX - 1;
    }

    if(thread_time_count_st[task_num].count < TASK_TIME_COUNT_BUF_MAX)//δ��ƽ��ֵ
    {
        thread_time_count_st[task_num].buf[thread_time_count_st[task_num].offset] = temp_time;
        thread_time_count_st[task_num].sum += thread_time_count_st[task_num].buf[thread_time_count_st[task_num].offset];
    }
    else//�ﵽƽ��ֵ
    {
        thread_time_count_st[task_num].temp = thread_time_count_st[task_num].buf[thread_time_count_st[task_num].offset];
        thread_time_count_st[task_num].buf[thread_time_count_st[task_num].offset] = temp_time;
        thread_time_count_st[task_num].sum += thread_time_count_st[task_num].buf[thread_time_count_st[task_num].offset] - thread_time_count_st[task_num].temp;
    }

    if(thread_time_count_st[task_num].us_max < thread_time_count_st[task_num].buf[thread_time_count_st[task_num].offset])
    {
        thread_time_count_st[task_num].us_max = thread_time_count_st[task_num].buf[thread_time_count_st[task_num].offset];
    }

    if(thread_time_count_st[task_num].us_min > thread_time_count_st[task_num].buf[thread_time_count_st[task_num].offset])
    {
        thread_time_count_st[task_num].us_min = thread_time_count_st[task_num].buf[thread_time_count_st[task_num].offset];
    }

    if(thread_time_count_st[task_num].count < TASK_TIME_COUNT_BUF_MAX)
        thread_time_count_st[task_num].count++;

    thread_time_count_st[task_num].us_avarage = thread_time_count_st[task_num].sum/thread_time_count_st[task_num].count;

    if(++thread_time_count_st[task_num].offset >= TASK_TIME_COUNT_BUF_MAX)
        thread_time_count_st[task_num].offset = 0;

    if(time_count_reset_flag)
    {
        time_count_reset_flag = 0;
        Time_Count::time_reset();
    }
}

/**
  * ���ܣ�������ʱͳ�ƺ��������ں���ĩβ
	*	������task_num �Զ�������function_time_count_st�±�
  */
void Time_Count::function_time(unsigned char task_num)
{
    if(task_num >= MAX_FUNCTION_COUNT || task_num < 0)
        return;

    function_time_count_st[task_num].timer_overflow_count_updata = timer_overflow_count;

    long timer_updata_us_count = TIMER_COUNT_REGISER + (function_time_count_st[task_num].timer_overflow_count_updata - function_time_count_st[task_num].timer_overflow_count_start) * (TIMER_COUNT_REGISER_MAX - 1);
    long temp_time = timer_updata_us_count - function_time_count_st[task_num].timer_last_count;

    if(temp_time < 0)
    {
        temp_time += TIMER_COUNT_REGISER_MAX - 1;
    }

    if(function_time_count_st[task_num].count < TASK_TIME_COUNT_BUF_MAX)//δ��ƽ��ֵ
    {
        function_time_count_st[task_num].buf[function_time_count_st[task_num].offset] = temp_time;
        function_time_count_st[task_num].sum += function_time_count_st[task_num].buf[function_time_count_st[task_num].offset];
    }
    else//�ﵽƽ��ֵ
    {
        function_time_count_st[task_num].temp = function_time_count_st[task_num].buf[function_time_count_st[task_num].offset];
        function_time_count_st[task_num].buf[function_time_count_st[task_num].offset] = temp_time;
        function_time_count_st[task_num].sum += function_time_count_st[task_num].buf[function_time_count_st[task_num].offset] - function_time_count_st[task_num].temp;
    }

    if(function_time_count_st[task_num].us_max < function_time_count_st[task_num].buf[function_time_count_st[task_num].offset])
    {
        function_time_count_st[task_num].us_max = function_time_count_st[task_num].buf[function_time_count_st[task_num].offset];
    }

    if(function_time_count_st[task_num].us_min > function_time_count_st[task_num].buf[function_time_count_st[task_num].offset])
    {
        function_time_count_st[task_num].us_min = function_time_count_st[task_num].buf[function_time_count_st[task_num].offset];
    }

    if(function_time_count_st[task_num].count < TASK_TIME_COUNT_BUF_MAX)
        function_time_count_st[task_num].count++;

    function_time_count_st[task_num].us_avarage = function_time_count_st[task_num].sum/function_time_count_st[task_num].count;

    if(++function_time_count_st[task_num].offset >= TASK_TIME_COUNT_BUF_MAX)
        function_time_count_st[task_num].offset = 0;

    if(time_count_reset_flag)
    {
        time_count_reset_flag = 0;
        Time_Count::time_reset();
    }
}

#endif
