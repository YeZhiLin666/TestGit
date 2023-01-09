Entry: 
    Firmware的入口控制。一些与整个Firmware相关的定义、配置、说明。

Task_Dispatcher:
	Mini-OS的调度部分，与硬件无关。
	
Protocols:
	各种通信协议的结构、枚举定义。包含了IV-PC,IV-IV,IV-AUX,AUX-PC。
	
Data Manipulation:
	纯粹的数据处理部分，不依赖于任何硬件设备。就好像通用数学库一样。

MCU_Core:
	MCU及其片上设备的各种驱动。将来若有多种MCU的话，下面还有子目录，以便分别管理。
	
Ext_Devices:
	所有不在MCU上的设备驱动。
	
TCP-NET：
    KEIL所提供的TCP NET库文件相关。
	
EthernetParse:
    通信协议解析部分代码，以及事件确认缓冲区等。
	
CAN_Bus:
	CAN通信相关驱动。
	
IV_Tasks:
	IV通道下位机所有相关任务集合。
	
Aux_Tasks:
	辅助通道下位机所有相关任务集合。
	
IBT、TESLA、AUTOCALI、BT2208:
	对应板类型的驱动。
	
	
