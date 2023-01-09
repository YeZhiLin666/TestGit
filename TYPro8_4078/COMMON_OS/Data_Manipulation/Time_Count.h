#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h> 
#include <stdint.h>
#include <float.h>

#include "includes.h"
#include "../Task_Dispatcher/OS_ServeProcess.h"

#if THREAD_TASK_TIME_STATISTIC == 1

class Time_Count
{
public:
		static void time_reset(void);
		static void thread_time_init(unsigned char task_num);
		static void function_time_init(unsigned char task_num);
    static void thread_time(unsigned char task_num);
    static void function_time(unsigned char task_num);
};

#define TASK_TIME_COUNT_BUF_MAX (128)
#define MAX_FUNCTION_COUNT (10)
#define TIMER_COUNT_REGISER LPC_TIM1->TC
#define TIMER_COUNT_REGISER_MAX 0xFFFE

struct task_time_count_t
{
	long buf[TASK_TIME_COUNT_BUF_MAX];
	long long sum;
	long count;
	long offset;
	long temp;
	long us_avarage;
	long us_max;
	long us_min;
	long timer_last_count;
	long timer_overflow_count_start;
	long timer_overflow_count_updata;
};

extern unsigned char time_count_reset_flag;
extern volatile unsigned long long timer_overflow_count;
extern struct task_time_count_t thread_time_count_st[MAX_TASK_COUNT];
extern struct task_time_count_t function_time_count_st[MAX_FUNCTION_COUNT];
#endif
