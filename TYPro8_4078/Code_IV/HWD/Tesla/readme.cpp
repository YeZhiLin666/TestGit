/*
����һ��Power On  ��̴�Vin_Relay
VinӦ�����������ṩ���ʵ��relay����Ӧ������LogicProcess

3��ʱ��    LogicProcess����ʱ����Timer0 100usΪ������λ    10ms��100��100us
channel starting

channel Auto Cali start

channel stop



/////////////////////////////////////////////////////
Relay����LogicProcess��enum��Ӧ��  Ȼ����XD��Ӧ��
EN��PRT��IH��Cali��Vin
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
