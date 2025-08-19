#include <string.h>
#include <assert.h>

#include "masImpl.h"


/********************************************************************************************
*
*********************************************************************************************/
#define MAS_EVENT_MAX 64


/********************************************************************************************
*
*********************************************************************************************/
typedef struct masEventQueue_
{
    masEvent Events[MAS_EVENT_MAX];
    int32_t  AddIdx;
    int32_t  GetIdx;
    int32_t  Count;
} masEventQueue;


/********************************************************************************************
*
*********************************************************************************************/
static masEventQueue EventQueue;


/********************************************************************************************
*
*********************************************************************************************/
void mas_impl_event_init()
{

}

void mas_impl_event_deinit()
{

}

masEvent* mas_impl_event_add(masEventType EventType)
{
    if(EventType <= EventType_None || EventType >= EventType_Count)
        assert(false && "EventType is invalid");

    int32_t   AddIdx = EventQueue.AddIdx;
    masEvent *Event  = &EventQueue.Events[AddIdx];
    if(Event->Type == EventType_None)
    {
        EventQueue.AddIdx++;
        if(EventQueue.AddIdx >= MAS_EVENT_MAX)
            EventQueue.AddIdx = 0;
        return Event;
    }

    assert(false && "EventQueue is full, can't add events");

    return NULL;
}

bool mas_impl_event_get(masEvent* Event)
{
    int32_t   GetIdx       = EventQueue.GetIdx;
    masEvent *CurrentEvent = &EventQueue.Events[GetIdx];
    if(Event->Type != EventType_None)
    {
        memcpy(Event, CurrentEvent, sizeof(masEvent));
        memset(CurrentEvent, 0, sizeof(masEvent));

        EventQueue.GetIdx++;
        if(EventQueue.GetIdx >= MAS_EVENT_MAX)
            EventQueue.GetIdx = 0;
        return true;
    }

    return false;
}