/*
��ʱֻ�Ǽ򵥵�1s�ϴ�һ��
updata 2012 11 15
*/

#ifndef _LOGLIMIT_H_
#define _LOGLIMIT_H_



class logLimit
{
public:
    static UInt16 Priority;
    static UInt16 TaskID;			//  should be a definition which positioning this task in OS_Task[]

    static void Init(void);
    static void DoWork(void);


private:
    static MsTime log_period;
    static unsigned char log_chn;
    static void sendMSGToEth(unsigned char chn);    //֪ͨethernet��������
};


#endif
