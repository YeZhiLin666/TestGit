#include "includes.h"
#include "ADC101C027.h"


void ADC101C027::ADC101C027_Init(unsigned char I2Cx)
{
	 ADC101C027_WriteInterReg1(I2Cx, 0, 0xB8, ADC101C027_ADDR_CONFG);		//1.7KSPS, Alerts will not self-clear, alert status enable, ALERT pin Disable.
	 ADC101C027_WriteInterReg2(I2Cx, 0, 0x0000, ADC101C027_ADDR_LOWL);
	 ADC101C027_WriteInterReg2(I2Cx, 0, 0x0FFC, ADC101C027_ADDR_HIGHL);
	 ADC101C027_WriteInterReg2(I2Cx, 0, 0x01FC, ADC101C027_ADDR_HYST);
	 ADC101C027_WriteInterReg1(I2Cx, 1, 0xB8, ADC101C027_ADDR_CONFG);		
	 ADC101C027_WriteInterReg2(I2Cx, 1, 0x0000, ADC101C027_ADDR_LOWL);
	 ADC101C027_WriteInterReg2(I2Cx, 1, 0x0FFC, ADC101C027_ADDR_HIGHL);
	 ADC101C027_WriteInterReg2(I2Cx, 1, 0x01FC, ADC101C027_ADDR_HYST);	
}

ADC101C027_TypeDef ADC101C027::ADC101C027_Read(unsigned char I2Cx,unsigned char chan)
{
	 ADC101C027_TypeDef Vol = {0,0,0};
	 unsigned short OutCode = 0;
	 OutCode = ADC101C027_ReadInterReg(I2Cx, chan, ADC101C027_ADDR_CONV, 2) >> 2;
	 Vol.Result = OutCode & 0x03FF;
	 Vol.AlertFlag = OutCode >> 13;
	 if(Vol.AlertFlag)
	 {
		 Vol.AlertStatus = ADC101C027_ReadInterReg(I2Cx, chan, ADC101C027_ADDR_ALERT,1);
		 ADC101C027_WriteInterReg1(I2Cx, chan, Vol.AlertStatus,ADC101C027_ADDR_ALERT);
	 }
	 return Vol;
}

unsigned char ADC101C027::ADC101C027_WriteInterReg1(unsigned char I2Cx, unsigned char chan, unsigned char Value, unsigned char Addr)
{
	st_I2CModule  I2Cmodule_a;
	I2Cmodule_a.I2Cx = I2Cx;
	I2Cmodule_a.CtrlByteNum = 1;//POINTER BYTE
	I2Cmodule_a.BufLen = 1;
	memset(I2Cmodule_a.Buf,I2Cmodule_a.BufLen,0);
	if(chan==0)
		I2Cmodule_a.MasterCtrlByte.SLA_Adr.SlaveAdr  = (0x51 << 1);
	else if(chan==1)
		I2Cmodule_a.MasterCtrlByte.SLA_Adr.SlaveAdr  = (0x52 << 1);
	I2Cmodule_a.MasterCtrlByte.SLA_Adr.WRRegAdr = Addr;
	I2Cmodule_a.Buf[0] = Value;	
	I2Cmodule_a.OPMode = I2C_MASTWRITE;
	if(I2CBusNew::I2C_ImmidiateTrans(&I2Cmodule_a))
	{
		//unlock
		I2CBusNew::m_I2CBusMgr[I2Cmodule_a.I2Cx].I2CBusy = I2C_NOT_BUSY;
		return 0 ;

	}
	else
		return 1;//Ê§°Ü
}
 
unsigned char ADC101C027::ADC101C027_WriteInterReg2(unsigned char I2Cx, unsigned char chan, unsigned short Value, unsigned char Addr)
{
	 unsigned short ADC_Input = 0;	
	 ADC_Input = (Value & 0x3FF) << 2;
	 
	st_I2CModule  I2Cmodule_a;
	I2Cmodule_a.I2Cx = I2Cx;
	I2Cmodule_a.CtrlByteNum = 1;//POINTER BYTE
	I2Cmodule_a.BufLen = 2;
	memset(I2Cmodule_a.Buf,I2Cmodule_a.BufLen,0);
	if(chan==0)
		I2Cmodule_a.MasterCtrlByte.SLA_Adr.SlaveAdr  = (0x51 << 1);
	else if(chan==1)
		I2Cmodule_a.MasterCtrlByte.SLA_Adr.SlaveAdr  = (0x52 << 1);
	I2Cmodule_a.MasterCtrlByte.SLA_Adr.WRRegAdr = Addr;
	I2Cmodule_a.Buf[0] = (uint8_t)((ADC_Input>>8) & 0x0F);
	I2Cmodule_a.Buf[1] = (uint8_t)(ADC_Input & 0xFC);
	I2Cmodule_a.OPMode = I2C_MASTWRITE;
	if(I2CBusNew::I2C_ImmidiateTrans(&I2Cmodule_a))
	{
		//unlock
		I2CBusNew::m_I2CBusMgr[I2Cmodule_a.I2Cx].I2CBusy = I2C_NOT_BUSY;
		return 0 ;

	}
	else
		return 1;//Ê§°Ü
}
 
 unsigned short ADC101C027::ADC101C027_ReadInterReg(unsigned char I2Cx, unsigned char chan, unsigned char Addr, unsigned char Len)
 {
	 unsigned short ADC_Output = 0;
	 st_I2CModule  I2Cmodule_a;
	 uint8_t Buf[2] = {0,0};
	 
	 if((Len != 0x01) && (Len != 0x02))
		 return 0;
	 
	I2Cmodule_a.I2Cx = I2Cx;
	I2Cmodule_a.CtrlByteNum = 1;
	I2Cmodule_a.BufLen = Len;
	memset(&Buf[0],2,0);
	if(chan==0)
	{
		I2Cmodule_a.MasterCtrlByte.SLA_Adr.SlaveAdr  = (0x51 << 1);
	}
	else if(chan==1)
	{
		I2Cmodule_a.MasterCtrlByte.SLA_Adr.SlaveAdr  = (0x52 << 1);
	}
	I2Cmodule_a.MasterCtrlByte.SLA_Adr.WRRegAdr = Addr;
	I2Cmodule_a.OPMode = I2C_MASTREAD;
	I2Cmodule_a.PageSize = 64;
	if(I2CBusNew::I2C_ImmidiateTrans(&I2Cmodule_a))
	{				
		//buffer
		memcpy(Buf,I2CBusNew::RunningBuffer[I2Cmodule_a.I2Cx].Buf,Len);
		if(Len == 1)
		 ADC_Output = Buf[0];
	 else
		 ADC_Output = (Buf[0] << 8) | Buf[1];
		return ADC_Output;
	}
	else
	{	
		return 0;
	}
 }
