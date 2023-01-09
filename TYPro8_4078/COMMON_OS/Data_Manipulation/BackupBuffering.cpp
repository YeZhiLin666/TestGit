

#include "../../Entry/includes.h"


void BackupBuffering::SendDataPack(uint8 uc_Net)
{
    int i = 1, buf_Inactive2, empty_Idx;
    UWord32    uw32_EVSize;
    MsTime  *  MyCounterTime;
    uint32  *  st_p_Back_EventNo;
    int     *  st_p_Back_EventLen;
    uint8   *  st_p_Back_EventContent;
    int     *  w16_Buf_Active;
    int     *  w16_InUse_Idx;


    if(uc_Net == CAN_NET)
    {
        w16_Buf_Active = & CANMsgBuffering::Buf_Active_CAN;
        w16_InUse_Idx = & CANMsgBuffering::InUse_Idx_CAN;
        uw32_EVSize = EVENTSBUFFERSIZE_CAN;
        MyCounterTime =  & CANMsgBuffering::MyCounter_CAN;
    }
    else //if(uc_Net == ETHERNET)
    {
        w16_Buf_Active = & EventsBuffering::Buf_Active;
        w16_InUse_Idx = & EventsBuffering::InUse_Idx;
        uw32_EVSize = EVENTSBUFFERSIZE;
        MyCounterTime =  & EventsBuffering::MyCounter;
    }

    BackupBuffering::GetTimeTicksUpdate(MyCounterTime);	 // reset counter
    // 活跃区专用于存储发送的事件，非活跃区专用于重发。
    buf_Inactive2 = !(*w16_Buf_Active);							// init variables for following operation
    //Buf_Inactive = buf_Inactive2;		// 活跃区切换到非活跃区
    empty_Idx = *w16_InUse_Idx; // 活跃区指针空位置

    for (i = 0; i < uw32_EVSize; i ++)
    {
        if(uc_Net == CAN_NET)
        {
            st_p_Back_EventNo = & CANMsgBuffering::EV_Buf_CAN[buf_Inactive2][i].EventNo;
            st_p_Back_EventLen = & CANMsgBuffering::EV_Buf_CAN[buf_Inactive2][i].EventLen;
            st_p_Back_EventContent = & CANMsgBuffering::EV_Buf_CAN[buf_Inactive2][i].EventContent[0];
        }
        else //if(uc_Net == ETHERNET)
        {
            st_p_Back_EventNo = & EventsBuffering::EV_Buf[buf_Inactive2][i].EventNo;
            st_p_Back_EventLen = & EventsBuffering::EV_Buf[buf_Inactive2][i].EventLen;
            st_p_Back_EventContent = & EventsBuffering::EV_Buf[buf_Inactive2][i].EventContent[0];
        }

        if ( *st_p_Back_EventNo != 0)		// find the next event in Inactive buffer that is not confirmed
        {
            // !!!!!!!THIS IS PSEUDO CODE
            if(uc_Net == CAN_NET)
            {
                //CANBusParse::PushToSendingEventBuf(&CANMsgBuffering::EV_Buf_CAN[buf_Inactive2][i].EventContent[0], CANMsgBuffering::EV_Buf_CAN[buf_Inactive2][i].EventLen); // Add by DKQ on 08.05.2013, Try to put this in TCP buffer to resend it anyway

                memcpy( (void*)&CANMsgBuffering::ReSend_CAN_Msg.Status,(void*)st_p_Back_EventContent,sizeof(MP_CAN_MSG));
                CANMsgBuffering::ReSend_CAN_Msg.Content.Data[7] = 0xcc;          //测试是否重发出去
                CANFunc::CAN_Push_to_Send(&CANMsgBuffering::ReSend_CAN_Msg);
            }
            else //if(uc_Net == ETHERNET)
            {
                //暂时屏蔽 测试can
                //	EthernetParse::PushToSendingEventBuf(st_p_Back_EventContent, *st_p_Back_EventLen); // Add by DKQ on 08.05.2013, Try to put this in TCP buffer to resend it anyway
            }

            // <<<END>>> !!!!!!!THIS IS PSEUDO CODE
            empty_Idx = FindNextEmpty(empty_Idx,uc_Net);				// find the next empty slot in active buffer

            if (empty_Idx != -1)								// if there's no empty space, dump this event.
            {
                MoveEvent2Active(i, empty_Idx,uc_Net);					// move the event from inactive buffer to Active buffer empty slot.
                //EV_Buf[buf_Inactive2][i].EventNo = 0;		    // the event in inactive buffer is erased.
            }
            else
            {
                *st_p_Back_EventNo = 0;		    // the event in inactive buffer is erased.
            }
        }
    }

    *w16_Buf_Active = !(*w16_Buf_Active);									// Swap the buffers   	// 活跃区切换到非活跃区
    *w16_InUse_Idx = 0;												// reset index pointer
}

int BackupBuffering::ConfirmEvent(uint32 eventNo, uint8 uc_Net)
{
    int i;
    UWord32 uw32_EVSize;
    uint32  *  st_p_Front_EventNo;
    uint32  *  st_p_Back_EventNo;
    if(uc_Net == CAN_NET)
    {
        uw32_EVSize = EVENTSBUFFERSIZE_CAN;
    }
    else //if(uc_Net == ETHERNET)
    {
        uw32_EVSize = EVENTSBUFFERSIZE;
    }

    for (i = 0; i < uw32_EVSize; i ++)
    {   // 活跃区专用于存储发送的事件，非活跃区专用于重发。 同时找确认的事件。

        if(uc_Net == CAN_NET)
        {
            st_p_Front_EventNo = & CANMsgBuffering::EV_Buf_CAN[0][i].EventNo;
            st_p_Back_EventNo  = & CANMsgBuffering::EV_Buf_CAN[1][i].EventNo;
        }
        else //if(uc_Net == ETHERNET)
        {
            st_p_Front_EventNo = & EventsBuffering::EV_Buf[0][i].EventNo;
            st_p_Back_EventNo  = & EventsBuffering::EV_Buf[1][i].EventNo;
        }

        if( *st_p_Front_EventNo == eventNo)
        {
            *st_p_Front_EventNo = 0;
            return 1;
        }

        if( *st_p_Back_EventNo == eventNo)
        {
            *st_p_Back_EventNo = 0;
            return 1;
        }
    }

    return -1;
}

int BackupBuffering::PushingEvents(uint8 * content, int eventLen, uint32 eventNo, uint8 uc_Net)
{
    int i;
    UWord32    uw32_EVSize;
    int        w16_Length;
    uint32  *  st_p_Front_EventNo;
    int     *  st_p_Front_EventLen;
    uint8   *  st_p_Front_EventContent;
    int      w16_Buf_Active;
    int      *w16_InUse_Idx;


    if(uc_Net == CAN_NET)
    {
        w16_Buf_Active = CANMsgBuffering::Buf_Active_CAN;
        w16_InUse_Idx =  &CANMsgBuffering::InUse_Idx_CAN;
        st_p_Front_EventNo = & CANMsgBuffering::EV_Buf_CAN[  w16_Buf_Active][  *w16_InUse_Idx].EventNo;
        st_p_Front_EventLen = & CANMsgBuffering::EV_Buf_CAN[  w16_Buf_Active][  *w16_InUse_Idx].EventLen;
        st_p_Front_EventContent  = & CANMsgBuffering::EV_Buf_CAN[  w16_Buf_Active][  *w16_InUse_Idx].EventContent[0];
        uw32_EVSize = EVENTSBUFFERSIZE_CAN;
        w16_Length  = EVENTLENGTH_CAN;
    }
    else //if(uc_Net == ETHERNET)
    {
        w16_Buf_Active =  EventsBuffering::Buf_Active;
        w16_InUse_Idx = &EventsBuffering::InUse_Idx;
        st_p_Front_EventNo = & EventsBuffering::EV_Buf[ w16_Buf_Active][ *w16_InUse_Idx].EventNo;
        st_p_Front_EventLen = & EventsBuffering::EV_Buf[ w16_Buf_Active][ *w16_InUse_Idx].EventLen;
        st_p_Front_EventContent  = & EventsBuffering::EV_Buf[  w16_Buf_Active][ *w16_InUse_Idx].EventContent[0];
        uw32_EVSize = EVENTSBUFFERSIZE;
        w16_Length  = EVENTLENGTH;
    }

    if (  *w16_InUse_Idx >= uw32_EVSize)
        return -1;
    if(eventLen > w16_Length)
        return -1;

    *st_p_Front_EventNo = eventNo;    // 活跃区专用于存储发送的事件，存入事件
    *st_p_Front_EventLen = eventLen;
    for (i = 0; i < eventLen; i ++)
    {
        st_p_Front_EventContent[i] = content[i];
    }

    (*w16_InUse_Idx) ++;  // 活跃区专用于存储发送的事件，移动指针到空位置。
    return 0;

}
void BackupBuffering::MoveEvent2Active(int source, int destination, uint8 uc_Net)
{
    // 活跃区专用于存储发送的事件，转移到非活跃区专用于重发。
    UWord32    uw32_EVSize;
    uint32  *  st_p_Front_EventNo;
    int       w16_Buf_Active;

    void    *  st_p_Front;
    void    *  st_p_Back;

    if(uc_Net == CAN_NET)
    {
        w16_Buf_Active =  CANMsgBuffering::Buf_Active_CAN;
        st_p_Front = & CANMsgBuffering::EV_Buf_CAN[!( w16_Buf_Active)][source];
        st_p_Front_EventNo = & CANMsgBuffering::EV_Buf_CAN[!(w16_Buf_Active)][source].EventNo;
        st_p_Back = & CANMsgBuffering::EV_Buf_CAN[w16_Buf_Active][destination];
        uw32_EVSize = sizeof(EventsBuffer_CAN);
    }
    else //if(uc_Net == ETHERNET)
    {
        w16_Buf_Active =  EventsBuffering::Buf_Active;
        st_p_Front = & EventsBuffering::EV_Buf[!( w16_Buf_Active)][source];
        st_p_Front_EventNo = & EventsBuffering::EV_Buf[!(w16_Buf_Active)][source].EventNo;
        st_p_Back = & EventsBuffering::EV_Buf[w16_Buf_Active][destination];
        uw32_EVSize = sizeof(EventsBuffer);
    }

    memcpy( st_p_Back,st_p_Front,uw32_EVSize);
    *st_p_Front_EventNo = 0;

}

int BackupBuffering::FindNextEmpty(int startPoint, uint8 uc_Net)
{
    int i;

    UWord32    uw32_EVSize;
    uint32  *  st_p_Front_EventNo;
    int       w16_Buf_Active;
    int     *  w16_InUse_Idx;


    if(uc_Net == CAN_NET)
    {
        w16_Buf_Active =  CANMsgBuffering::Buf_Active_CAN;
        w16_InUse_Idx = &CANMsgBuffering::InUse_Idx_CAN;
        st_p_Front_EventNo = & CANMsgBuffering::EV_Buf_CAN[ w16_Buf_Active][ *w16_InUse_Idx].EventNo;
        uw32_EVSize = EVENTSBUFFERSIZE_CAN;

    }
    else //if(uc_Net == ETHERNET)
    {
        w16_Buf_Active =  EventsBuffering::Buf_Active;
        w16_InUse_Idx = &EventsBuffering::InUse_Idx;
        st_p_Front_EventNo = & EventsBuffering::EV_Buf[  w16_Buf_Active][ * w16_InUse_Idx].EventNo;
        uw32_EVSize = EVENTSBUFFERSIZE;
    }

    if (startPoint == -1)										// if already checked one full round in Active buffer and no empty left.
        return startPoint;

    if (startPoint >= *w16_InUse_Idx)								// if the buffer is not used up yet, look into unused slots.
    {
        for(i = startPoint; i < uw32_EVSize; i ++)
        {
            if(uc_Net == CAN_NET)
            {
                st_p_Front_EventNo = & CANMsgBuffering::EV_Buf_CAN[ w16_Buf_Active][i].EventNo;
            }
            else //if(uc_Net == ETHERNET)
            {
                st_p_Front_EventNo = & EventsBuffering::EV_Buf[ w16_Buf_Active][i].EventNo;
            }
            if( *st_p_Front_EventNo == 0)  // 活跃区专用于存储发送的事件，找到没存的空闲位置。
                return i;
        }
    }

    for (i = 0; i < uw32_EVSize; i ++)							// if buffer used up, search confirmed slot which is available
    {
        if(uc_Net == CAN_NET)
        {
            st_p_Front_EventNo = & CANMsgBuffering::EV_Buf_CAN[ w16_Buf_Active][i].EventNo;
        }
        else //if(uc_Net == ETHERNET)
        {
            st_p_Front_EventNo = & EventsBuffering::EV_Buf[ w16_Buf_Active][i].EventNo;
        }
        if( *st_p_Front_EventNo == 0)
            return i;
    }

    return -1;

}



