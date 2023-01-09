#include "includes.h"

unsigned short TMP_KEYFunc::KeyUp_count = 0;
unsigned short TMP_KEYFunc::KeyDn_count = 0;
float TMP_KEYFunc::TempTemp = 30.0f;
unsigned short TMP_KEYFunc::Display_count = 0;
unsigned short TMP_KEYFunc::KeyUp_SpeedUp = 0;
unsigned short TMP_KEYFunc::KeyDn_SpeedUp = 0;

/******************************************************************************
                                   KEY
******************************************************************************/
bool TMP_KEYFunc::KEYUP(void)
{
    if(((LPC_GPIO1->PIN & 0x0004)>>2) == 0)
    {
        TMP_KEYFunc::KeyUp_count++;
//		if(TMP_KEYFunc::KeyUp_count == 11)//10 cycle, about 8ms
//			TMP_KEYFunc::TempTemp = TMP_KEYFunc::TempTemp + 0.1f;
//		else if(TMP_KEYFunc::KeyUp_count == 211)//200 cycle, about 160ms
        if(TMP_KEYFunc::KeyUp_count == 200)//200 cycle, about 160ms
        {
            TMP_KEYFunc::KeyUp_SpeedUp = 1;
            TMP_KEYFunc::TempTemp = TMP_KEYFunc::TempTemp + 1.0f;
            TMP_KEYFunc::KeyUp_count = 0;
        }
    }
    else if((TMP_KEYFunc::KeyUp_count >= 11) && (TMP_KEYFunc::KeyUp_SpeedUp == 0))//10 cycle, about 8ms
    {
        TMP_KEYFunc::TempTemp = TMP_KEYFunc::TempTemp + 0.1f;
        TMP_KEYFunc::KeyUp_count = 0;
    }
    else if(TMP_KEYFunc::KeyUp_SpeedUp == 1)
    {
        TMP_KEYFunc::KeyUp_count = 0;
        TMP_KEYFunc::KeyUp_SpeedUp = 0;
    }
    else
        return false;
    if(TMP_KEYFunc::TempTemp >= 60.0f)
        TMP_KEYFunc::TempTemp = 60.0f;
    return true;
}

bool TMP_KEYFunc::KEYDN(void)
{
    if(((LPC_GPIO1->PIN & 0x0008)>>2) == 0)
    {
        TMP_KEYFunc::KeyDn_count++;
//		if(TMP_KEYFunc::KeyDn_count == 11)
//			TMP_KEYFunc::TempTemp = TMP_KEYFunc::TempTemp - 0.1f;
//		else if(TMP_KEYFunc::KeyDn_count == 211)
        if(TMP_KEYFunc::KeyDn_count == 200)
        {
            TMP_KEYFunc::KeyDn_SpeedUp = 1;
            TMP_KEYFunc::TempTemp = TMP_KEYFunc::TempTemp - 1.0f;
            TMP_KEYFunc::KeyDn_count = 0;
        }
    }
    else if((TMP_KEYFunc::KeyDn_SpeedUp == 0)&&(TMP_KEYFunc::KeyDn_count >= 11))
    {
        TMP_KEYFunc::TempTemp = TMP_KEYFunc::TempTemp - 0.1f;
        TMP_KEYFunc::KeyDn_count = 0;
    }
    else if(TMP_KEYFunc::KeyDn_SpeedUp == 1)
    {
        TMP_KEYFunc::KeyDn_count = 0;
        TMP_KEYFunc::KeyDn_SpeedUp = 0;
    }
    else
        return false;
    if(TMP_KEYFunc::TempTemp <= 10.0f)
        TMP_KEYFunc::TempTemp = 10.0f;
    return true;
}


