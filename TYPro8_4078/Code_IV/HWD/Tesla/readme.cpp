/*
增加一个Power On  相继打开Vin_Relay
Vin应该是类似于提供功率电的relay，不应考虑入LogicProcess

3个时序    LogicProcess驱动时是以Timer0 100us为基本单位    10ms即100个100us
channel starting

channel Auto Cali start

channel stop



/////////////////////////////////////////////////////
Relay：与LogicProcess的enum对应上  然后与XD对应上
EN，PRT，IH，Cali，Vin
IH  bit1    ACTIVE_CUR_RELAY_HIGH     XD1
EN bit5    ACTIVE_CUR_CUT_PWM        XD2
Cali bit11 ACTIVE_VOL_RANGE             XD4
PRT bit14 ACTIVE_P_R                          XD3

Vin                                                     XD0             

Dac:
I_Set


VH VL
DAC_B DAC_D
*/
