#ifndef _TMP_KEYFUN_H_
#define _TMP_KEYFUN_H_


class TMP_KEYFunc
{
public:

    static float TempTemp;
    static unsigned short Display_count;

    static bool KEYUP(void);
    static bool KEYDN(void);
private:
    static unsigned short KeyUp_count;
    static unsigned short KeyDn_count;
    static unsigned short KeyUp_SpeedUp;
    static unsigned short KeyDn_SpeedUp;
};

#endif
