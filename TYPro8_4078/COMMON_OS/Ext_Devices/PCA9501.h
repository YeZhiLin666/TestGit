#pragma once

#define   PCA9501_IO_WRITE(adr)      (adr<<1)  // address ???
#define   PCA9501_IO_READ       0x01
#define	_PCA9501_LM75 			   1
#define   PCA9501_PORT         I2C2
class  PCA9501
{
public:
    static bool SetExpander(unsigned char adr,unsigned char bitIO);
//    static bool PCA9501_ReadTemp(unsigned char pca9501_Adr,unsigned char sub_adr,float &f_temp);
//    static bool readExpander(unsigned char adr,unsigned char &bitIO);
//    static void SetE2PromMode(unsigned char adr,unsigned char *buf);

private:
};

