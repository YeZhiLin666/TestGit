//AUX

#include "../Entry/includes.h"


unsigned short AuxthirdParty::Priority = PRIORITY4;
unsigned short AuxthirdParty::TaskID = AUXTHIRDPARTY_TASK;			// should be a definition which positioning this task in OS_Task[]

// ���ñ������������
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x03E80000
#endif
unsigned char AuxthirdParty::m_Auxstatus[MAX_AUXCH_PER_AUXMCU];	//״̬��	8bit
st_AUXThird_Party_Msg AuxthirdParty::st_Auxthird_party_msg[MAX_AUXCH_PER_AUXMCU];
unsigned char AuxthirdParty::m_AuxmsgFlag[MAX_AUXCH_PER_AUXMCU];
st_AUXThird_Handle AuxthirdParty::st_Auxthird_handle[MAX_AUXCH_PER_AUXMCU];


#pragma arm section //��������������

unsigned char AuxthirdParty::m_ActualAuxChannel;

void AuxthirdParty::Init(void)
{

    for(unsigned char uc_i = 0; uc_i < m_ActualAuxChannel; uc_i++)
    {
        setStatus(uc_i,AUXTHIRD_PARTY_STATUS_INACTIVE);
        m_AuxmsgFlag[uc_i] = 0;
        st_Auxthird_handle[uc_i].handleID = 0;
        st_Auxthird_handle[uc_i].type = 0;

    }


}
void AuxthirdParty::DoWork(void)
{
    for(unsigned char i = 0; i<m_ActualAuxChannel; i++)
    {
        processMSG(i);	  //���յ�ǰ״̬��������msg
        processFSM(i);	  //���յ�ǰ״̬����һЩˢ�ºʹ�����
    }

}
void AuxthirdParty::EndTask(void)
{
}
unsigned char AuxthirdParty::sendMsgToMe(const st_AUXThird_Party_Msg * msg)
{
    //��msg ���ڴ�λ��
    unsigned char chn = msg->chn;
    if(chn >MAX_AUXMCU_PER_IVMCU)
    {
        chn %= MAX_AUXMCU_PER_IVMCU;
    }

    st_Auxthird_party_msg[chn].chn = msg->chn;
    st_Auxthird_party_msg[chn].st_Auxthird_party_cmd = msg->st_Auxthird_party_cmd;
    st_Auxthird_party_msg[chn].TaskID = msg->TaskID;


    m_AuxmsgFlag[chn] = 1;
    return 1;
}
unsigned char AuxthirdParty::setStatus(unsigned char chn,unsigned char status)
{
    m_Auxstatus[chn] = status;

    if(status == AUXTHIRD_PARTY_STATUS_INACTIVE)    //�������δ����
    {
        //δ�������
    }
    return 1;
}

signed char AuxthirdParty::getStatus(unsigned char chn)
{
    return (signed char)(m_Auxstatus[chn]);
}

void AuxthirdParty::SoftWare_WatchDog(void)
{

}
void AuxthirdParty::processMSG(unsigned char chn)
{
    if(m_AuxmsgFlag[chn] == 1)
    {

        switch(getStatus(chn))	  //��ͬ״̬���ܲ�ͬ����
        {
        case AUXTHIRD_PARTY_STATUS_IDLE:	 //idle ����CC������ת����Ϊtran״̬

            switch(st_Auxthird_party_msg[chn].st_Auxthird_party_cmd.cmd)
            {

            case AUXTHIRD_PARTY_AUX_VOLTAGE:
            case AUXTHIRD_PARTY_AUX_TEMPERATURE:
            default:
                setStatus(chn,AUXTHIRD_PARTY_STATUS_IDLE);    //donothing
                break;
            }

            m_AuxmsgFlag[chn] = 0;    //clear
            return;    //break THIRD_PARTY_STATUS_IDLE

        case AUXTHIRD_PARTY_STATUS_TRAN:
            switch(st_Auxthird_party_msg[chn].st_Auxthird_party_cmd.cmd)
            {
                //DI/DO�ȵ��������

            }
            m_AuxmsgFlag[chn] = 0;    //clear
            return;//break	THIRD_PARTY_STATUS_TRAN

        case AUXTHIRD_PARTY_STATUS_OUTPUT: 	//���״̬

            switch(st_Auxthird_party_msg[chn].st_Auxthird_party_cmd.cmd)
            {
                //DI/DO�ȵ��������

            default:
                //donothing
                break;
            }

            m_AuxmsgFlag[chn] = 0;    //clear
            return;    //break	  THIRD_PARTY_STATUS_OUTPUT

        case AUXTHIRD_PARTY_STATUS_INACTIVE:
            switch(st_Auxthird_party_msg[chn].st_Auxthird_party_cmd.cmd)
            {
                //DI/DO�ȵ��������

            default:
                //donothing
                break;
            }

            m_AuxmsgFlag[chn] = 0;    //clear
            return;    //break	  THIRD_PARTY_STATUS_OUTPUT

        default:	//��ֹ�������
            m_Auxstatus[chn] = AUXTHIRD_PARTY_STATUS_INACTIVE;	  //תΪδ������
            m_AuxmsgFlag[chn] = 0;//clear
            return;
        }
    }
    else	  //Ϊ��
    {
        return;
    }

}
void AuxthirdParty::processFSM(unsigned char chn)
{
    unsigned char status = getStatus(0);	//��õ�ǰ��״̬

    switch(status)
    {
    case AUXTHIRD_PARTY_STATUS_INACTIVE:

        break;
    case AUXTHIRD_PARTY_STATUS_IDLE:	 //�����Ӧ����OFFģʽ

        break;
    case AUXTHIRD_PARTY_STATUS_TRAN:

        break;
    case AUXTHIRD_PARTY_STATUS_OUTPUT:

        break;
    default:
        Init();    //���³�ʼ�����������
        break;
    }



}



void AuxthirdParty::clearMSG(char chn)
{
    memset(&st_Auxthird_party_msg[chn], 0, sizeof(st_AUXThird_Party_Msg));

}


