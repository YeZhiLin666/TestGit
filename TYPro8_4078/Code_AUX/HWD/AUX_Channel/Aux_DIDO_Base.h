#pragma once

#if(AUX_MULTI_FUNCTION_BOARD_DIDO == 1 )
#define MAX_DO_COUNT   4
#else
#define MAX_DO_COUNT   32 
#endif
//#define MAX_DI_COUNT   32


typedef struct
{
	unsigned char   m_MapDIIdx;
	unsigned char   m_MapDOIdx;
	const St_Port * m_pSt_DIPort;
	const St_Port * m_pSt_DOPort;
}St_DIDOMapMgr;

class Aux_DIDO_Base
{
public: //attribute
	static St_DIDOMapMgr  m_DIDO_MapMgr[MAX_DO_COUNT];
	static UInt16 			 Priority;
	static UInt16   		 TaskID ;
public:   //fuction
	static void Hardware_Init(void);
	static void Software_Init(void);
	static void DoWork(void);

private:
	static const St_Port Pin_DI_1;
	static const St_Port Pin_DI_2;
	static const St_Port Pin_DI_3;
	static const St_Port Pin_DI_4;

	static const St_Port Pin_DO_1;
	static const St_Port Pin_DO_2;
	static const St_Port Pin_DO_3;
	static const St_Port Pin_DO_4;

};
