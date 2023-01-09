


#include "includes.h"

UInt16 DWIN_UartParse::Priority = PRIORITY4;
UInt16 DWIN_UartParse::TaskID = DWIN_UART_PARSE_TASK;
// #pragma arm section rwdata = "SRAM",zidata = "SRAM"		   //  dynamic memory  0XA0000000  0x01E84800
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
//temp
St_DWIN_QueueBuf  DWIN_UartParse::m_DWIN_RxQueue;
long long Aux_MTC_Base::Temperature_set[8];
#pragma arm section //

//==========================DGUS屏CRC校验 采用ANSI CRC-16(X16+X15+X2+1)格式==========================
const unsigned char CRCTABH[256]= { 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
                                    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
                                    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
                                    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
                                    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
                                    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
                                    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
                                    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
                                    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
                                    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
                                    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
                                    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
                                    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
                                    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
                                    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
                                    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
                                    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
                                    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
                                    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
                                    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
                                    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
                                    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
                                    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
                                    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
                                    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
                                    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
                                  };

const unsigned char CRCTABL[256]= { 0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
                                    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
                                    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
                                    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
                                    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
                                    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
                                    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
                                    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
                                    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
                                    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
                                    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
                                    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
                                    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
                                    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
                                    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
                                    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
                                    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
                                    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
                                    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
                                    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
                                    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
                                    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
                                    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
                                    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
                                    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
                                    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
                                  };
//===============================================END=================================================

void DWIN_UartParse::Init(void)
{
    m_DWIN_RxQueue.m_front = 0;
    m_DWIN_RxQueue.m_rear = 0;
    m_DWIN_RxQueue.m_length = 0;
}

/******************************************************************************
** Function name:        DoWork
** Descriptions:         Communicate with DWIN DGUS LCD SCREEN
** parameters:           None
** Returned value:       None
** Created Date:         2018/04/05
** Revision Dates:
******************************************************************************/
void DWIN_UartParse::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_UART_DWIN]++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time_init(COUNT_UART_DWIN);
#endif
	
    //===============================================
    //++++++++DWIN+++++++++++++++++++ Receive Data
    //===============================================
    if(UARTFunc::gl_Rx_last[UART_M4TOM0_CH]>8)
    {
        unsigned short CmdHead;
        unsigned short CRCCheck,CRCSum;
        //数据帧头
        CmdHead = UARTFunc::gl_Rx_buf[UART_M4TOM0_CH][0]<<8;
        CmdHead += UARTFunc::gl_Rx_buf[UART_M4TOM0_CH][1];
        if(CmdHead != 0x5AA5) //判断帧头
        {
            UARTFunc::gl_Rx_last[1]=0;
            UARTFunc::gl_Rx_first[1]=0;
			
#if DEBUG_TASK_STATISTIC == 1
            OS_ServeProcess::Task_Count_Time[COUNT_UART_DWIN] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
            Time_Count::thread_time(COUNT_UART_DWIN);
#endif
			
            return;
        }
        unsigned char length = UARTFunc::gl_Rx_buf[UART_M4TOM0_CH][2]+3;//length=指令+数据+CRC+3(包括2位帧头和1位数据长度)
        if(length >UARTFunc::gl_Rx_last[UART_M4TOM0_CH])  //字节未收完，返回不做处理
		{
		
#if DEBUG_TASK_STATISTIC == 1
            OS_ServeProcess::Task_Count_Time[COUNT_UART_DWIN] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
            Time_Count::thread_time(COUNT_UART_DWIN);
#endif		
            return;
		}
        memcpy((void*)&m_DWIN_RxQueue.m_Buf[m_DWIN_RxQueue.m_rear][0],(void*)&UARTFunc::gl_Rx_buf[UART_M4TOM0_CH][0],length);

        unsigned char CRCLEN =UARTFunc::gl_Rx_buf[UART_M4TOM0_CH][2];
        CRCCheck = DWIN_UartParse::CRC_Check( &m_DWIN_RxQueue.m_Buf[m_DWIN_RxQueue.m_rear][3],(CRCLEN-2));
        CRCSum = m_DWIN_RxQueue.m_Buf[m_DWIN_RxQueue.m_rear][CRCLEN+1]<<8;
        CRCSum += m_DWIN_RxQueue.m_Buf[m_DWIN_RxQueue.m_rear][CRCLEN+2];
        if(CRCCheck!=CRCSum)           //CRC校验错，返回
        {
            UARTFunc::gl_Rx_last[1]=0;
            UARTFunc::gl_Rx_first[1]=0;
			
#if DEBUG_TASK_STATISTIC == 1
            OS_ServeProcess::Task_Count_Time[COUNT_UART_DWIN] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
            Time_Count::thread_time(COUNT_UART_DWIN);
#endif			
            return;
        }

        m_DWIN_RxQueue.m_rear = (m_DWIN_RxQueue.m_rear+1)%MAX_BUF_LENGTH;	//移动存数指针
        m_DWIN_RxQueue.m_length++; //长度加一？
        UARTFunc::gl_Rx_last[1]=0;
        UARTFunc::gl_Rx_first[1]=0;
    }
    DoDWINParse();

#if DEBUG_TASK_STATISTIC == 1
            OS_ServeProcess::Task_Count_Time[COUNT_UART_DWIN] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
            Time_Count::thread_time(COUNT_UART_DWIN);
#endif
}

/******************************************************************************
** Function name:        DoDWINParse
** Descriptions:         Parse the data from DWIN LCD SCREEN
** parameters:           None
** Returned value:       None
** Created Date:         2018/04/04
** Revision Dates:
******************************************************************************/
void DWIN_UartParse::DoDWINParse(void)
{
    unsigned char Cmd,length;
//    unsigned char	uc_BoardIdx;
    unsigned short CmdAddress;
    unsigned short keycode;
    long ReadData;
//    unsigned char CRC_H,CRC_L;

    if(m_DWIN_RxQueue.m_length>0 && m_DWIN_RxQueue.m_length<MAX_BUF_LENGTH)
    {
        //NVIC_DisableIRQ(DMA_IRQn);
        if(m_DWIN_RxQueue.m_front ==  m_DWIN_RxQueue.m_rear)
            return;

        Cmd =  m_DWIN_RxQueue.m_Buf[m_DWIN_RxQueue.m_front][3];
        CmdAddress = m_DWIN_RxQueue.m_Buf[m_DWIN_RxQueue.m_front][4]<<8;
        CmdAddress += m_DWIN_RxQueue.m_Buf[m_DWIN_RxQueue.m_front][5];

        keycode = m_DWIN_RxQueue.m_Buf[m_DWIN_RxQueue.m_front][7]<<8;
        keycode += m_DWIN_RxQueue.m_Buf[m_DWIN_RxQueue.m_front][8];

        ReadData = m_DWIN_RxQueue.m_Buf[m_DWIN_RxQueue.m_front][7]<<24;
        ReadData += m_DWIN_RxQueue.m_Buf[m_DWIN_RxQueue.m_front][8]<<16;
        ReadData += m_DWIN_RxQueue.m_Buf[m_DWIN_RxQueue.m_front][9]<<8;
        ReadData += m_DWIN_RxQueue.m_Buf[m_DWIN_RxQueue.m_front][10];

        if((CmdAddress >= 0x00F0) && (CmdAddress <= 0x00FE))//For target temperature of channel 1-8
        {
            int TECChannelNo = (int) ((CmdAddress - 0x00F0)/2);
//			if(Aux_MTC_Base::MTC_CH_Status[TECChannelNo] == 0x10000)  //温控箱在运行状态时才会接受设定值。
//			{
            if(ReadData>6000)
                ReadData=6000;
            else if(ReadData<1000)
                ReadData=1000;
            Aux_MTC_Base::Temperature_set[TECChannelNo] = ReadData/10;//get temperature setpoint
            DWIN_UartParse::UpdateValue(CmdAddress, ReadData);
//			}
        }
        else if(CmdAddress == 0x00EE) {							//For all-set temperature
            if(ReadData>6000)
                ReadData=6000;
            else if(ReadData<1000)
                ReadData=1000;
            for(int i=0; i<8; i++) {
                Aux_MTC_Base::Temperature_set[i] = ReadData/10;
                Aux_MTC_Base::MTCSafetyCheckTimeBuffer[i] = CommonFunc::GetSystemTime();
                DWIN_UartParse::UpdateValue(0x00F0+i*2, ReadData);
            }
        }
        else if(CmdAddress == 0x00E6) {							//For all-set chamber on/off
            if((ReadData&0x10000) == 0x10000) {
                for(int i=0; i<8; i++) {
                    Aux_MTC_Base::TurnOnMTCChn(i);
                    Aux_MTC_Base::MTCSafetyCheckTimeBuffer[i] = CommonFunc::GetSystemTime();
                }
            }
            else {
                for(int i=0; i<8; i++) {
                    Aux_MTC_Base::TurnOffMTCChn(i);
                }
            }
        }
        else if(CmdAddress == 0x00E0) {							//For all-set 超时保护
            if((ReadData&0x10000) == 0x10000) {
                for(int i=0; i<8; i++) {
                    Aux_MTC_Base::ALLTemperatureSafetyCheckFlag = 1;
                    Aux_MTC_Base::Aux_MTC_TemperatureSafetyCheckFlag[i] = 1;
                    Aux_MTC_Base::MTCSafetyCheckTimeBuffer[i] = CommonFunc::GetSystemTime();
                }
            }
            else if((ReadData&0x10000) == 0x00000) {
                for(int i=0; i<8; i++) {
                    Aux_MTC_Base::ALLTemperatureSafetyCheckFlag = 0;
                    Aux_MTC_Base::Aux_MTC_TemperatureSafetyCheckFlag[i] = 0;
                }
            }
        }
        else if(CmdAddress == 0x00E8) {							//For all-set 超时保护时间
            if(ReadData >= 0) {
                for(int i=0; i<8; i++) {
                    Aux_MTC_Base::MTCSafetyCheckTimeValue[i] = ReadData/100;
                    Aux_MTC_Base::MTCSafetyCheckTimeBuffer[i] = CommonFunc::GetSystemTime();
                }
            }
        }
        else if(CmdAddress == 0x00EA) {							//For all-set 温度保护上限值
            if(ReadData >= 0) {
                for(int i=0; i<8; i++) {
                    Aux_MTC_Base::Aux_MTC_TemperatureHighLimit[i] = ReadData/100;
                }
            }
        }
        else if(CmdAddress == 0x00EC) {							//For all-set 温度保护下限值
            if(ReadData >= 0) {
                for(int i=0; i<8; i++) {
                    Aux_MTC_Base::Aux_MTC_TemperatureLowLimit[i] = ReadData/100;
                }
            }
        }
        else if(CmdAddress == 0x00E2) {							//For all-set 温度保护上限
            if((ReadData&0x10000) == 0x10000) {
                for(int i=0; i<8; i++) {
                    Aux_MTC_Base::ALLTemperatureHighLimitCheckFlag = 1;
                    Aux_MTC_Base::Aux_MTC_TemperatureHighLimitCheckFlag[i] = 1;
                }
            }
            else if((ReadData&0x10000) == 0) {
                for(int i=0; i<8; i++) {
                    Aux_MTC_Base::ALLTemperatureHighLimitCheckFlag = 0;
                    Aux_MTC_Base::Aux_MTC_TemperatureHighLimitCheckFlag[i] = 0;
                }
            }
        }

        else if(CmdAddress == 0x00E4) {							//For all-set 温度保护下限
            if((ReadData&0x10000) == 0x10000) {
                for(int i=0; i<8; i++) {
                    Aux_MTC_Base::ALLTemperatureLowLimitCheckFlag = 1;
                    Aux_MTC_Base::Aux_MTC_TemperatureLowLimitCheckFlag[i] = 1;
                }
            }
            else if((ReadData&0x10000) == 0) {
                for(int i=0; i<8; i++) {
                    Aux_MTC_Base::ALLTemperatureLowLimitCheckFlag = 0;
                    Aux_MTC_Base::Aux_MTC_TemperatureLowLimitCheckFlag[i] = 0;
                }
            }
        }
        else if((CmdAddress >= 0x0090) && (CmdAddress <= 0x009E)) {
            int TECChannelNo = (int) ((CmdAddress - 0x0090)/2);
            if((ReadData&0x10000) == 0x10000)
            {
                Aux_MTC_Base::TurnOnMTCChn(TECChannelNo);
                Aux_MTC_Base::MTCSafetyCheckTimeBuffer[TECChannelNo] = CommonFunc::GetSystemTime();
            }
            else if((ReadData&0x10000) == 0)
                Aux_MTC_Base::TurnOffMTCChn(TECChannelNo);
        }
        else if((CmdAddress >= 0x00A0) && (CmdAddress <= 0x00AE)) { //温度保护上限值
            int TECChannelNo = (int) ((CmdAddress - 0x00A0)/2);
            if(ReadData >= 0) {
                Aux_MTC_Base::Aux_MTC_TemperatureHighLimit[TECChannelNo] = ReadData/100;
            }
        }
        else if((CmdAddress >= 0x00C0) && (CmdAddress <= 0x00CE)) { //温度保护下限值
            int TECChannelNo = (int) ((CmdAddress - 0x00C0)/2);
            if(ReadData >= 0) {
                Aux_MTC_Base::Aux_MTC_TemperatureLowLimit[TECChannelNo] = ReadData/100;
            }
        }
        else if((CmdAddress >= 0x00B0) && (CmdAddress <= 0x00BE)) { //温度超时时间
            int TECChannelNo = (int) ((CmdAddress - 0x00B0)/2);
            if(ReadData >= 0) {
                Aux_MTC_Base::MTCSafetyCheckTimeValue[TECChannelNo] = ReadData/100;
                Aux_MTC_Base::MTCSafetyCheckTimeBuffer[TECChannelNo] = CommonFunc::GetSystemTime();
            }
        }
        else if((CmdAddress >= 0x0110) && (CmdAddress <= 0x011E)) { //温度保护上限
            int TECChannelNo = (int) ((CmdAddress - 0x0110)/2);
            if((ReadData&0x10000) == 0x10000) {
                Aux_MTC_Base::Aux_MTC_TemperatureHighLimitCheckFlag[TECChannelNo] = 1;
            }
            else if((ReadData&0x10000) == 0) {
                Aux_MTC_Base::Aux_MTC_TemperatureHighLimitCheckFlag[TECChannelNo] = 0;
            }
        }
        else if((CmdAddress >= 0x0120) && (CmdAddress <= 0x012E)) { //温度保护下限
            int TECChannelNo = (int) ((CmdAddress - 0x0120)/2);
            if((ReadData&0x10000) == 0x10000) {
                Aux_MTC_Base::Aux_MTC_TemperatureLowLimitCheckFlag[TECChannelNo] = 1;
            }
            else if((ReadData&0x10000) == 0) {
                Aux_MTC_Base::Aux_MTC_TemperatureLowLimitCheckFlag[TECChannelNo] = 0;
            }
        }
        else if((CmdAddress >= 0x0100) && (CmdAddress <= 0x010E)) { //使能温度超时保护
            int TECChannelNo = (int) ((CmdAddress - 0x0100)/2);
            if((ReadData&0x10000) == 0x10000) {
                Aux_MTC_Base::Aux_MTC_TemperatureSafetyCheckFlag[TECChannelNo] = 1;
            }
            else if((ReadData&0x10000) == 0x00000) {
                Aux_MTC_Base::Aux_MTC_TemperatureSafetyCheckFlag[TECChannelNo] = 0;
            }
            Aux_MTC_Base::MTCSafetyCheckTimeBuffer[TECChannelNo] = CommonFunc::GetSystemTime();
        }
        else if(CmdAddress == 0x00D2) { //更新运行模式
            if((ReadData&0x30000) == 0x10000) { //Remote Mode
                for(int i=0; i<8; i++) {
                    DWIN_UartParse::SwitchModeReset(i);
                }
            }
            else if((ReadData&0x30000) == 0x20000) { //Local Mode
                for(int i=0; i<8; i++) {
                    DWIN_UartParse::SwitchModeReset(i);
                }
            }
        }
        m_DWIN_RxQueue.m_length--;
        m_DWIN_RxQueue.m_front = (m_DWIN_RxQueue.m_front + 1)%MAX_BUF_LENGTH;
    }
}

/******************************************************************************
** Function name:        DWIN_SendCmd
** Descriptions:         UART SEND DATA TO DWIN
** parameters:           Uart_Cmd, length
** Returned value:       None
** Created Date:         2018/04/04
** Revision Dates:
******************************************************************************/
void DWIN_UartParse::DWIN_SendCmd(unsigned char  * Uart_Cmd,unsigned char length)
{
    UARTFunc::uart_putcmd (1, (unsigned char*)Uart_Cmd,length);
}

/******************************************************************************
** Function name:        DWIN_ACK
** Descriptions:         DWIN ACKNOWLEDGE
** parameters:           address, length
** Returned value:       None
** Created Date:         2018/04/04
** Revision Dates:
******************************************************************************/
void DWIN_UartParse::DWIN_ACK(unsigned short address, unsigned char length)
{
    unsigned char sentdata[9];
    unsigned short CRCSum;

    sentdata[0] = 0x5A;
    sentdata[1] = 0xA5;
    sentdata[2] = 0x06;
    sentdata[3] = 0x83;
    sentdata[4] = (address&0xFF00)>>8;
    sentdata[5] =(address&0x00FF);
    sentdata[6] =length;

    CRCSum=DWIN_UartParse::CRC_Check(&sentdata[3],4);
    sentdata[7]= (CRCSum>>8);
    sentdata[8]= (CRCSum&0x00FF);
    DWIN_UartParse::DWIN_SendCmd(&sentdata[0],9);
}
/******************************************************************************
** Function name:        UpdateValue
** Descriptions:         Update current temperature setpoint value
** parameters:           address, data
** Returned value:       None
** Created Date:         2018/04/04
** Revision Dates:
******************************************************************************/
void DWIN_UartParse::UpdateValue(unsigned short address, long data)
{
    unsigned char sentdata[12];
    unsigned short CRCSum;

    sentdata[0] = 0x5A;
    sentdata[1] = 0xA5;
    sentdata[2] = 0x09;
    sentdata[3] = 0x82; //指定变量地址开始写入数据串（字数据）到变量存储区
    sentdata[4] = (address&0xFF00)>>8;
    sentdata[5] = (address&0x00FF);
    sentdata[6] = (data&0xFF000000)>>24;
    sentdata[7] = (data&0x00FF0000)>>16;
    sentdata[8] = (data&0x0000FF00)>>8;
    sentdata[9] = (data&0x000000FF);

    CRCSum=DWIN_UartParse::CRC_Check(&sentdata[3],7);
    sentdata[10]= (CRCSum>>8);
    sentdata[11]= (CRCSum&0x00FF);
    DWIN_UartParse::DWIN_SendCmd(&sentdata[0],12);
}

/******************************************************************************
** Function name:        UpdateValue
** Descriptions:         Update current temperature setpoint value
** parameters:           address, data
** Returned value:       None
** Created Date:         2018/04/04
** Revision Dates:
******************************************************************************/
void DWIN_UartParse::MTCReportErr(unsigned short address, unsigned short data)
{
    unsigned char sentdata[10];
    unsigned short CRCSum;

    sentdata[0] = 0x5A;
    sentdata[1] = 0xA5;
    sentdata[2] = 0x07;
    sentdata[3] = 0x82; //指定变量地址开始写入数据串（字数据）到变量存储区
    sentdata[4] = (address&0xFF00)>>8;
    sentdata[5] = (address&0x00FF);
    sentdata[6] = (data&0xFF00)>>8;
    sentdata[7] = (data&0x00FF);

    CRCSum=DWIN_UartParse::CRC_Check(&sentdata[3],5);
    sentdata[8]= (CRCSum>>8);
    sentdata[9]= (CRCSum&0x00FF);
    DWIN_UartParse::DWIN_SendCmd(&sentdata[0],10);
}

/******************************************************************************
** Function name:        SwitchToRemote
** Descriptions:         Switch To Remote Mode
** parameters:           address, data
** Returned value:       None
** Created Date:         2019/01/03
** Revision Dates:
******************************************************************************/
void DWIN_UartParse::MTCSwitchToRemote()
{
    unsigned char sentdata[12];
    unsigned short CRCSum;

    sentdata[0] = 0x5A;
    sentdata[1] = 0xA5;
    sentdata[2] = 0x06;
    sentdata[3] = 0x80; //指定变量地址开始写入数据串（字数据）到变量存储区
    sentdata[4] = 0x03;
    sentdata[5] = 0x00;
    sentdata[6] = 0x16;

    CRCSum=DWIN_UartParse::CRC_Check(&sentdata[3],4);
    sentdata[7]= (CRCSum>>8);
    sentdata[8]= (CRCSum&0x00FF);
    DWIN_UartParse::DWIN_SendCmd(&sentdata[0],9);

    sentdata[6] = 0x08;

    CRCSum=DWIN_UartParse::CRC_Check(&sentdata[3],4);
    sentdata[7]= (CRCSum>>8);
    sentdata[8]= (CRCSum&0x00FF);
    DWIN_UartParse::DWIN_SendCmd(&sentdata[0],9);

    sentdata[2] = 0x09;
    sentdata[3] = 0x82; //指定变量地址开始写入数据串（字数据）到变量存储区
    sentdata[4] = 0x00;
    sentdata[5] = 0xD0;
    sentdata[6] = 0x00;
    sentdata[7] = 0x01;
    sentdata[8] = 0x00;
    sentdata[9] = 0x00;

    CRCSum=DWIN_UartParse::CRC_Check(&sentdata[3],7);
    sentdata[10]= (CRCSum>>8);
    sentdata[11]= (CRCSum&0x00FF);
    DWIN_UartParse::DWIN_SendCmd(&sentdata[0],12);

    Aux_MTC_Base::MTCMode[Aux_MTC_Base::index]++;
}

void DWIN_UartParse::MTCKeepRemote()
{
    unsigned char sentdata[12];
    unsigned short CRCSum;

    sentdata[0] = 0x5A;
    sentdata[1] = 0xA5;
    sentdata[2] = 0x06;
    sentdata[3] = 0x80; //指定变量地址开始写入数据串（字数据）到变量存储区
    sentdata[4] = 0x03;
    sentdata[5] = 0x00;
    sentdata[6] = 0x08;

    CRCSum=DWIN_UartParse::CRC_Check(&sentdata[3],4);
    sentdata[7]= (CRCSum>>8);
    sentdata[8]= (CRCSum&0x00FF);
    DWIN_UartParse::DWIN_SendCmd(&sentdata[0],9);
}

void DWIN_UartParse::SwitchModeReset(unsigned char AuxChanNo) {
    Aux_MTC_Base::TurnOffMTCChn(AuxChanNo);
    Aux_MTC_Base::Aux_MTC_TemperatureSafetyCheckFlag[AuxChanNo] = 0;
    Aux_MTC_Base::Aux_MTC_TemperatureHighLimitCheckFlag[AuxChanNo] = 0;
    Aux_MTC_Base::Aux_MTC_TemperatureLowLimitCheckFlag[AuxChanNo] = 0;
    Aux_MTC_Base::MTCSafetyCheckTimeValue[AuxChanNo] = 60;
    Aux_MTC_Base::Aux_MTC_TemperatureHighLimit[AuxChanNo] = 65;
    Aux_MTC_Base::Aux_MTC_TemperatureLowLimit[AuxChanNo] = 5;
    for(int m=0; m<=31; m++)
    {
        for(int n=0; n<=7; n++)
        {
            DWIN_UartParse::MTCReportErr(0x0FC0+n*2,0x0000);
        }
    }
}

void DWIN_UartParse::MITSwitchRemote(unsigned char AuxChanNo) {
    Aux_MTC_Base::Aux_MTC_TemperatureSafetyCheckFlag[AuxChanNo] = 0;
    Aux_MTC_Base::Aux_MTC_TemperatureHighLimitCheckFlag[AuxChanNo] = 0;
    Aux_MTC_Base::Aux_MTC_TemperatureLowLimitCheckFlag[AuxChanNo] = 0;
    Aux_MTC_Base::MTCSafetyCheckTimeValue[AuxChanNo] = 60;
    Aux_MTC_Base::Aux_MTC_TemperatureHighLimit[AuxChanNo] = 65;
    Aux_MTC_Base::Aux_MTC_TemperatureLowLimit[AuxChanNo] = 5;
    for(int m=0; m<=31; m++)
    {
        for(int n=0; n<=7; n++)
        {
            DWIN_UartParse::MTCReportErr(0x0FC0+n*2,0x0000);
        }
    }
}

/******************************************************************************
** Function name:        CRC_Check
** Descriptions:         CRC Check
** parameters:           *pDataBuf, Datalength
** Returned value:       CRCSum
** Created Date:         2018/04/04
** Revision Dates:
******************************************************************************/
unsigned short DWIN_UartParse::CRC_Check( unsigned char *pDataBuf,unsigned char Datalength)
{
    unsigned char index, crch, crcl;
    unsigned short CRCSum;

    crch = 0xFF;
    crcl = 0xFF;
    for(unsigned char i=0; i<Datalength; i++)
    {
        index= crch ^(*pDataBuf);
        crch =crcl ^ CRCTABH[index];
        crcl = CRCTABL[index];
        pDataBuf++;
    }
    CRCSum = crch<<8;
    CRCSum +=crcl;
    return  CRCSum;
}

