#include "../../Entry/includes.h"
#include <string.h>

#define SEND_CMD_BUF_SIZE 8
#define STOP 	0
#define RUN 	1

extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function ����ָ��   //��ʽ  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

const char* My_strstr(const char* str1, const char* str2) 
{ 
  while (*str1 != '\0') 
  { 
    const char* p1 = str1; 
    const char* p2 = str2; 
    while ((*p1 != '\0') && (*p2 != '\0') && (*p1 == *p2)) 
    { 
      ++p1; 
      ++p2; 
    } 
    if (*p2 == '\0') 
    { 
      return str1; 
    } 
    ++str1; 
  } 
  return 0; 
}

char* TH_HITACHI_EC15MHP::itoa(int num,char* str,int radix)
{/*???*/
    char index[]="0123456789ABCDEF";
    unsigned unum;/*????*/
    int i=0,j,k;
    /*??unum??*/
    if(radix==10&&num<0)/*?????*/
    {
        unum=(unsigned)-num;
        str[i++]='-';
    }
    else unum=(unsigned)num;/*????*/
    /*??*/
    do{
        str[i++]=index[unum%(unsigned)radix];
        unum/=radix;
       }while(unum);
    str[i]='\0';
    /*??*/
    if(str[0]=='-')
        k=1;/*?????*/
    else
        k=0;
     
    for(j=k;j<=(i-1)/2;j++)
    {       char temp;
        temp=str[j];
        str[j]=str[i-1+k-j];
        str[i-1+k-j]=temp;
    }
    return str;
}

void TH_HITACHI_EC15MHP::Init(unsigned char ChanNo)
{
    // ��ʼ������ָ�롣
    unsigned char uc_i; // ��ֹ�����������ȫ����ʼ��һ�κ���ָ�롣
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_HITACHI_EC15MHP::SendRunCmd);   //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_HITACHI_EC15MHP::SendWriteCmd);  //  1-M)�����¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_HITACHI_EC15MHP::SendReadCmd);    //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_HITACHI_EC15MHP::SendRunCmd);      //  3-M)Ҫ�����������������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_HITACHI_EC15MHP::SendStopCmd);    //  4-M)Ҫ��ֹͣ�����������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_HITACHI_EC15MHP::Parse);  //  5-S)��ȡ�¶ȴӻ��ظ���������  (���ֽڲ���)
}

/***************************************************************
*   Parse the feedback from temperature chamber                *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/
void TH_HITACHI_EC15MHP::Parse(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    short PresentValue = 0;
    unsigned short int uw16_i = 0;

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    UARTFunc::uart_getcmd(uc_ChNo,MTCI_CTRL::gR_CmdBuf);

    do 
		{
        if((MTCI_CTRL::gR_CmdBuf[uw16_i] == 'M') && (MTCI_CTRL::gR_CmdBuf[uw16_i + 1] == 'N') && (MTCI_CTRL::gR_CmdBuf[uw16_i + 2] == ' ') && (MTCI_CTRL::gR_CmdBuf[uw16_i + 3] == 'T'))//������
        {
						PresentValue = (short)(atof((const char*)(MTCI_CTRL::gR_CmdBuf + uw16_i + 10))*10);
						MTCI_CTRL::SetPresentValue(uc_ChNo,PresentValue,SETPOINTVALUE_TEMPERATURE, FIRST_CHAMBER_CHANNEL);

						PresentValue = (short)(atoi((const char*)(MTCI_CTRL::gR_CmdBuf + uw16_i + 28))*10);
						MTCI_CTRL::SetPresentValue(uc_ChNo,PresentValue,SETPOINTVALUE_HUMI, FIRST_CHAMBER_CHANNEL);
            
            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[uc_ChNo]);     //update time out stamp
            return;
				}

        uw16_i++;
    } while((MTCI_CTRL::gR_CmdBuf[uw16_i] != 0) && (uw16_i + 3 < sizeof(MTCI_CTRL::gR_CmdBuf))) ;
}

/**************************************************************
*   Send "write" command to temperature chamber.              *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TH_HITACHI_EC15MHP::SendWriteCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
		char temp[10] = {0};
		char CmdBuf[20] = {0};
		char FixCmdBuf[] = "C3=T\r";

		if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)//�¶�
		{
			strcpy(CmdBuf,"C6=");
			//itoa(MTCI_CTRL::gW_TemperatureValue[uc_ChNo]/10,temp,10);
			sprintf(temp,"%.1f",(float)MTCI_CTRL::gW_TemperatureValue[uc_ChNo][FIRST_CHAMBER_CHANNEL]/10);
		}
		else if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)//ʪ��
		{
			strcpy(CmdBuf,"C7=");
			//itoa(MTCI_CTRL::gW_HumiValue[uc_ChNo]/10,temp,10);
			sprintf(temp,"%d",MTCI_CTRL::gW_HumiValue[uc_ChNo][FIRST_CHAMBER_CHANNEL]/10);
		}
		
		strcat(CmdBuf,temp);
		strcat(CmdBuf,"\r");

		UARTFunc::uart_putcmd(uc_ChNo,(unsigned char*)FixCmdBuf,strlen(FixCmdBuf));
		DELAY2(5000);
    UARTFunc::uart_putcmd(uc_ChNo,(unsigned char*)CmdBuf,strlen(CmdBuf));
		CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[uc_ChNo]);
}

/**************************************************************
*   Send "read" command to temperature chamber.               *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TH_HITACHI_EC15MHP::SendReadCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
		char CmdBuf[] = "C2\r";
	
    UARTFunc::uart_putcmd(uc_ChNo,(unsigned char*)CmdBuf,strlen(CmdBuf));
		CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[uc_ChNo]);
}

/**************************************************************
*   Send "run" command to temperature chamber.                *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TH_HITACHI_EC15MHP::SendRunCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    char CmdBuf[] = "C1=ON\r";

    UARTFunc::uart_putcmd(uc_ChNo,(unsigned char*)CmdBuf,strlen(CmdBuf));
		CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[uc_ChNo]);
}

/**************************************************************
*   Send "stop" command to temperature chamber.               *
*   In : n/a                                                   *
*   Out: void                                                  *
***************************************************************/
void TH_HITACHI_EC15MHP::SendStopCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    char CmdBuf[] = "C1=OFF\r";

    UARTFunc::uart_putcmd(uc_ChNo,(unsigned char*)CmdBuf,strlen(CmdBuf));
		CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[uc_ChNo]);
}