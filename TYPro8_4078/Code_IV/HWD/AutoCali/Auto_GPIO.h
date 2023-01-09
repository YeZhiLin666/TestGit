
#ifndef _AUTO_CALI_GPIO_H_
#define _AUTO_CALI_GPIO_H_

class AUTO_GPIO
{
private:

public:
	static  unsigned char uc_PortInitValue_IBT[PORT_MAX][PIN_MAX];
	static  unsigned char GPIO_Base[PORT_MAX][PIN_MAX];
	 
	static  void GPIO_Init(void);
};	

#endif
