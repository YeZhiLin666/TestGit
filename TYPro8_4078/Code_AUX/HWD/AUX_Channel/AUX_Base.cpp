///////////////////////////////////////////////////////////////////////////////////
//File Name: AUX_BASE.cpp
//Version:		V1.0
//Date:				2013/11/18
//Description:
//						����������ز���
//Others:      dirui
//History��

///////////////////////////////////////////////////////////////////////////////////
#include "../../Entry/includes.h"


//ssp0   P0.15 P0.17 P0.18 ssp0    P0.15 P0.17 P0.18 
const St_Port AUX_Base::Pin_SCK0      = {0,        15,        eCPTYPE_MCUGPIO,   SSP0_SCK,              0};
const St_Port AUX_Base::Pin_MISO0     = {0,        17,        eCPTYPE_MCUGPIO,   SSP0_MISO,             0};
const St_Port AUX_Base::Pin_MOSI0     = {0,        18,        eCPTYPE_MCUGPIO,   SSP0_MOSI,             0};


//----// K���¶Ȱ�-----------------------------------------------------------------------------------
//����ʱ�¶�ת��Ϊ΢����ѹ���峣��ϵ��
//�¶ȷ�Χ��0(��C)��1372(��C)
//��΢����ѹת��Ϊ��ʱ�¶ȶ��峣��ϵ��
//�¶ȷ�Χ��-200(��C)��0(��C) ��΢����ѹ��Χ��-5891(��V)��0(��V)
const   float    AUX_Base::f_TCoefficientA_Ktype[9]={   0.0f        ,  2.5173462f , -1.1662878f,  -1.0833638f, -8.977354f, 
	-3.7342377f,  -8.6632643f, -1.0450598f,  -5.1920577f };

const   float   AUX_Base::f_tcA_Ktype[9]=            {   1.0e0,         1.0e-2,       1.0e-4,  1.0e-3  ,  1.0e-4,
	1.0e-3,        1.0e-4,       1.0e-3,  1.0e-5     };   
//��΢����ѹת��Ϊ��ʱ�¶ȶ��峣��ϵ��
//�¶ȷ�Χ��0(��C)��500(��C) ��΢����ѹ��Χ��0(��V)��20644(��V)
const   float    AUX_Base::f_TCoefficientB_Ktype[10]={    0.0f        ,     2.508355f,    7.860106f,   -2.503131f,   8.31527f,
	-1.228034f ,     9.804036f,   -4.413030f,    1.057734f,   -1.052755f }; 

const   float   AUX_Base::f_tcB_Ktype[10]=            {     1.0e0,         1.0e-2,        1.0e-6,          1.0e-2,  1.0e-4,
	1.0e-3,        1.0e-5,        1.0e-4,          1.0e-4,  1.0e-5 }; 

const   float    AUX_Base::f_TCoefficientC_Ktype[7]={  -1.318058f  , 4.830222f,  -1.646031f, 5.464731f, -9.650715f,8.802193f   ,  -3.110810f  };
//��΢����ѹת��Ϊ��ʱ�¶ȶ��峣��ϵ��
//�¶ȷ�Χ��500(��C)��1372(��C) ��΢����ѹ��Χ��20644(��V)��54886(��V)
const   float   AUX_Base::f_tcC_Ktype[7]=           {  1.0e2,         1.0e-2,            1.0e-4,           1.0e-5,  1.0e-5,
	1.0e-5,        1.0e-5     }; 
//---------// K���¶Ȱ�-----------------------------------------------------------------------------------------------------

//--------// T���¶Ȱ�-----------------------------------------------------------------------------------------------------------
//����ʱ�¶�ת��Ϊ΢����ѹ���峣��ϵ��
//�¶ȷ�Χ��0(��C)��400(��C)        T type thermocouple
//T_type�ȵ�ż��΢����ѹת��Ϊ��ʱ�¶ȶ��峣��ϵ��
//�¶ȷ�Χ��-200(��C)��0(��C) ��΢����ѹ��Χ��-5603(��V)��0(��V)
const   float    AUX_Base::f_TCoefficientA_Ttype[8]={   0.0           ,  2.5949192, -2.1316967,  7.9018692, 4.2527777, 
	1.3304473    ,  2.0241446, 1.2668171  };

const   float   AUX_Base::f_tcA_Ttype[8]=           {      1.0e0,         1.0e-2,       1.0e-5,  1.0e-3  ,  1.0e-3,
	1.0e-3,        1.0e-4,       1.0e-4    };    //��ǰһ��ָ���Ĳ�൱�Ṳ��ָ��
//T_type�ȵ�ż��΢����ѹת��Ϊ��ʱ�¶ȶ��峣��ϵ��
//�¶ȷ�Χ��0(��C)��400(��C) ��΢����ѹ��Χ��0(��V)��20872(��V)
const   float    AUX_Base::f_TCoefficientB_Ttype[7]={   0.0      ,   2.5928    , -7.602961,  4.637791, -2.165394, 
	6.048144,  -7.293422  };

const   float   AUX_Base::f_tcB_Ttype[7]=            {    1.0e0  ,     1.0e-2,       1.0e-5,  1.0e-4  ,  1.0e-4,
	1.0e-5,      1.0e-5    };   //��ǰһ��ָ���Ĳ�൱�Ṳ��ָ��
//-------// T���¶Ȱ�-----------------------------------------------------------------------------------------------------------



float AUX_Base::t90_SampleEx_KType[8];
float AUX_Base::t90_SampleEx_TType[8];




//start init
//hardware init
/******************************************************************************
** Function name:		Hardware_Init
**
** Descriptions:	        INitialize typeA IO
**			     
** parameters:		  None
** Returned value:		None
** 
******************************************************************************/
void AUX_Base::Hardware_Init(void)
{  
	//�ڲ�IO��ʼ��
	//ssp0   P0.15 P0.17 P0.18 ssp0      P0.15 P0.17 P0.18 
	// 	ControlPin::SetIObyCP(Pin_MISO0);
	// 	ControlPin::SetIObyCP(Pin_MOSI0);
	// 	ControlPin::SetIObyCP(Pin_SCK0);

	//ssp0      P1.20 P1.23 P1.24 ssp0    P1.20 P1.23 P1.24
	ControlPin::SetIObyCP(Pin_MISO0);
	ControlPin::SetIObyCP(Pin_MOSI0);
	ControlPin::SetIObyCP(Pin_SCK0);

}

float AUX_Base::T_t90_Convert_KType(float f_Value,unsigned char uc_CheckFlag)
{
	//��΢����ѹת��Ϊ��ʱ�¶�
	//�¶ȷ�Χ��-200(��C)��0(��C) ��΢����ѹ��Χ��-5891(��V)��0(��V)
	//�¶ȷ�Χ��0(��C)��500(��C) ��΢����ѹ��Χ��0(��V)��20644(��V)
	//�¶ȷ�Χ��500(��C)��1372(��C) ��΢����ѹ��Χ��20644(��V)��54886(��V)
	unsigned  char  uc_i = 0;
	float  f_TempA = 0;
	float  f_TempE = 0;

	if(( f_Value < -6458)||( f_Value >54886))        //��������  ΢����ѹΪ��С��λ����
	{
		return ( 22.0 );     // �����ȵ���ʱĬ��Ϊ22��ʱ��   [K���¶ȷ�Χ:-270(��C)��1372(��C)]
	}

	if(uc_CheckFlag == CHECK_T_VOLTAGERANG)
	{
		return ( 1.0 );   // ������ѹֵ�Ƿ����ڹ涨��Χ��
	}

	//�����
	// t90 = c0 + c1*E + c2*E^2+ c3*E^3+ c4*E^4+ c5*E^5+ c6*E^6+ c7*E^7+ c8*E^8+ c9*E^9
	if(f_Value<0)       //�¶ȷ�Χ��-200(��C)��0(��C) ��΢����ѹ��Χ��-5891(��V)��0(��V)
	{
		f_TempA  = 1;                      //  ����Z
		f_TempE =f_TempA * f_TCoefficientA_Ktype[0];   
		for(uc_i=1;uc_i<9;uc_i++)
		{
			f_TempA  = f_TempA *f_Value*f_tcA_Ktype[uc_i];                //  ����Z                A=A*a*fmin[i]
			f_TempE =  f_TempE+ f_TCoefficientA_Ktype[uc_i]*f_TempA ;      //��ϵ��            E=E+d[i]*A
		}	
	}
	else if(f_Value<=20644)        //�¶ȷ�Χ��0(��C)��500(��C) ��΢����ѹ��Χ��0(��V)��20644(��V)
	{       
		f_TempA  = 1;                      //  ����Z
		f_TempE =f_TempA * f_TCoefficientB_Ktype[0];   
		for(uc_i=1;uc_i<10;uc_i++)
		{
			f_TempA  = f_TempA *f_Value*f_tcB_Ktype[uc_i];                //  ����Z                A=A*a*fmin[i]
			f_TempE =  f_TempE+ f_TCoefficientB_Ktype[uc_i]*f_TempA ;      //��ϵ��
		}	
	}
	else if(f_Value<=54886)       //�¶ȷ�Χ��500(��C)��1372(��C) ��΢����ѹ��Χ��20644(��V)��54886(��V)
	{ 
		f_TempA  = f_Value*f_tcC_Ktype[1];                      //  ����Z	 
		f_TempE = f_tcC_Ktype[0] * f_TCoefficientC_Ktype[0] + f_tcC_Ktype[1]* f_Value*f_TCoefficientC_Ktype[1];   
		for(uc_i=2;uc_i<7;uc_i++)
		{
			f_TempA  = f_TempA *f_Value*f_tcC_Ktype[uc_i];               //  ����Z                A=A*a*fmin[i]
			f_TempE =  f_TempE+f_TCoefficientC_Ktype[uc_i]*f_TempA ;      //��ϵ��
		}	
	}

	return ( f_TempE );
}

float  AUX_Base::T_t90_Convert_TType(float f_Value,unsigned char uc_CheckFlag)
{ 
	//��΢����ѹת��Ϊ��ʱ�¶�
	//�¶ȷ�Χ��-200(��C)��0(��C) ��΢����ѹ��Χ��-5603(��V)��0(��V)
	//�¶ȷ�Χ��0(��C)��400(��C) ��΢����ѹ��Χ��0(��V)��20872(��V)
	unsigned  char  uc_i = 0;
	float  f_TempA = 0;
	float  f_TempE = 0;

	if(( f_Value < -6258)||( f_Value >20872))        //��������  ΢����ѹΪ��С��λ����
	{
		return ( 22.0 );    // �����ȵ���ʱĬ��Ϊ22��ʱ��   [T���¶ȷ�Χ:-270(��C)��400(��C)]
	}

	if(uc_CheckFlag == CHECK_T_VOLTAGERANG)
	{
		return ( 1.0 );   // ������ѹֵ�Ƿ����ڹ涨��Χ��
	}

	//�����
	// t90 = c0 + c1*E + c2*E^2+ c3*E^3+ c4*E^4+ c5*E^5+ c6*E^6+ c7*E^7
	if(f_Value<0)       //�¶ȷ�Χ��-200(��C)��0(��C) ��΢����ѹ��Χ��-5603(��V)��0(��V)
	{
		f_TempA  = 1;                      //  ����Z
		f_TempE =f_TempA * f_TCoefficientA_Ttype[0];   
		for(uc_i=1;uc_i<8;uc_i++)
		{
			f_TempA  = f_TempA *f_Value*f_tcA_Ttype[uc_i];               //  ����Z                A=A*a*fmin[i]
			f_TempE =  f_TempE+ f_TCoefficientA_Ttype[uc_i]*f_TempA ;     //��ϵ��            E=E+d[i]*A
		}	
	}
	else if(f_Value<=20872)        //�¶ȷ�Χ��0(��C)��400(��C) ��΢����ѹ��Χ��0(��V)��20872(��V)
	{       
		f_TempA  = 1;                      //  ����Z
		f_TempE =f_TempA * f_TCoefficientB_Ttype[0];   
		for(uc_i=1;uc_i<7;uc_i++)
		{
			f_TempA  = f_TempA *f_Value*f_tcB_Ttype[uc_i];               //  ����Z                A=A*a*fmin[i]
			f_TempE =  f_TempE+ f_TCoefficientB_Ttype[uc_i]*f_TempA ;     //��ϵ��
		}	
	}

	return ( f_TempE );
}      

//�¶�to��ѹ
void AUX_Base::T_t90_KType_SampleEx(unsigned char Bid,float f_Value)
{
	t90_SampleEx_KType[Bid] = (float)Ea0_Ktype*exp((float)Ea1_Ktype*( f_Value-126.9686f)*( f_Value-126.9686f)) +((float)Ec0_Ktype+(float)Ec1_Ktype*f_Value+Ec2_Ktype* f_Value*f_Value+(float)Ec3_Ktype* f_Value*f_Value* f_Value);
	// E = Alpha0 * exp ( Alpha1 * ( t90 - 126.9686 ) ^ 2) + c0 + c1 * t90 + c2 * t90^2+ c3 * t90^3; 
}

void AUX_Base::T_t90_TType_SampleEx(unsigned char Bid,float f_Value)
{
	t90_SampleEx_TType[Bid] = (float)Ec0_Ttype+(float)Ec1_Ttype*f_Value+(float)Ec2_Ttype* f_Value*f_Value+(float)Ec3_Ttype* f_Value*f_Value* f_Value;    //�����±�ɵ�ѹE����λ΢��
	// E = c0 + c1 * t90 + c2 * t90^2+ c3 * t90^3; 
}

float AUX_Base::get_SampleEx(unsigned char Bid,unsigned char type)
{
	//	if (type == AUX_TCT_K)
	if (type == TC_K)

	{
		return t90_SampleEx_KType[Bid];
	}
	//	if (type == AUX_TCT_T)
	if (type == TC_T)

	{
		return t90_SampleEx_TType[Bid];
	}

	//ekse
	return 0;
}

unsigned char AUX_Base::getBID(void)
{
	//BS0-BS7    P3.24-P3.31
	unsigned char BoardID = 0;

	St_Port MyPort;
	MyPort.PortType = eCPTYPE_MCUGPIO;
	MyPort.Function = GPIO_I;
	MyPort.Port = 3;
	MyPort.Bits = 24;

	for(unsigned char i = 0;i<8;i++)
	{
		ControlPin::SetIObyCP(MyPort);
		BoardID |= ControlPin::ReadMe(MyPort)<<i;

		MyPort.Bits++;
	}

	return BoardID;
}


