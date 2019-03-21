#ifndef BUTTON_HANDLER_LIB
#define BUTTON_HANDLER_LIB

enum STATES{IDLE, ON_PRESS, ON_RELEASE};

int intPins[2] = {2, 3};
volatile int prevStates[2] = {0, 0};
volatile int button_states[2] = {IDLE, IDLE};



void enableButtonInterrupts()
{
  PCICR |= (1 <<PCIE2);
  PCMSK2 |= ((1 << PCINT18) | (1 << PCINT19));
}
void resetButtonStates()
{
  button_states[0] = IDLE;
  button_states[1] = IDLE;
}
void initButtonHandler()
{
  resetButtonStates();
  for (int i=0; i<2; i++)
  {
    pinMode(intPins[i], INPUT_PULLUP);
    prevStates[i] = digitalRead(intPins[i]);
  }
}
ISR(PCINT2_vect)
{
  //Serial.println("Interrupt:");
  int states[2];
  for (int i=0; i<2; i++)
    states[i] = digitalRead(intPins[i]);
  delayMicroseconds(200);

  for (int i=0; i<2; i++)
  {
    if (states[i] == digitalRead(intPins[i]))
    {
      if (prevStates[i] == 1 && states[i] == 0)
      {
        //On press (falling edge)
        button_states[i] = ON_PRESS;
        Serial.print("Button ");
        Serial.print(i);
        Serial.println(": ON_PRESS");
        Serial.println();
      }
      else if (prevStates[i] == 0 && states[i] == 1)
      {
        //On release (rising edge)
        //button_states[i] = ON_RELEASE;
        Serial.print("Button ");
        Serial.print(i);
        Serial.println(": ON_RELEASE");
        Serial.println();
      }
      prevStates[i] = states[i];
    }
  }
}

class EventQueue
{
  private:
    static const int QUEUE_SIZE = 32;
    int start, end;
    int queue[QUEUE_SIZE];
  public:
    EventQueue();
    int getQueueSize();
    int popQueue();
    void pushQueue();
    int peekQueue();
    int peekQueue(int idx);
};


#endif
