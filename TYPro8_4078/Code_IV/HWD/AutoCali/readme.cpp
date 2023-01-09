/*
下位机第三方处理仍然接受
Set-3rd
Clr-3rd
Self-3rd
HP-3rd
ADC-3rd
Cell-set
Cell-get
Stop
命令

//comment out chen 20130422
状态：NoActive
         IDLE
		 TRAN
		 OutPut

TRAN时：    简单时序
ON-OFF-ON

//由Auto_Base来接受第三方请求
//Auto_Sample是自校准板采样

//使用器件
//FRAM(FM25L256)  SCK1 MOSI1 MISO1
//DAC AD5754         SCK   MOSI  MISO
//ADC AD7190         SCK0 MOSI0 MISO0


temp.ADR1 = (~AUTO_CALI_CPLD_HP_MID_ADD1);
temp.ADR2 = (~AUTO_CALI_CPLD_HP_MID_ADD2);

*/

