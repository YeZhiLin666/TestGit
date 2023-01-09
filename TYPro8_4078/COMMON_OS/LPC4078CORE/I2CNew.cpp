#include "includes.h"
//#include "../Ext_Devices/LM75B.h"
#if  USE_AS_AUXMCU == 1
UInt16	I2CBusNew::TaskID = I2C_TASK;
UInt16 I2CBusNew::Priority = PRIORITY4;
#endif
extern uint32_t PeripheralClock;
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x03E80000
#endif

//////////////////////////////////////////////
st_I2CTaskManager I2CBusNew::m_I2CTaskMgr[MAX_I2C_NUM];
st_I2CInitConfig  I2CBusNew::I2CConfig[MAX_I2C_NUM];
st_I2CModule      I2CBusNew::RunningBuffer[MAX_I2C_NUM];
CQ_I2CBUF         I2CBusNew::m_I2CBuf[MAX_I2C_NUM];  //�������
I2CHandleQueue    I2CBusNew::m_qI2CHandle;
st_I2CBusManager  I2CBusNew::m_I2CBusMgr[MAX_I2C_NUM];

////////////////////////////////////////////////
#pragma arm section

unsigned short I2CBusNew::HandleIdx;

const int I2CBusNew::const_StartOvertime = 10;
const int I2CBusNew::const_StopOvertime = 10;
const St_Port I2CBusNew::I2C1_SDA_IOCFG	=	{0,	19, eCPTYPE_MCUGPIO,	I2C1_SDA,	0	};
const St_Port I2CBusNew::I2C1_SCL_IOCFG	=	{0,	20, eCPTYPE_MCUGPIO,	I2C1_SCL,	0	};



void I2C1_IRQHandler(void)
{
    I2CBusNew::ProcessISRstate(I2C1);
}
void I2C0_IRQHandler(void)
{
    I2CBusNew::ProcessISRstate(I2C0);
}

void I2C2_IRQHandler(void)
{
    I2CBusNew::ProcessISRstate(I2C2);
}
//��ʱֻ��Masterģʽ
void I2CBusNew::ProcessISRstate(unsigned char I2Cx)
{
    LPC_I2C_TypeDef  *pI2Cx;
    if(I2Cx == I2C0)
        pI2Cx = LPC_I2C0;
    else if(I2Cx == I2C1)
        pI2Cx = LPC_I2C1;
    else if(I2Cx == I2C2)
        pI2Cx = LPC_I2C2;
    else
        return;
    // unsigned long IRQ1,IRQ2;
    //IRQFunc::DisableIRQ(&IRQ1,&IRQ2);
    unsigned char I2C_State = pI2Cx->STAT;

    switch(I2C_State)
    {
    case IRQ_BUS_ERR: //0x00
        pI2Cx->CONSET = I2CONSET_STO|I2CONSET_AA;
        pI2Cx->CONCLR = I2CONCLR_SIC ;
        break;
    case IRQ_STARTED: //0x08
        pI2Cx->DAT = I2CBusNew::m_I2CBusMgr[I2Cx].SlaveAdr;//�ӻ���ַ+WRITE
        pI2Cx->CONCLR = I2CONCLR_STAC|I2CONCLR_SIC;
        //pI2Cx->CONCLR = I2CONCLR_SIC;//
        I2CBusNew::m_I2CBusMgr[I2Cx].BusState = I2CBus_STARTED; //set master state as started
        CommonFunc::GetTimeTicksUpdate( &m_I2CBusMgr[I2Cx].Overtime.TransOvertime);
        break;
    case IRQ_RESTARTED://0x10 �ѷ����ظ���ʼ
        if(I2CBusNew::RunningBuffer[I2Cx].OPMode == I2C_MASTREAD)
            pI2Cx->DAT = I2CBusNew::m_I2CBusMgr[I2Cx].SlaveAdr|I2C_ReadBit;//�ӻ���ַ+read
        else if(I2CBusNew::RunningBuffer[I2Cx].OPMode == I2C_MASTWRITE)
            pI2Cx->DAT = I2CBusNew::m_I2CBusMgr[I2Cx].SlaveAdr;
        pI2Cx->CONCLR = I2CONCLR_STAC ;//clear the AA and SI bit
        pI2Cx->CONCLR = I2CONCLR_SIC ;
        I2CBusNew::m_I2CBusMgr[I2Cx].BusState = I2CBus_STARTED;
        break;
    case IRQ_MSTTX_SLA_ACK://0x18 �ѷ��ʹӻ���ַ���յ�ACK
        if(I2CBusNew::m_I2CBusMgr[I2Cx].CtrlByteNum>0)    //slave adr �ݲ�����ctrlbyte
        {
            pI2Cx->DAT = I2CBusNew::m_I2CTaskMgr[I2Cx].CurrentAdr[I2CBusNew::m_I2CBusMgr[I2Cx].CtrlByteIdx];  //�ѷ��شӵ�ַACK�����͵�һ������(WR��ַ��
            I2CBusNew::m_I2CBusMgr[I2Cx].CtrlByteIdx++;
        }
        I2CBusNew::m_I2CBusMgr[I2Cx].BusState = I2CBus_ACK;
        pI2Cx->CONCLR = I2CONCLR_SIC;
        break;

    case IRQ_MSTTX_SLA_NACK://0x20 �ѷ��ʹӻ���ַ���յ�NACK
        I2CBusNew::m_I2CBusMgr[I2Cx].BusState = I2CBus_NACK;
        pI2Cx->CONSET = I2CONSET_STO;
        pI2Cx->CONCLR = I2CONCLR_SIC;
        break;
    case IRQ_MSTTX_DATA_ACK:   //0x28  �ѷ������ݣ��յ�����ACK
        if(I2CBusNew::m_I2CBusMgr[I2Cx].CtrlByteIdx<I2CBusNew::m_I2CBusMgr[I2Cx].CtrlByteNum)
        {
            pI2Cx->DAT =I2CBusNew::m_I2CTaskMgr[I2Cx].CurrentAdr[I2CBusNew::m_I2CBusMgr[I2Cx].CtrlByteIdx];	  //�ѷ��شӵ�ַACK�����͵�һ������(WR��ַ��
            I2CBusNew::m_I2CBusMgr[I2Cx].CtrlByteIdx++;  //�ȵ��ÿ鴦��������
        }
        else  //�ѷ���������ֽڣ��ж�ģʽ
        {
            if(I2CBusNew::RunningBuffer[I2Cx].OPMode == I2C_MASTREAD)  //ת������ģʽ
            {
                pI2Cx->CONSET = (I2CONSET_AA|I2CONSET_STA);	// Set Repeated-start flag //
                I2CBusNew::m_I2CBusMgr[I2Cx].BusState = I2CBus_START;
            }
            else if(I2CBusNew::RunningBuffer[I2Cx].OPMode == I2C_MASTWRITE) //��ʼ��д
            {
                if(I2CBusNew::RunningBuffer[I2Cx].BufIdx< m_I2CTaskMgr[I2Cx].SubBufEndingAdr)
                {
                    pI2Cx->DAT = I2CBusNew::RunningBuffer[I2Cx].Buf[I2CBusNew::RunningBuffer[I2Cx].BufIdx];
                    I2CBusNew::RunningBuffer[I2Cx].BufIdx++;
                    I2CBusNew::m_I2CBusMgr[I2Cx].BusState = I2CBus_ACK;
                }
                else
                {
                    pI2Cx->CONSET = I2CONSET_STO;
                    I2CBusNew::m_I2CBusMgr[I2Cx].BusState = I2CBus_STOPPED;			//��ֹͣ
                    I2CBusNew::m_I2CBusMgr[I2Cx].StopCondition = I2CNormalStop;
                }
            }
        }
        pI2Cx->CONCLR = I2CONCLR_SIC;
        break;
    case IRQ_MSTTX_DATA_NACK: //0x30
//        I2CBusNew::m_I2CBusMgr[I2Cx].BusState = I2CBus_NACK;
        pI2Cx->CONSET = I2CONSET_STO;
        pI2Cx->CONCLR = I2CONCLR_SIC;
        break;
    case IRQ_MSTRX_SLA_ACK:  //0x40 ��������ģʽ �յ��ӻ�ACK
        I2CBusNew::m_I2CBusMgr[I2Cx].BusState = I2CBus_ACK;
        pI2Cx->CONSET = I2CONSET_AA;  //���ý���ACK�ظ�
        pI2Cx->CONCLR = I2CONCLR_SIC;
        break;
    case IRQ_MSTRX_SLA_NACK: //0x48
//        I2CBusNew::m_I2CBusMgr[I2Cx].BusState = I2CBus_NACK;
        pI2Cx->CONSET = I2CONSET_STO;
        pI2Cx->CONCLR = I2CONCLR_SIC;
        break;
    case IRQ_MSTRX_DATA_ACK:   //0x50 ������������
        if(I2CBusNew::RunningBuffer[I2Cx].OPMode == I2C_MASTREAD)
        {
            I2CBusNew::RunningBuffer[I2Cx].Buf[I2CBusNew::RunningBuffer[I2Cx].BufIdx] = pI2Cx->DAT;
            I2CBusNew::RunningBuffer[I2Cx].BufIdx++;
            if(I2CBusNew::RunningBuffer[I2Cx].BufIdx<m_I2CTaskMgr[I2Cx].SubBufEndingAdr)
            {
                I2CBusNew::m_I2CBusMgr[I2Cx].BusState = I2CBus_ACK;
                pI2Cx->CONSET = I2CONSET_AA;
            }
            else
            {
                pI2Cx->CONCLR = I2CONCLR_AAC;
                CommonFunc::GetTimeTicksUpdate(&m_I2CBusMgr[I2Cx].Overtime.StopOvertime);
            }
            pI2Cx->CONCLR = I2CONCLR_SIC;
        }
        else
        {
            pI2Cx->CONSET = I2CONSET_STO;
            pI2Cx->CONCLR = I2CONCLR_SIC;
        }
        break;
    case IRQ_MSTRX_DATA_NACK:  //0x58 �ѷ���NACK��������ϣ�STOP
        I2CBusNew::m_I2CBusMgr[I2Cx].BusState = I2CBus_STOPPED;
        I2CBusNew::m_I2CBusMgr[I2Cx].StopCondition = I2CNormalStop;
        pI2Cx->CONSET = I2CONSET_STO;
        pI2Cx->CONCLR = I2CONCLR_SIC;
        break;
    default:
        pI2Cx->CONCLR = I2CONCLR_SIC;
        break;
    }
    //IRQFunc::EnableIRQ(IRQ1,IRQ2);
}



/***************************I2C��ʼ��*********************/
unsigned char  I2CBusNew::I2CInit (unsigned char I2Cx,unsigned short BaudRate)
{
    I2CConfig[I2Cx].I2CSpeed = BaudRate;
    I2CConfig[I2Cx].I2C_SCKPort = I2C1_SCL_IOCFG;
    I2CConfig[I2Cx].I2C_SDLPort = I2C1_SDA_IOCFG;
    I2CConfig[I2Cx].TransDelaytime = MAX_FRAMELEN*(I2CConfig[I2Cx].I2CSpeed);   //10ms?  0.1ms/byte  100KHz

    LPC_I2C_TypeDef  *pI2Cx;
    if(I2Cx == I2C0)
    {
        pI2Cx = LPC_I2C0;
        LPC_IOCON->P0_27 = 0x01;//Added by ZC 09/29/2016
        LPC_IOCON->P0_28 = 0x01;//Added by ZC 09/29/2016
    }
    else if(I2Cx == I2C1)
    {
        pI2Cx = LPC_I2C1;
        LPC_IOCON->P0_19 = 0x4b3;//0x4b3  //���óɿ�©ģʽ
        LPC_IOCON->P0_20 = 0x4b3;
    }
    else if(I2Cx == I2C2)
    {
        pI2Cx = LPC_I2C2;
        LPC_IOCON->P0_10 = 0x4b2;
        LPC_IOCON->P0_11 = 0x4b2;
    }
    else
        return false;
    //ControlPin::SetIObyCP(I2CConfig[I2Cx].I2C_SCKPort);
    //ControlPin::SetIObyCP(I2CConfig[I2Cx].I2C_SDLPort);

    //--- Clear flags ---//
    pI2Cx->CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC;
    //clear acc bit ,sic bit ,stac bit ,i2enc bit

    if(I2CConfig[I2Cx].I2CSpeed<=400UL)
    {
        pI2Cx->SCLL = (PeripheralClock/(I2CConfig[I2Cx].I2CSpeed*1000))/2;//???
        pI2Cx->SCLH = (PeripheralClock/(I2CConfig[I2Cx].I2CSpeed*1000))/2; //
    }
    else
    {
        pI2Cx->SCLL   = 75;  //I2Cbitfreq 400KHz; //  PCLK=  60(MHz), I2SCLL+I2SCLH values=125
        pI2Cx->SCLH   = 75;
    }

    switch( I2Cx )
    {
    case I2C0:
        LPC_SC->PCONP |= PCI2C0;   // Turn On I2C0 Power //
        IRQFunc::install_irq( I2C0_IRQn, HIGHEST_PRIORITY + 7);
// 		if( IRQFunc::install_irq( I2C0_IRQn, HIGHEST_PRIORITY + 7) == FALSE )
// 			return false;
        break;
    case I2C1:
        LPC_SC->PCONP |= PCI2C1;   // Turn On I2C1 Power //
        // Install interrupt handler //
        IRQFunc::install_irq( I2C1_IRQn, HIGHEST_PRIORITY + 8);
// 		if( IRQFunc::install_irq( I2C1_IRQn, HIGHEST_PRIORITY + 8) == FALSE )
// 			return false ;
        break;
    case I2C2:
        LPC_SC->PCONP |= PCI2C2;   // Turn On I2C2 Power //
        IRQFunc::install_irq( I2C2_IRQn, HIGHEST_PRIORITY + 9);
// 		if( IRQFunc::install_irq( I2C2_IRQn, HIGHEST_PRIORITY + 9) == FALSE )
// 			return false;
        break;
    default:
        return false;//error
    }
    pI2Cx->CONSET = I2CONSET_I2EN;//i2c interface enable
    // I2CӦ��״̬��ʼ��
    m_I2CBusMgr[I2Cx].I2CBusy = I2C_NOT_BUSY;
    m_I2CBusMgr[I2Cx].BusState = I2CBus_IDLE;
    RunningBuffer[I2Cx].BufIdx = 0;
    RunningBuffer[I2Cx].CtrlByteNum = 0;
    return true;
}
#if  USE_AS_AUXMCU == 1
void I2CBusNew::TaskInit()
{
    INIT_QUEUE(m_qI2CHandle);
    HandleIdx = 0;
    for(unsigned char i = 0; i<MAX_I2C_NUM; i++)
    {
        INIT_QUEUE(m_I2CBuf[i]);
        m_I2CTaskMgr[i].I2C_TaskState = I2C_Task_Idle;
        RunningBuffer[i].BufIdx = 0;
        RunningBuffer[i].CtrlByteNum = 0;
        m_I2CTaskMgr[i].RetryCnt = 0;
        m_I2CTaskMgr[i].CurrentPackNo = 0;
        m_I2CBusMgr[i].I2CBusy = I2C_NOT_BUSY;
    }
}
void I2CBusNew::DoWork()
{
    //�ж��Ƿ�������
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_I2C] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time_init(COUNT_I2C);
#endif

    for(unsigned char I2Cx = 0; I2Cx<MAX_I2C_NUM; I2Cx++)
    {
        DoI2CTask(I2Cx);
    }

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_CAN] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time(COUNT_I2C);
#endif

}


void I2CBusNew::DoI2CTask(unsigned char I2Cx)
{
    unsigned char bDequeueOk = 0;
    if(m_I2CBusMgr[I2Cx].I2CBusy == I2C_IMME_BUSY)  //lock
        return;
    switch(m_I2CTaskMgr[I2Cx].I2C_TaskState)
    {
    case I2C_Task_Idle:
        if( m_I2CBuf[I2Cx].m_u32Length>0)
        {
            m_I2CBusMgr[I2Cx].I2CBusy = I2C_TASK_BUSY;
            DEQUEUE(m_I2CBuf[I2Cx],MaxRequestNum,RunningBuffer[I2Cx],bDequeueOk);
            if(bDequeueOk)
            {
                m_I2CTaskMgr[I2Cx].I2C_TaskState = I2C_Task_DataTransfer;
                InitFirstBuf(&RunningBuffer[I2Cx]);   //�׸����ݷֿ鴦��
            }
            else
            {
                m_I2CBusMgr[I2Cx].I2CBusy = I2C_NOT_BUSY;
                return;
            }
        }
        else
            m_I2CBusMgr[I2Cx].I2CBusy = I2C_NOT_BUSY;
        break;
    case I2C_Task_DataTransfer: //�������ͺͼ��I2C״̬
        I2C_BusState I2CState;
        I2CState= DetectBusState(I2Cx);
        if(I2CState == I2CBus_STOPPED)  //�����/�쳣
        {
            CommonFunc::GetTimeTicksUpdate(&m_I2CBusMgr[I2Cx].Overtime.NextTransDelay); //waiting for next transmission
            if(m_I2CBusMgr[I2Cx].StopCondition ==I2CNormalStop)
                m_I2CTaskMgr[I2Cx].I2C_TaskState = I2C_Task_PrepareNext;
            else if(m_I2CBusMgr[I2Cx].StopCondition ==I2CAbnormalStop)
                m_I2CTaskMgr[I2Cx].I2C_TaskState = I2C_Task_RetryTrans;
            else
                m_I2CTaskMgr[I2Cx].I2C_TaskState = I2C_Task_DataTransfer;//!!!
            m_I2CBusMgr[I2Cx].BusState = I2CBus_IDLE;   //i2c����״̬
        }
        else if(I2CState == I2CBus_NACK)
        {
            m_I2CTaskMgr[I2Cx].I2C_TaskState = I2C_Task_ReportError;
        }
        break;

    case I2C_Task_PrepareNext: //׼����һ���ӿ�/������һ������
        if(CommonFunc::GetTimeTicks( &m_I2CBusMgr[I2Cx].Overtime.NextTransDelay)>50) //10ms �ȴ���һ������
        {
            if(PrepareNextBuf(&RunningBuffer[I2Cx]))	   //���һ���ӿ�
            {
                m_I2CTaskMgr[I2Cx].I2C_TaskState = I2C_TASK_DataHold;   //ת�����ݱ���
                CommonFunc::GetTimeTicksUpdate(&m_I2CBusMgr[I2Cx].Overtime.HoldingTime);
                SetRequestStatus(m_I2CTaskMgr[I2Cx].CurrentHandle,I2C_ReqFin);
            }
            else
                m_I2CTaskMgr[I2Cx].I2C_TaskState = I2C_Task_DataTransfer;  //�������һ���ӿ�
        }
        break;

    case I2C_TASK_DataHold:    //�Ѵ������������ݱ���1S����ʱ����������������һ������
        if(GetRequestStatus(m_I2CTaskMgr[I2Cx].CurrentHandle) == I2C_ReqHandled) //�ѱ�����ģ�鴦����
        {
            m_I2CTaskMgr[I2Cx].I2C_TaskState = I2C_Task_Idle;   //ת��IDLE
            SetRequestStatus(m_I2CTaskMgr[I2Cx].CurrentHandle,I2C_ReqDestroy);
        }
        else if(CommonFunc::GetTimeTicks( &m_I2CBusMgr[I2Cx].Overtime.HoldingTime)>10000) //1s ������������ݱ���
        {
            m_I2CTaskMgr[I2Cx].I2C_TaskState = I2C_Task_Idle;   //ת��IDLE
        }
        break;

    case I2C_Task_RetryTrans:  // ���Դ���
        RetryCurrentBuf(I2Cx);
        break;

    case I2C_Task_ReportError:  //������
        SetRequestStatus(m_I2CTaskMgr[I2Cx].CurrentHandle,I2C_ReqErr);
        m_I2CTaskMgr[I2Cx].I2C_TaskState = I2C_Task_Idle;
        break;
    default:
        break;
    }

}
//I2C����״̬���
I2C_BusState I2CBusNew::DetectBusState(unsigned char I2Cx)
{
    LPC_I2C_TypeDef  *pI2Cx;
    static unsigned char CurrentOPMode = I2C_NOWORK;
    if(I2Cx == I2C0)
        pI2Cx = LPC_I2C0;
    else if(I2Cx == I2C1)
        pI2Cx = LPC_I2C1;
    else if(I2Cx == I2C2)
        pI2Cx = LPC_I2C2;
    else
        return I2CBus_NACK;

    CurrentOPMode = RunningBuffer[I2Cx].OPMode;

    switch(I2CBusNew::m_I2CBusMgr[I2Cx].BusState)
    {
    case I2CBus_IDLE:
        if(CurrentOPMode!=I2C_NOWORK)
        {
            m_I2CBusMgr[I2Cx].BusState = I2CBus_START;
            pI2Cx->CONSET = I2CONSET_STA;
            CommonFunc::GetTimeTicksUpdate(&m_I2CBusMgr[I2Cx].Overtime.StartOvertime);
        }
        break;
    case I2CBus_START:
        //Over time judgement
        if(CommonFunc::GetTimeTicks( &m_I2CBusMgr[I2Cx].Overtime.StartOvertime) > const_StartOvertime)	// ��ʱ1ms
        {
            pI2Cx->CONCLR = I2CONCLR_STAC;
            m_I2CBusMgr[I2Cx].StopCondition = I2CAbnormalStop;
            m_I2CBusMgr[I2Cx].BusState = I2CBus_STOPPED;
            CurrentOPMode = I2C_NOWORK;
        }
        break;
    case I2CBus_STARTED:
        break;
    case I2CBus_ACK:
        if(CommonFunc::GetTimeTicks( &m_I2CBusMgr[I2Cx].Overtime.TransOvertime) > I2CConfig[I2Cx].TransDelaytime)// overtime Xms
        {   //��ʱ����
            I2CBusNew::I2CStop(I2Cx);
            m_I2CBusMgr[I2Cx].StopCondition = I2CAbnormalStop;
            m_I2CBusMgr[I2Cx].BusState = I2CBus_STOPPED;
        }
        break;
    case I2CBus_NACK:           //Set CONSET = STO in I2CIRQ
        m_I2CBusMgr[I2Cx].BusState = I2CBus_STOPPED;
        m_I2CBusMgr[I2Cx].StopCondition = I2CAbnormalStop;
        break;
    case I2CBus_STOPPED:		 //�ж�ֹͣ���� normal or abnormal
        CurrentOPMode = I2C_NOWORK;
        break;
    default:
        break;
    }

    return m_I2CBusMgr[I2Cx].BusState;
}

//I2C�������� ���ؾ��handle
unsigned short I2CBusNew::I2C_Task_Request(unsigned char OpMode, st_I2CModule * pI2CModule)
{
    unsigned char I2Cx = pI2CModule->I2Cx;
    if(I2Cx!=I2C0 && I2Cx!=I2C1&& I2Cx!=I2C2)
        return 0;
    if(m_I2CBuf[I2Cx].m_u32Length>=MaxRequestNum)  //������
        return 0;   //0�����������
    else  //���뻺����
    {
        I2CRequestHandle I2CHandle;
        HandleIdx++;  //update HandleIdx
        if(HandleIdx>MAXI2CHANDLE)
            HandleIdx = 1;
        I2CHandle.m_ID = HandleIdx;
        pI2CModule->HandleID = HandleIdx;
        I2CHandle.m_Status = I2C_ReqRdy;
        ENQUEUE(m_I2CBuf[I2Cx],MaxRequestNum,*pI2CModule);
        ENQUEUE(m_qI2CHandle,MAXI2CHANDLE,I2CHandle);
        m_I2CTaskMgr[I2Cx].RequestCnt++;
        return I2CHandle.m_ID;
    }
}


I2C_Handle_Status I2CBusNew::GetRequestStatus(unsigned short Handle)
{
    for(unsigned short i = 0; i<MAXI2CHANDLE; i++)
    {
        if(m_qI2CHandle.Object[i].m_ID == Handle)
            return m_qI2CHandle.Object[i].m_Status;
    }
    return I2C_ReqErr;
}
#endif
void I2CBusNew::SetRequestStatus(unsigned short Handle,I2C_Handle_Status status)
{
    for(unsigned short i = 0; i<MAXI2CHANDLE; i++)
    {
        if(m_qI2CHandle.Object[i].m_ID == Handle)
        {
            m_qI2CHandle.Object[i].m_Status = status ;
            break;
        }
    }
}
//ÿ�δ����꣬��������ȡ����Ȼ��m_I2CTaskMgr[I2Cx].I2CBusy == I2C_NOT_BUSY
bool I2CBusNew::I2C_ImmidiateTrans(st_I2CModule *I2CMoudle)
{
    unsigned char I2Cx = I2CMoudle->I2Cx;
    if(m_I2CBusMgr[I2Cx].I2CBusy == I2C_NOT_BUSY)  //���Ϊ����״̬�Ƿ�Ӧ�ñ��棬���ж�
        m_I2CBusMgr[I2Cx].I2CBusy = I2C_IMME_BUSY;
    else
        return false;
    I2CMoudle->HandleID = 0;  // ֱ�������IDΪ0
    memcpy(&RunningBuffer[I2Cx],I2CMoudle,sizeof(st_I2CModule));
    InitFirstBuf(I2CMoudle); //
    if(!I2CBusNew::I2CStart(I2Cx))
    {
        I2CStop(I2Cx);
        m_I2CBusMgr[I2Cx].I2CBusy = I2C_NOT_BUSY;
        return false;
    }
    CommonFunc::GetTimeTicksUpdate( &m_I2CBusMgr[I2Cx].Overtime.TransOvertime);
    bool ReturnVal = false;
    for(;;)  //while(1)
    {
        if(CommonFunc::GetTimeTicks( &m_I2CBusMgr[I2Cx].Overtime.TransOvertime) > I2CConfig[I2Cx].TransDelaytime)// ��ʱ xms
        {
            I2CStop(I2Cx);
            CommonFunc::GetTimeTicksUpdate( &m_I2CBusMgr[I2Cx].Overtime.TransOvertime);
            ReturnVal = false;
            break;
        }
        else if(m_I2CBusMgr[I2Cx].BusState == I2CBus_NACK)
        {
            I2CStop(I2Cx);
            ReturnVal = false;
            break;
        }
        else if(m_I2CBusMgr[I2Cx].BusState == I2CBus_STOPPED
                &&m_I2CBusMgr[I2Cx].StopCondition == I2CNormalStop)
        {
            if(PrepareNextBuf(I2CMoudle))  //��ȫ��������
            {
                ReturnVal = true;
                break;
            }
            else  //����pagesize
            {
                TIMERSFunc::DELAY_SYS(120000, 0); 	//��һ�δ����ӳ�
                if(!I2CBusNew::I2CStart(I2Cx))
                {
                    I2CStop(I2Cx);
                    ReturnVal = false;
                    break;
                }
                CommonFunc::GetTimeTicksUpdate( &m_I2CBusMgr[I2Cx].Overtime.TransOvertime);
            }
        }
    }//while

    m_I2CBusMgr[I2Cx].I2CBusy = I2C_NOT_BUSY;
    m_I2CBusMgr[I2Cx].BusState= I2CBus_IDLE; //
    TIMERSFunc::DELAY_SYS(120000, 0); 	//��һ�δ����ӳ�
    return ReturnVal;
}

bool I2CBusNew::I2CStart(unsigned char I2Cx )
{
    LPC_I2C_TypeDef  *pI2Cx;
    if(I2Cx == I2C0)
        pI2Cx = LPC_I2C0;
    else if(I2Cx == I2C1)
        pI2Cx = LPC_I2C1;
    else if(I2Cx == I2C2)
        pI2Cx = LPC_I2C2;
    else
        return false;
    pI2Cx->CONSET = I2CONSET_STA;	// Set Start flag
    CommonFunc::GetTimeTicksUpdate(&m_I2CBusMgr[I2Cx].Overtime.StartOvertime);
    /*--- Wait until START transmitted ---*/
    while( 1 )
    {
        if(I2CBusNew::m_I2CBusMgr[I2Cx].BusState == I2CBus_STARTED)
        {
            return true;
        }
        if(CommonFunc::GetTimeTicks( &m_I2CBusMgr[I2Cx].Overtime.StartOvertime) > const_StartOvertime)	// ��ʱ1ms
        {
            pI2Cx->CONCLR = I2CONCLR_STAC;
            return false;
        }
    }
}

bool I2CBusNew::I2CStop( unsigned char I2Cx)
{
    LPC_I2C_TypeDef  *pI2Cx;
    if(I2Cx == I2C0)
        pI2Cx = LPC_I2C0;
    else if(I2Cx == I2C1)
        pI2Cx = LPC_I2C1;
    else if(I2Cx == I2C2)
        pI2Cx = LPC_I2C2;
    else
        return false;

    pI2Cx->CONSET = I2CONSET_STO;      // Set Stop flag //
    pI2Cx->CONCLR = I2CONCLR_SIC;      // Clear SI flag //
    CommonFunc::GetTimeTicksUpdate(&m_I2CBusMgr[I2Cx].Overtime.StopOvertime);
    while( pI2Cx->CONSET & I2CONSET_STO ) //wait the stop flag of control set
    {
        if(CommonFunc::GetTimeTicks( &m_I2CBusMgr[I2Cx].Overtime.StopOvertime) > const_StopOvertime)
        {
            return ( false );
        }
    }
    TIMERSFunc::DELAY_SYS(1000, 0);          //
    return true;
}
//�׸����ݿ鴦��
void I2CBusNew::InitFirstBuf(st_I2CModule *I2CMoudle)
{
    unsigned char I2Cx = I2CMoudle->I2Cx;
    m_I2CTaskMgr[I2Cx].CurrentPackNo = 0;
    //if(I2CMoudle->BufLen == 0)
    //		return;
    //2014 05 22
    m_I2CTaskMgr[I2Cx].PageSize = MAX_FRAMELEN;//��ʱд��
    unsigned short offsetAdr =  I2CMoudle->MasterCtrlByte.SLA_Adr.WRRegAdr%m_I2CTaskMgr[I2Cx].PageSize;  //�װ�ƫ�Ƶ�ַ����
    unsigned short offsetBufLen = 	I2CMoudle->BufLen+offsetAdr;
    if(offsetBufLen%m_I2CTaskMgr[I2Cx].PageSize == 0)
        m_I2CTaskMgr[I2Cx].SubPackNum = offsetBufLen/m_I2CTaskMgr[I2Cx].PageSize;
    else
        m_I2CTaskMgr[I2Cx].SubPackNum = offsetBufLen/m_I2CTaskMgr[I2Cx].PageSize+1;
    if(m_I2CTaskMgr[I2Cx].SubPackNum < 1)
        m_I2CTaskMgr[I2Cx].SubPackNum = 1;
    if(m_I2CTaskMgr[I2Cx].SubPackNum == 1)  //���ϵ�ַƫ��Ҳֻ��һ����
        m_I2CTaskMgr[I2Cx].SubPackLen =  I2CMoudle->BufLen;
    else
        m_I2CTaskMgr[I2Cx].SubPackLen = m_I2CTaskMgr[I2Cx].PageSize - offsetAdr;	//���ϵ�ַƫ��,��ҳ��д��

    for(unsigned char i = 0; i<I2CMoudle->CtrlByteNum; i++) //����ƣ� <8byte
    {
        m_I2CTaskMgr[I2Cx].CurrentAdr[I2CMoudle->CtrlByteNum-i-1] = I2CMoudle->MasterCtrlByte.SLA_Adr.WRRegAdr>>(8*i);
    }
    RunningBuffer[I2Cx].BufIdx = 0;    // initial
    m_I2CTaskMgr[I2Cx].SubPackStartIdx = RunningBuffer[I2Cx].BufIdx; //retry ��
    m_I2CTaskMgr[I2Cx].SubBufEndingAdr = m_I2CTaskMgr[I2Cx].SubPackLen; //�ӿ�buffer��ֹƫ�Ƶ�ַ
    m_I2CBusMgr[I2Cx].SlaveAdr = I2CMoudle->MasterCtrlByte.SLA_Adr.SlaveAdr;
    m_I2CBusMgr[I2Cx].CtrlByteIdx = 0;
    m_I2CBusMgr[I2Cx].CtrlByteNum = I2CMoudle->CtrlByteNum;
    m_I2CTaskMgr[I2Cx].CurrentHandle =  I2CMoudle->HandleID;   //����Handle
    m_I2CTaskMgr[I2Cx].RetryCnt = 0;
}

//������һ�����ĵĳ���,��ַƫ�Ƶ�
//true ���һ���ӿ飬false������һ��
bool I2CBusNew::PrepareNextBuf(st_I2CModule *I2CMoudle)
{
    unsigned char I2Cx = I2CMoudle->I2Cx;
    m_I2CTaskMgr[I2Cx].CurrentPackNo++;
    SetRequestStatus(m_I2CTaskMgr[I2Cx].CurrentHandle,I2C_ReqRun);
    m_I2CTaskMgr[I2Cx].RetryCnt = 0;
    unsigned short LastSubPackLen = m_I2CTaskMgr[I2Cx].SubPackLen;
    if(m_I2CTaskMgr[I2Cx].CurrentPackNo==m_I2CTaskMgr[I2Cx].SubPackNum)  //�ѷ�����
    {
        m_I2CBusMgr[I2Cx].CtrlByteIdx = 0;
        m_I2CBusMgr[I2Cx].CtrlByteNum = 0;
        return true;
    }
    else if(m_I2CTaskMgr[I2Cx].CurrentPackNo ==m_I2CTaskMgr[I2Cx].SubPackNum -1)  //����е����һ�����ݿ飬����δ��(MaxFrameLen)
    {
        m_I2CTaskMgr[I2Cx].SubPackLen  = I2CMoudle->BufLen - RunningBuffer[I2Cx].BufIdx;
    }
    else
        m_I2CTaskMgr[I2Cx].SubPackLen = m_I2CTaskMgr[I2Cx].PageSize;

    I2CMoudle->MasterCtrlByte.SLA_Adr.WRRegAdr+= LastSubPackLen;   //����ڴ洢���ĵ�ַƫ��!!!  LastSubPackLen
    for(unsigned char i = 0; i<I2CMoudle->CtrlByteNum; i++)
    {
        m_I2CTaskMgr[I2Cx].CurrentAdr[I2CMoudle->CtrlByteNum-i-1] = I2CMoudle->MasterCtrlByte.SLA_Adr.WRRegAdr>>(8*i);
    }
    m_I2CTaskMgr[I2Cx].SubPackStartIdx = RunningBuffer[I2Cx].BufIdx;   //retry ��
    m_I2CTaskMgr[I2Cx].SubBufEndingAdr +=  m_I2CTaskMgr[I2Cx].SubPackLen;  //����Bufƫ��
    m_I2CBusMgr[I2Cx].CtrlByteIdx = 0;
    m_I2CBusMgr[I2Cx].CtrlByteNum = I2CMoudle->CtrlByteNum;
    return false;
}

void I2CBusNew::RetryCurrentBuf(unsigned char I2Cx)
{
    //���������Դ�m_I2CTaskMgr��ȡ
    m_I2CBusMgr[I2Cx].CtrlByteIdx = 0;
    //�޸�BufIdx
    RunningBuffer[I2Cx].BufIdx = m_I2CTaskMgr[I2Cx].SubPackStartIdx;
    m_I2CTaskMgr[I2Cx].RetryCnt++;
    if(m_I2CTaskMgr[I2Cx].RetryCnt >=MAXI2CRETRY)
    {
        m_I2CTaskMgr[I2Cx].I2C_TaskState = I2C_Task_ReportError;
        m_I2CTaskMgr[I2Cx].RetryCnt = 0;
    }
    else
        m_I2CTaskMgr[I2Cx].I2C_TaskState = I2C_Task_DataTransfer;
}
//test 24lc128 read
//caution ��ַ��ҳ����
// unsigned short I2CBusNew::I2C_WriteTest(unsigned short adr,unsigned char iData,unsigned short Len)
// {
// 	unsigned short Handle = 0;
// 	if(Len>MAXI2CBUFLEN)
// 		return 0;
// 	st_I2CModule  I2Cmodule_a;
// 	I2Cmodule_a.I2Cx = I2C1;
// 	I2Cmodule_a.CtrlByteNum = 2;
// 	I2Cmodule_a.BufLen = Len;
// 	//WRITE
// 	for(unsigned int i = 0;i<Len;i++)
// 	{
// 		I2Cmodule_a.Buf[i] = i%255;
// 		if(iData)
// 		I2Cmodule_a.Buf[i] = iData;
// 	}
// 	I2Cmodule_a.MasterCtrlByte.SLA_Adr.SlaveAdr  =0xa0|(7<<1);
// 	I2Cmodule_a.MasterCtrlByte.SLA_Adr.WRRegAdr = adr;
// 	I2Cmodule_a.OPMode = I2C_MASTWRITE;
// 	Handle = I2CBusNew::I2C_Task_Request(I2C_MASTWRITE,&I2Cmodule_a);
// 	//I2C_ImmidiateTrans(&I2Cmodule_a);
// 	return Handle;
//
// }
// //read
// unsigned short I2CBusNew::I2C_ReadTest(unsigned short adr,unsigned short Len)
// {
// 	unsigned short Handle = 0;
// 	if(Len>MAXI2CBUFLEN)
// 	return 0;
// 	st_I2CModule  I2Cmodule_a;
// 	I2Cmodule_a.I2Cx = I2C1;
// 	I2Cmodule_a.CtrlByteNum = 2;
// 	I2Cmodule_a.BufLen = Len;
// 	memset(I2Cmodule_a.Buf,Len,0);
// 	static unsigned short MyRAdr = adr;
// 	I2Cmodule_a.MasterCtrlByte.SLA_Adr.SlaveAdr  =0xa0|(7<<1);
// 	I2Cmodule_a.MasterCtrlByte.SLA_Adr.WRRegAdr = MyRAdr;
// 	I2Cmodule_a.OPMode = I2C_MASTREAD;
// 	I2Cmodule_a.PageSize = 64;
// 	Handle = I2CBusNew::I2C_Task_Request(I2C_MASTREAD,&I2Cmodule_a);
// 	return Handle;
// }
// void I2CBusNew::LM75BTEST(unsigned char I2Cx,unsigned char  adr)
// {
// 	st_I2CModule  I2Cmodule_a;
// 	I2Cmodule_a.I2Cx = I2C1;
// 	I2Cmodule_a.CtrlByteNum = 1;//POINTER BYTE
// 	I2Cmodule_a.BufLen = 2;
// 	memset(I2Cmodule_a.Buf,I2Cmodule_a.BufLen,0);
// 	I2Cmodule_a.MasterCtrlByte.SLA_Adr.SlaveAdr  =0x90|(adr<<1);
// 	I2Cmodule_a.MasterCtrlByte.SLA_Adr.WRRegAdr = 0x0000;
// 	I2Cmodule_a.OPMode = I2C_MASTREAD;
// 	I2CBusNew::I2C_Task_Request(I2C_MASTREAD,&I2Cmodule_a);
// }

bool I2CBusNew::AD5245_SET(unsigned short adr,unsigned char data,unsigned char I2C_Port)
{
    st_I2CModule  I2Cmodule_a;
    I2Cmodule_a.I2Cx = I2C_Port;
    I2Cmodule_a.CtrlByteNum = 1;//POINTER BYTE
    I2Cmodule_a.BufLen = 1;
    memset(I2Cmodule_a.Buf,I2Cmodule_a.BufLen,0);
    I2Cmodule_a.MasterCtrlByte.SLA_Adr.SlaveAdr  =0x58|adr;
    I2Cmodule_a.MasterCtrlByte.SLA_Adr.WRRegAdr = 0x0000;
    I2Cmodule_a.OPMode = I2C_MASTWRITE;
    I2Cmodule_a.Buf[0] = data;
    if(I2CBusNew::I2C_ImmidiateTrans(&I2Cmodule_a))
    {
        //unlock
        I2CBusNew::m_I2CBusMgr[I2Cmodule_a.I2Cx].I2CBusy = I2C_NOT_BUSY;
        return true;

    }
    else
        return false;//ʧ��
}
