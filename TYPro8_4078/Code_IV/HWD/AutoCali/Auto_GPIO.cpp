
#include "../../COMMON/includes.h"

unsigned char AUTO_GPIO::uc_PortInitValue_IBT[PORT_MAX][PIN_MAX];
unsigned char AUTO_GPIO::GPIO_Base[PORT_MAX][PIN_MAX];

void AUTO_GPIO::GPIO_Init(void)
{
	unsigned char uc_Port = 0, uc_Bit;

	for(uc_Port = 0; uc_Port < PORT_MAX; uc_Port ++)
	{
		for(uc_Bit = 0; uc_Bit < PIN_MAX; uc_Bit ++)
			GPIOFunc::InitPort(uc_Port, uc_Bit, GPIOFunc::GPIO_MOD[uc_Port][uc_Bit], uc_PortInitValue_IBT[uc_Port][uc_Bit],GPIO_Base[uc_Port][uc_Bit]);  //comment out chen
	}

}
