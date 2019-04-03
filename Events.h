#ifndef EVENTS_LIB
#define EVENTS_LIB

#include "RingBuf.h"

//enum EVENT{B0_ON_PRESS, B0_ON_RELEASE, B1_ON_PRESS, B1_ON_RELEASE, ON_PRESS, ON_RELEASE};

class Event
{
  public:
    enum EVENT{B0_ON_PRESS, B0_ON_RELEASE, B1_ON_PRESS, B1_ON_RELEASE, ON_PRESS, ON_RELEASE};
  
    EVENT type;
    uint8_t button_idx;
    Event(){};
    Event(EVENT t, uint8_t b);
};
Event::Event(EVENT t, uint8_t b)
{
  type = t;
  button_idx = b;
}

//class EventQueue
//{
//  private:
//    static const int QUEUE_SIZE = 32;
//    int start, end;
//    int queue[QUEUE_SIZE];
//  public:
//    EventQueue();
//    int getQueueSize();
//    int popQueue();
//    void pushQueue();
//    int peekQueue();
//    int peekQueue(int idx);
//};

extern RingBuf<Event, 32> eventBuffer;
RingBuf<Event, 32> eventBuffer;


#endif
