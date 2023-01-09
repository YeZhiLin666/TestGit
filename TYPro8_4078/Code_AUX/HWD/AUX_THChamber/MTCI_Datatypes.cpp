/*****************************************************************************
// �����ļ�      :DATATYPES.C  �м䴦�����������������͵ȳ���
// �ļ�����      :ΪLPC2378 AUX-MPU board����
// ��д����      :2008.02.29
*****************************************************************************/

#include "../../Entry/includes.h"


//MODBUS CRCHi[] AND CRCLo[]
const unsigned char auchCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,  // 8*10
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,  // 8*10
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,  // 8*10
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};  // ��λ256����

const unsigned char auchCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2,
    0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04,
    0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
    0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8,
    0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6,
    0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,
    0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
    0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,  // 8*10
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE,
    0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA,
    0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
    0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
    0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62,
    0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE,
    0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,  // 8*10
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
    0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C,
    0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76,
    0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,
    0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
    0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
    0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A,
    0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,  // 8*10
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86,
    0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};  // ��λ256����





//---------------------------------------------------------------------

void MTCI_datatypes::ctrl_Value2String_Hex(short int * pValue, unsigned char* string)
{
    // ����ֵת�����ַ���������ʱ�á�
    unsigned char  uc_i = 0;
    unsigned char  uc_j = 4;
    unsigned int  Count = 0;
    unsigned short int   uw16_MoveValue = * pValue;

    for( uc_i = 0; uc_i < 4; uc_i ++)
    {
        uw16_MoveValue = * pValue;
        uc_j--;
        uw16_MoveValue >>= (uc_j*4);
        uw16_MoveValue &= 0X000F;

        if(uw16_MoveValue < 10)
        {
            *string = uw16_MoveValue + '0';
            string++;
            Count++;
        }
        else
        {
            uw16_MoveValue -= 0X0A;
            *string = uw16_MoveValue + 'A';
            string++;
            Count++;
        }
    }
}

//CONVERT THE VALUE TO STRING //
unsigned int MTCI_datatypes::ctrl_Value2String_Dot1(float Value,unsigned char* string)
{
	//  ����ֵת�����ַ���
	short int pValue;
	unsigned int  Count = 0; 
	pValue = (Value *1000);
	if(  pValue < 0 )
	{
		*string = '-';
		string++;
		Count++;
		 pValue = -  pValue;  // ����������
	} 
	if( pValue > 30000) 
		 pValue = 30000;	 // ���ֵ���� [99]��
// 	if( pValue >= 10000)
// 	{
		*string =( pValue/10000)+'0';     //  99/10=9.9=9   (3)
		string++;
		Count++;
		 pValue =  pValue%10000;
// 	}
// 	if( pValue >= 1000)
// 	{
		*string =  pValue/1000+'0';    //  99%10=9   (4)
		string++;
		Count++;
		 pValue =  pValue%1000;
// 	}
	*string = '.';               // ��С���㣬���ֵ���� [9999]��ʵ��Ӧ��[999.9]��
	string++;
	Count++;
// 	if( pValue >=100)
// 	{
		*string =  pValue/100+'0';    //  99%10=9   (4)
		string++;
		Count++;
		 pValue =  pValue%100;
//     }
	*string =( pValue/10)+'0';     //  99/10=9.9=9   (3)
	string++;
	Count++;

	*string =  pValue%10+'0';    //  99%10=9   (4)
	string++;
	Count++;
	return Count;
}

//CONVERT THE VALUE TO STRING //
unsigned int MTCI_datatypes::ctrl_Value2String_Dot(short int* pValue,unsigned char* string)
{
    //  ����ֵת�����ַ���
    //  ����C�������棺 ������: gcvt  ��  ��: �Ѹ�����ת�����ַ���  ��  ��: char *gcvt(double value, int ndigit, char *buf);
    unsigned int  Count = 0;

    if( * pValue < 0 )
    {
        *string = '-';
        string++;
        Count++;
        * pValue = - * pValue;  // ����������
    }

    if(* pValue > 9999)
        * pValue = 9999;	 // ���ֵ���� [9999]������λ���´�ʱ�ѷŴ�10����ʵ��Ӧ��[999.9]��

    if(* pValue >= 1000)
    {
        *string = (* pValue/1000)+'0';  //  9999/1000=9.999=9  (1)    '0'=0x30 // �� + 0x30 = ��ֵ
        string++;
        Count++;
    }

    if(* pValue >= 100)
    {
        * pValue = * pValue%1000;   //  9999%1000=999
        *string = (* pValue/100)+'0';  //  999/100=9.99=9   (2)
        string++;
        Count++;
        * pValue = * pValue%100;   //  9999%100=99
    }

    *string =(* pValue/10)+'0';     //  99/10=9.9=9   (3)
    string++;
    Count++;

    *string = '.';               // ��С���㣬���ֵ���� [9999]��ʵ��Ӧ��[999.9]��
    string++;
    Count++;

    *string = * pValue%10+'0';    //  99%10=9   (4)
    string++;
    Count++;

    return Count;
}


void MTCI_datatypes::ctrl_Value2String_BeforeZero(short int* pValue,unsigned char* string)
{
    //  ����ֵת�����ַ���
    //  ����C�������棺 ������: gcvt  ��  ��: �Ѹ�����ת�����ַ���  ��  ��: char *gcvt(double value, int ndigit, char *buf);
    unsigned short int uw16_Count = 0;
    unsigned short int uw16_i = 0;
    unsigned char uc_CharData_Temp;
    unsigned char uc_CharData[10];


    if( * pValue < 0 )
    {
        uc_CharData_Temp =  '-';
        //*string = uc_CharData_Temp;
        //string++;
        * pValue = -* pValue;  // ����������
        uc_CharData[uw16_Count++] = uc_CharData_Temp;
    }

    if(* pValue > 9999)
        * pValue = 9999;	 // ���ֵ���� [9999]������λ���´�ʱ�ѷŴ�10����ʵ��Ӧ��[-999.9]��[999.9]��

    if(* pValue >= 1000)
    {
        uc_CharData_Temp = (* pValue/1000)+'0';  //  9999/1000=9.999=9  (1)    '0'=0x30 // �� + 0x30 = ��ֵ
        //*string = uc_CharData_Temp;
        //string++;
        uc_CharData[uw16_Count++] = uc_CharData_Temp;
    }

    if(* pValue >= 100)
    {
        * pValue = * pValue%1000;   //  9999%1000=999
        uc_CharData_Temp = (* pValue/100)+'0';  //  999/100=9.99=9   (2)
        //*string = uc_CharData_Temp;
        //string++;
        * pValue = * pValue%100;   //  9999%100=99
        uc_CharData[uw16_Count++] = uc_CharData_Temp;
    }

    uc_CharData_Temp = (* pValue/10)+'0';     //  99/10=9.9=9   (3)
    //*string = uc_CharData_Temp;
    //string++;
    uc_CharData[uw16_Count++] = uc_CharData_Temp;


    uc_CharData_Temp = '.';               // ��С���㣬���ֵ���� [9999]��ʵ��Ӧ��[999.9]��
    //*string = uc_CharData_Temp;
    //string++;
    uc_CharData[uw16_Count++] = uc_CharData_Temp;

    uc_CharData_Temp = * pValue%10+'0';    //  99%10=9   (4)
    //*string = uc_CharData_Temp;
    //string++;
    uc_CharData[uw16_Count++] = uc_CharData_Temp;

    uc_CharData_Temp = 0;
    if(uw16_i < 6)
    {   //  ��ʾ���䷽ʽ������0080.0   //���� -055.0

        if((uc_CharData[0] == '-')&&((6-uw16_Count) >= 1))
        {   //  ��ʾ���䷽ʽ����������-005.4 //����λ��
            *string = '-';
            string++;
            uc_CharData_Temp = 1;
        }

        for(uw16_i = 0; uw16_i < (6 - uw16_Count); uw16_i++)
        {
            *string = '0';   // ��ǰ���������㲹��
            string++;
        }

        for(uw16_i = 0; uw16_i < (uw16_Count - uc_CharData_Temp); uw16_i++)
        {
            *string = uc_CharData[uw16_i + uc_CharData_Temp]; // ���Ϊ������ȥ�����Ž���ƽ�ƴ���
            string++;
        }
    }
    //return s;
}


// CONVERT DIGITAL NUMBERS OF CHAMBER TO VALUE AND SEND IT TO PC //
unsigned char*  MTCI_datatypes::ctrl_String2Value_Signed(unsigned char* string, short int* pValue)
{
    //  ���ַ���ת����ֵ
    //  ����C�������棺 ������: atof   ��  ��: ���ַ���ת���ɸ�����   ��  ��: double atof(const char *nptr);

    short int sign = 1;
    short int Value = 0;

    while( (*string) == 0x20 && (*string)!=0 )
        string++; // ignore preblank, original   // Խ���ո�: 0x20=Space; 0x00=NUL

    if( (*string) <= 0x20 )   // ��������Ϊ��Ч��
        return  NULL;

    if( (*string) == '-' )
    {
        sign = -1;      // ��ֵ
        string++;
    }
    else if( (*string) == '+' )
    {
        string++;	  // ��ֵ
    }

    if( (*string)<'0' || (*string)>'9' )   // ���ֲ���Ч��
        return NULL;

    while( (*string)>='0' && (*string)<='9' )
    {
        Value = Value*10 + (*string-'0');   //  '0'=0X30  // ��ֵ - 0X30 = ����
        string++;
    }

    Value *= 10;	     //  ���Ŵ�10�����Ա��ϴ���λ����
    if( (*string) == '.' )
    {
        string++;

        if( (*string)>='0' && (*string)<='9' )
        {
            Value += (*string-'0');   //  ��ȡС�����һλ��Ч����
            string++;
        }
    }

    *pValue = Value * sign;   // �洢�����ֵ
    return string;
}

void MTCI_datatypes::ctrl_String2Value_Hex(unsigned char* string, short int* pValue)
{
    //  ���ַ���ת����ֵ  (�����ʮ������4�ֽ�����)
    //  ����C�������棺 ������: atof   ��  ��: ���ַ���ת���ɸ�����   ��  ��: double atof(const char *nptr);

    unsigned char  uc_i = 0;
    unsigned char  uc_j = 4;
    unsigned int  XChkSum = 0;
    short int sign = 1;
    short int Value = 0;
    short int   uw16_MoveValue = 0;

    for( uc_i = 0; uc_i < 4; uc_i ++)  // �ܹ�4���ֽ�
    {
        uw16_MoveValue = *string;
        if(uw16_MoveValue >= 'A' )
        {
            if( uc_i == 0)
                sign = -1;        // ��ֵ
            uw16_MoveValue = (uw16_MoveValue - 0X41) + 0X0A;   // ASCII ��ĸ   //   'A'=0x41 // ����ֵ - 0x41 = ��ֵ
            string++;      //  �Ӹ��ֽڵ����ֽ�
            XChkSum ++;
        }
        else
        {
            uw16_MoveValue = (uw16_MoveValue - 0X30);      // ASCII ����  //   '0'=0x30 // ��ֵ - 0x30 = ��ֵ
            string++;    //  �Ӹ��ֽڵ����ֽ�
            XChkSum ++;
        }

        uw16_MoveValue &= 0X000F;
        uc_j--;
        uw16_MoveValue <<= (uc_j*4);
        Value += uw16_MoveValue;
    }


    *pValue = Value;    // �洢�����ֵ
}


unsigned short int MTCI_datatypes::CRC16(unsigned char *puchMsg, unsigned int usDataLen)
{
    //  ������ͨ�Ű�У��CRC�� ;ʪ�ȼ���ʱ��Щ�������ɲ���ֻҪ�¶ȵ���������Ϊ����
// http://www.ip33.com/crc.html
// CRC��ѭ������У�飩���߼���
// ����ģ�� NAME��CRC-16/MODBUS     x16+x15+x2+1
// ��� WIDTH��16
// ����ʽ POLY��Hex����8005
// ��ʼֵ INIT��Hex����FFFF
// ������ֵ XOROUT��Hex����0000
// �������ݷ�ת��REFIN��       ������ݷ�ת��REFOUT��
// �ù��ߣ�CRCУ�鹤�� 2.0.exe

    unsigned char uchCRCHi = 0xFF ;
    unsigned char uchCRCLo = 0xFF ;
    unsigned int  uIndex = 0;

    while (usDataLen--)
    {
        uIndex = uchCRCHi ^ *puchMsg++ ;
        uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
        uchCRCLo = auchCRCLo[uIndex] ;
    }
    return ((uchCRCHi << 8) | uchCRCLo) ;
}

/*
unsigned short int CRC16_ToView(unsigned char *puchMsg, unsigned int usDataLen)
{
//  ������ͨ�Ű�У��CRC�� ;ʪ�ȼ���ʱ��Щ�������ɲ���ֻҪ�¶ȵ���������Ϊ����

unsigned char uchCRCHi = 0xFF ;
unsigned char uchCRCLo = 0xFF ;
unsigned char uIndex ;
unsigned char uc_DH,uc_DL,uc_P,uc_Hi,uc_Lo;

while (usDataLen--)
{
uc_Hi = uchCRCHi;
uc_P = (*puchMsg);
uIndex = uc_Hi ^ uc_P;
puchMsg ++;

uc_Lo = uchCRCLo;
uc_DH = auchCRCHi[uIndex];
uchCRCHi = uc_Lo ^ uc_DH;

uc_DL = auchCRCLo[uIndex];
uchCRCLo =  uc_DL;

}
return ((uchCRCHi << 8) | uchCRCLo);
}
*/

unsigned short int MTCI_datatypes::CRC16_Counter(unsigned char *puchMsg, unsigned int usDataLen)
{
    //  ������ͨ�Ű�У��CRC�� ;ʪ�ȼ���ʱ��Щ�������ɲ���ֻҪ�¶ȵ���������Ϊ����
// ����ģ�� NAME��CRC-16/MODBUS 
// ��� WIDTH��16
// ����ʽ POLY��Hex����A001
// �ù��ߣ�MODBUS CRC ������.exe

    unsigned int  IX,IY,CRC;
    //         unsigned char Rcvbuf[2];
    CRC = 0xFFFF; //set all 1

    if(usDataLen <= 0)
        CRC = 0;
    else
    {
        usDataLen--;
        for (IX=0; IX <= usDataLen; IX++)
        {
            CRC = CRC ^ ((unsigned int)(*puchMsg));
            puchMsg ++;
            for(IY = 0; IY <= 7; IY++)
            {
                if ((CRC & 1) != 0 )
                    CRC =(CRC >> 1) ^ 0xA001;
                else
                    CRC = CRC >> 1;    //
            }
        }

    }
    //Rcvbuf[0] = (CRC & 0xff00)>>8;//��λ��
    //Rcvbuf[1] = (CRC & 0x00ff);  //��λ��

    //CRC= Rcvbuf[0]<<8;
    //CRC+= Rcvbuf[1];
    return CRC;
}

unsigned int MTCI_datatypes::CheckSum(unsigned char *puchMsg, unsigned int usDataLen)
{
    //  ����CheckSum��ֵ��
    unsigned int  uIndex = 0 ;

    while (usDataLen--)
    {
        uIndex =  uIndex + *puchMsg;
        puchMsg++;
    }
    return (uIndex);
}

unsigned char MTCI_datatypes::CheckXorSum(unsigned char *puchMsg, unsigned int usDataLen)
{
    //  ��������CheckXorSum��ֵ��
    unsigned char  uIndex = 0 ;

    while (usDataLen--)
    {
        uIndex =  uIndex ^ *puchMsg;
        puchMsg++;
    }
    return (uIndex);
}

unsigned char  MTCI_datatypes::CalcCheckSum_Byte(unsigned char *puchMsg, unsigned int usDataLen)
{
    //  �����У����
    UWord32 uw32_Sum = 0;
    unsigned char uc_Byte ;

    while (usDataLen--)
    {
        uw32_Sum += *puchMsg++ ;
    }

    uc_Byte = (unsigned char)uw32_Sum;
    uc_Byte = 0xFF - uc_Byte;

    return (uc_Byte) ;
}

short int MTCI_datatypes::Convert_FloatToInt(unsigned char* Source)
{
    //  ת����ֵ���Ӹ�����ת��������������ʱʹ�á�
    short int w16_Value = 0;
    float f_Value = 0;
    short int w16_i;
    unsigned char* uc_P;
    uc_P = (unsigned char*) &f_Value;// ����ֵ��8λ�ں󣬵�8λ��ǰ����ɸ�������

    for(w16_i = 0; w16_i < 4; w16_i ++) // // ����ֵ�洢ΪСͷ��ʽ��С��ַ�Ե�8λ���ߵ�ַ�Ը�8λ��
        *(uc_P + w16_i) = *Source++ ;

    /*f_Value = (* (Source + 3) << 24) | (* (Source + 2) << 16)
    | (* (Source + 1) << 8) | (* Source) ; // ����ֵ��8λ�ں󣬵�8λ��ǰ����ɸ�������*/
    f_Value *= 10;  //  �Ŵ�10�������ϴ����ݡ�
    w16_Value =  (short int) f_Value; // ת������
    return (w16_Value);
}

void MTCI_datatypes::Convert_IntToFloat(unsigned char* Source,short int w16_Value)
{
    //  ת����ֵ������������ת��������������ʱʹ�á�
    float f_Value;
    short int w16_i;
    unsigned char* uc_P;
    f_Value = w16_Value;
    f_Value /= 10;    //  ��С10���ָ�ԭֵ��
    uc_P = (unsigned char*) &f_Value;  // ����ֵ��8λ�ں󣬵�8λ��ǰ����ɸ�������

    /*	*Source= ((unsigned char)(f_Value & 0XFF));  // ����ֵ��8λ�ں󣬵�8λ��ǰ����ɸ�������
    *(Source + 1) = ((unsigned char)((f_Value & 0X0000FF00) >> 8));
    *(Source + 2) = ((unsigned char)((f_Value & 0X00FF0000) >> 16));
    *(Source + 3) = ((unsigned char)((f_Value & 0XFF000000) >> 24));
    for(w16_i = 0;w16_i < 4;w16_i ++)
    *Source++ = *(uc_P + w16_i);*/

    for(w16_i = 0; w16_i < 4; w16_i ++) // // ����ֵ�洢ΪСͷ��ʽ��С��ַ�Ե�8λ���ߵ�ַ�Ը�8λ��
        *Source++ = *(uc_P + w16_i);
}

bool MTCI_datatypes::ctrl_strncmp(unsigned char* Src, unsigned char* Dest, unsigned int Len)
{
    unsigned int i;
    for(i = 0; i < Len; i++)
        if( Src[i] != Dest[i] ) return false;
    return true;
}

unsigned char MTCI_datatypes::Convert_ASCIIToHEX(unsigned char uc_ASCII)
{
    // ��ʮ������8λ�任��4λ��  ��'E' 0X45-0X30�任��0X0E
    unsigned char  uc_Value;
    uc_Value = ((uc_ASCII - 0X30)%0X10) + ((uc_ASCII - 0X30)/0X10*0X09);
    uc_Value &= 0X0F;
    return  uc_Value;

}


