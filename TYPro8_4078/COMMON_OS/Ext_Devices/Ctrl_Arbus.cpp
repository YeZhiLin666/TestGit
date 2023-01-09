/*
Ctrl_Arbus.cpp

Arbin Bus Write/Read
*/


#include "../../COMMON/includes.h"


/*----*/
void Ctrl_Arbus::Init(void)
{

}


/*----*/
void Ctrl_Arbus::WriteBus(uint16 addr,uint16 val)
{

    if(addr == 0xFFFF)return;

    //	// Disable interrupts for now
    IRQFunc::All_DisableIRQ();
    //
    cpu_iowr_16(ARBINBUS_DIR_ADDRESS,ARBINBUS_ARBINDIR_OUT); //zl,0

    cpu_iowr_16(ARBINBUS_CTRB_ADDRESS,0
                |ARBINBUS_CTRB_READ
                |ARBINBUS_CTRB_ADDRW
                |ARBINBUS_CTRB_DATAW
                |ARBINBUS_CTRB_ARBIN);   	 //zl, 0xf,

    cpu_iowr_16(ARBIN_BUS_WRITE_ADDRESS,addr);	// BUS=Addr

    ARBUS_DELAY(ARBIN_BUS_DELAY_M);				// WAIT(M)

    cpu_iowr_16(ARBINBUS_CTRB_ADDRESS,0			// ADDRW=0
                |ARBINBUS_CTRB_READ
                |ARBINBUS_CTRB_DATAW
                |ARBINBUS_CTRB_ARBIN);      //zl, 1011b

    ARBUS_DELAY(ARBIN_BUS_DELAY_L);				// WAIT(L)

    cpu_iowr_16(ARBINBUS_CTRB_ADDRESS,0			// ADDRW=1
                |ARBINBUS_CTRB_READ
                |ARBINBUS_CTRB_ADDRW
                |ARBINBUS_CTRB_DATAW
                |ARBINBUS_CTRB_ARBIN);     //zl, 0xf

    ARBUS_DELAY(ARBIN_BUS_DELAY_M);				// WAIT(M)

    cpu_iowr_16(ARBIN_BUS_WRITE_ADDRESS,val);   // BUS=Val

    ARBUS_DELAY(ARBIN_BUS_DELAY_M);				// WAIT(M)

    cpu_iowr_16(ARBINBUS_CTRB_ADDRESS,0			// DATAW=0
                |ARBINBUS_CTRB_READ
                |ARBINBUS_CTRB_ADDRW
                |ARBINBUS_CTRB_ARBIN); 	   //zl, 0111b

    ARBUS_DELAY(ARBIN_BUS_DELAY_S);				// WAIT(S)

    cpu_iowr_16(ARBINBUS_CTRB_ADDRESS,0			// ARBIN=0
                |ARBINBUS_CTRB_READ
                |ARBINBUS_CTRB_ADDRW);	   //zl, 0110b


    ARBUS_DELAY(ARBIN_BUS_DELAY_L);				// WAIT(L)


    cpu_iowr_16(ARBINBUS_CTRB_ADDRESS,0			// ARBIN=1
                |ARBINBUS_CTRB_READ
                |ARBINBUS_CTRB_ADDRW
                |ARBINBUS_CTRB_ARBIN);		//zl, 0111b

    ARBUS_DELAY(ARBIN_BUS_DELAY_S); 			// WAIT(S)

    cpu_iowr_16(ARBINBUS_CTRB_ADDRESS,0			// DATAW=1
                |ARBINBUS_CTRB_READ
                |ARBINBUS_CTRB_ADDRW
                |ARBINBUS_CTRB_DATAW
                |ARBINBUS_CTRB_ARBIN);      //zl,0xf

    cpu_iowr_16(ARBINBUS_DIR_ADDRESS,ARBINBUS_ARBINDIR_IN);    //zl, 0x1


    ////////////////////////////////////////////////////////////////////
    IRQFunc::All_EnableIRQ(); //enable interrupt
}


/*----*/
int16 Ctrl_Arbus::ReadBus(uint16 addr)
{
    if(addr == 0xFFFF)return 0;
    //
    int16 readvalue;
    //
    //	// Disable interrupts for now
    IRQFunc::All_DisableIRQ(); //keep the old interrupt status

    cpu_iowr_16(ARBINBUS_DIR_ADDRESS,ARBINBUS_ARBINDIR_OUT);

    cpu_iowr_16(ARBINBUS_CTRB_ADDRESS,0
                |ARBINBUS_CTRB_READ
                |ARBINBUS_CTRB_ADDRW
                |ARBINBUS_CTRB_DATAW
                |ARBINBUS_CTRB_ARBIN);


    cpu_iowr_16(ARBIN_BUS_WRITE_ADDRESS,addr);		// BUS=Addr

    ARBUS_DELAY(ARBIN_BUS_DELAY_M);					// WAIT(M)

    cpu_iowr_16(ARBINBUS_CTRB_ADDRESS,0				// ADDRW=0
                |ARBINBUS_CTRB_READ
                |ARBINBUS_CTRB_DATAW
                |ARBINBUS_CTRB_ARBIN);

    ARBUS_DELAY(ARBIN_BUS_DELAY_L);					// WAIT(L)

    cpu_iowr_16(ARBINBUS_CTRB_ADDRESS,0				// ADDRW=1
                |ARBINBUS_CTRB_READ
                |ARBINBUS_CTRB_ADDRW
                |ARBINBUS_CTRB_DATAW
                |ARBINBUS_CTRB_ARBIN);

    cpu_iowr_16(ARBIN_BUS_WRITE_ADDRESS,0);			// BUS=0

    cpu_iowr_16(ARBINBUS_DIR_ADDRESS,ARBINBUS_ARBINDIR_IN); // set ArbinBus IN

    cpu_iowr_16(ARBINBUS_CTRB_ADDRESS,0				// READ=0
                |ARBINBUS_CTRB_ADDRW
                |ARBINBUS_CTRB_DATAW
                |ARBINBUS_CTRB_ARBIN);

    ARBUS_DELAY(ARBIN_BUS_DELAY_S);					// WAIT(S)

    cpu_iowr_16(ARBINBUS_CTRB_ADDRESS,0				// ARBIN=0
                |ARBINBUS_CTRB_ADDRW
                |ARBINBUS_CTRB_DATAW);

    ARBUS_DELAY(ARBIN_BUS_DELAY_L);					// WAIT(L)

    readvalue=cpu_iord_16(ARBIN_BUS_READ_ADDRESS);	// BUS=>


    cpu_iowr_16(ARBINBUS_CTRB_ADDRESS,0				//READ=1 ARBIN=1
                |ARBINBUS_CTRB_READ
                |ARBINBUS_CTRB_ADDRW
                |ARBINBUS_CTRB_DATAW
                |ARBINBUS_CTRB_ARBIN);

    //	////////////////////////////////////////////////////////////////////
    IRQFunc::All_EnableIRQ(); //enable interrupt
    //
    return readvalue;
}




/*----*/
void Ctrl_Arbus::WriteBusX(uint16 addr,uint16 val)
{
    if(addr == 0xFFFF)return;
    //
    //	// Disable interrupts for now
    IRQFunc::All_DisableIRQ(); //keep the old interrupt status

    cpu_iowr_16(ARBINBUS_DIR_ADDRESS,ARBINBUS_ARBINDIR_OUT);

    cpu_iowr_16(ARBINBUS_CTRB_ADDRESS,0
                |ARBINBUS_CTRB_READ
                |ARBINBUS_CTRB_ADDRW
                |ARBINBUS_CTRB_DATAW
                |ARBINBUS_CTRB_ARBIN);


    cpu_iowr_16(ARBIN_BUS_WRITE_ADDRESS,addr);	// BUS=Addr

    ARBUS_DELAY(ARBIN_BUS_DELAY_MX);			// WAIT(M)

    cpu_iowr_16(ARBINBUS_CTRB_ADDRESS,0			// ADDRW=0
                |ARBINBUS_CTRB_READ
                |ARBINBUS_CTRB_DATAW
                |ARBINBUS_CTRB_ARBIN);

    ARBUS_DELAY(ARBIN_BUS_DELAY_LX)				// WAIT(L)

    cpu_iowr_16(ARBINBUS_CTRB_ADDRESS,0			// ADDRW=1
                |ARBINBUS_CTRB_READ
                |ARBINBUS_CTRB_ADDRW
                |ARBINBUS_CTRB_DATAW
                |ARBINBUS_CTRB_ARBIN);

    ARBUS_DELAY(ARBIN_BUS_DELAY_MX);			// WAIT(M)

    cpu_iowr_16(ARBIN_BUS_WRITE_ADDRESS,val);   // BUS=Val

    ARBUS_DELAY(ARBIN_BUS_DELAY_MX)				// WAIT(M)

    cpu_iowr_16(ARBINBUS_CTRB_ADDRESS,0			// DATAW=0
                |ARBINBUS_CTRB_READ
                |ARBINBUS_CTRB_ADDRW
                |ARBINBUS_CTRB_ARBIN);

    ARBUS_DELAY(ARBIN_BUS_DELAY_SX);			// WAIT(S)

    cpu_iowr_16(ARBINBUS_CTRB_ADDRESS,0			// ARBIN=0
                |ARBINBUS_CTRB_READ
                |ARBINBUS_CTRB_ADDRW);


    ARBUS_DELAY(ARBIN_BUS_DELAY_LX);			// WAIT(L)


    cpu_iowr_16(ARBINBUS_CTRB_ADDRESS,0			// ARBIN=1
                |ARBINBUS_CTRB_READ
                |ARBINBUS_CTRB_ADDRW
                |ARBINBUS_CTRB_ARBIN);

    ARBUS_DELAY(ARBIN_BUS_DELAY_SX);			// WAIT(S)

    cpu_iowr_16(ARBINBUS_CTRB_ADDRESS,0			// DATAW=1
                |ARBINBUS_CTRB_READ
                |ARBINBUS_CTRB_ADDRW
                |ARBINBUS_CTRB_DATAW
                |ARBINBUS_CTRB_ARBIN);

    cpu_iowr_16(ARBINBUS_DIR_ADDRESS,ARBINBUS_ARBINDIR_IN);
    ////////////////////////////////////////////////////////////////////

    IRQFunc::All_EnableIRQ(); //enable interrupt

}


/*----*/
int16 Ctrl_Arbus::ReadBusX(uint16 addr)
{
    if(addr == 0xFFFF)return 0;
    //
    int16 readvalue;
    //
    //	// Disable interrupts for now
    IRQFunc::All_DisableIRQ(); //keep the old interrupt status
    ///////////////////////////////////////////////////////////////////

    cpu_iowr_16(ARBINBUS_DIR_ADDRESS,ARBINBUS_ARBINDIR_OUT);

    cpu_iowr_16(ARBINBUS_CTRB_ADDRESS,0
                |ARBINBUS_CTRB_READ
                |ARBINBUS_CTRB_ADDRW
                |ARBINBUS_CTRB_DATAW
                |ARBINBUS_CTRB_ARBIN);


    cpu_iowr_16(ARBIN_BUS_WRITE_ADDRESS,addr);		// BUS=Addr

    ARBUS_DELAY(ARBIN_BUS_DELAY_MX);				// WAIT(M)

    cpu_iowr_16(ARBINBUS_CTRB_ADDRESS,0				// ADDRW=0
                |ARBINBUS_CTRB_READ
                |ARBINBUS_CTRB_DATAW
                |ARBINBUS_CTRB_ARBIN);

    ARBUS_DELAY(ARBIN_BUS_DELAY_LX);				// WAIT(L)

    cpu_iowr_16(ARBINBUS_CTRB_ADDRESS,0				// ADDRW=1
                |ARBINBUS_CTRB_READ
                |ARBINBUS_CTRB_ADDRW
                |ARBINBUS_CTRB_DATAW
                |ARBINBUS_CTRB_ARBIN);

    cpu_iowr_16(ARBIN_BUS_WRITE_ADDRESS,0);			// BUS=0

    cpu_iowr_16(ARBINBUS_DIR_ADDRESS,ARBINBUS_ARBINDIR_IN); // set ArbinBus IN

    cpu_iowr_16(ARBINBUS_CTRB_ADDRESS,0				// READ=0
                |ARBINBUS_CTRB_ADDRW
                |ARBINBUS_CTRB_DATAW
                |ARBINBUS_CTRB_ARBIN);

    ARBUS_DELAY(ARBIN_BUS_DELAY_SX);				// WAIT(S)

    cpu_iowr_16(ARBINBUS_CTRB_ADDRESS,0				// ARBIN=0
                |ARBINBUS_CTRB_ADDRW
                |ARBINBUS_CTRB_DATAW);

    ARBUS_DELAY(ARBIN_BUS_DELAY_LX);				// WAIT(L)

    readvalue=cpu_iord_16(ARBIN_BUS_READ_ADDRESS);	// BUS=>


    cpu_iowr_16(ARBINBUS_CTRB_ADDRESS,0				//READ=1 ARBIN=1
                |ARBINBUS_CTRB_READ
                |ARBINBUS_CTRB_ADDRW
                |ARBINBUS_CTRB_DATAW
                |ARBINBUS_CTRB_ARBIN);

    ////////////////////////////////////////////////////////////////////

    IRQFunc::All_EnableIRQ(); //enable interrupt
    //
    return readvalue;
}

/*----*/
void Ctrl_Arbus::ReadAdc(unsigned char Device_Number,unsigned long AdcNo, int16& Current, int16& Voltage)
{
    Adc16bit_AD7656::Adc_Read_Pair(Device_Number,AdcNo);
    Current =  Adc16bit_AD7656::ad7656[Device_Number].Adc_Buffer[2*AdcNo];
    Voltage	=  Adc16bit_AD7656::ad7656[Device_Number].Adc_Buffer[2*AdcNo + 1];

    if(Current ==0 && Voltage ==0) // 11.16.2010
    {
        Adc16bit_AD7656::Adc_Init(Device_Number);
        Adc16bit_AD7656::Adc_Read_Pair(Device_Number,AdcNo);
        Current =  Adc16bit_AD7656::ad7656[Device_Number].Adc_Buffer[2*AdcNo];
        Voltage	=  Adc16bit_AD7656::ad7656[Device_Number].Adc_Buffer[2*AdcNo + 1];
    }

}


/*----*/
void Ctrl_Arbus::WriteMcDac(uint8 DacNo, uint16 Value)
{
    Value = Value - 0x8000;
    //	Dac_Out(DacNo, Value);
}

/*----*/
void Ctrl_Arbus::UnSafeCmd(void)
{
    //	PORT_UNSAFE = 0;
    //	_nop_();
    //	_nop_();
    //	_nop_();
    //	_nop_();
    //	PORT_UNSAFE = 1;
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
