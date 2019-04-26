#ifndef SNAKE_LIB
#define SNAKE_LIB
#include <Arduino.h>
#include <FastLED.h>
#include "Helper.h"
#include "ButtonHandler.h"
#include "Events.h"
#define GAME_WIDTH kMatrixWidth
#define GAME_HEIGHT kMatrixHeight
#define WRAP false

class SnakeNode
{
  public:
    int x, y;
    uint32_t color;

    SnakeNode();
    SnakeNode(int x_, int y_, int color_);
    static SnakeNode randNode();
};
SnakeNode::SnakeNode()
{
  //SnakeNode node = randNode();
  x = rand() % GAME_WIDTH;
  y = rand() % GAME_HEIGHT;
  color = (rand() % 256) << 16;
  color |= (rand() % 256) << 8;
  color |= (rand() % 256);
}
SnakeNode::SnakeNode(int x_, int y_, int color_)
{
  x = x_;
  y = y_;
  color = color_;
}
SnakeNode SnakeNode::randNode()
{
  SnakeNode out;
  out.x = rand() % GAME_WIDTH;
  out.y = rand() % GAME_HEIGHT;
  out.color = (rand() % 256) << 16;
  out.color |= (rand() % 256) << 8;
  out.color |= (rand() % 256);
  return out;
}

class Snake
{
  private:
    const static int MAX_LEN = 24;
    int len = 3;
    SnakeNode body[MAX_LEN];
    SnakeNode dot; 
    int dir;
    bool end_game;


  public:
    enum DIRECTION{UP, RIGHT, DOWN, LEFT, NUM_DIR};

    Snake();
    void reset();

    void update();
    void draw(CRGB* const leds);
    bool game_over() {return end_game;}
    int handleOnPress(Event e, int dir_);
    int handleOnRelease(Event e, int dir_);
    
};


Snake::Snake()
{
  //Serial.println("Enter construct");
  reset();
  //Serial.println("Exit construct");
}
void Snake::reset()
{
  Serial.println("Enter Reset");
  len = 3;
  dir = RIGHT;
  end_game = false;
  for (int i=0; i<len; i++)
  {
    body[i].x = (len-1)-i;
    body[i].y = 0;
    body[i].color = 0xFFFFFF;
  }
  while(1)
  {
    SnakeNode node = SnakeNode::randNode();
    bool pass = true;
    for (int i=0; i<len; i++)
    {
      if (body[i].x == node.x && body[i].y == node.y)
      {
        pass = false;
        break;
      }
    }
    if (pass)
    {
      dot = node;
      break;
    }
  }
  dot.x = 2;
  dot.y = 2;
  Serial.println("Exit Reset");
}

void Snake::update()
{
  //Serial.println("Enter Update");
  SnakeNode next = body[0];


  /*
  if (button_states[0] == ON_PRESS || button_states[0] == ON_RELEASE)
  {
    //Turn left
    dir--;
    if (dir < 0)
      dir = NUM_DIR - 1;
  }
  else if (button_states[1] == ON_PRESS || button_states[1] == ON_RELEASE)
  {
    //Turn right
    dir++;
    if (dir >= NUM_DIR)
      dir = 0;
  }
  resetButtonStates();
  */
  int dir_ = dir;
  while (!eventBuffer.isEmpty())
  {
    Event e;
    if (!eventBuffer.lockedPop(e))
    {
      //Handle some error condition
    }
    
    switch (e.type)
    {
      case Event::ON_PRESS:
      {
        handleOnPress(e, dir_);
        break;
      }
      case Event::ON_RELEASE:
      {
        handleOnRelease(e, dir_);
        break;
      }
    }
  }

  
  switch(dir)
  {
    case UP:
    {
      next.y--;
      break;
    }
    case RIGHT:
    {
      next.x++;
      break;
    }
    case DOWN:
    {
      next.y++;
      break;
    }
    case LEFT:
    {
      next.x--;
      break;
    }
  }

  //Check for game edge collision
  if ((next.x >= GAME_WIDTH || next.y >= GAME_HEIGHT || next.x < 0 || next.y <0) && !WRAP)
  {
    end_game = true;
    return;
  }

  //Update snake body
  for (int i=len-1; i>=1; i--)
  {
    body[i].x = body[i-1].x;
    body[i].y = body[i-1].y;
  }
  body[0] = next;

  for (int i=1; i<len; i++)
  {
    if (next.x == body[i].x && next.y == body[i].y)
    {
      end_game = true;
      return;
    }
  }

  //Check for dot collision
  if (body[0].x == dot.x && body[0].y == dot.y)
  {
    if (len < MAX_LEN)
    {
      body[len] = dot;
      body[len].x = body[len-1].x;
      body[len].y = body[len-1].y;
      len++;
    }
    while(1)
    {
      SnakeNode dot_ = SnakeNode::randNode();
      bool pass = true;
      for (int i=0; i<len; i++)
      {
        if (dot_.x == body[i].x && dot_.y == body[i].y)
        {
          pass = false;
          break;
        }
      }
      if (pass)
      {
        dot = dot_;
        break;
      }
    }
  }


  

  //Serial.println("Exit update");
}

void Snake::draw(CRGB* const leds)
{
  //Serial.println("Enter draw");
  FastLED.clear();
  for (int i=0; i<len; i++)
  {
    leds[XY(body[i].x, body[i].y)] = body[i].color;  
  }
  leds[XY(dot.x, dot.y)] = dot.color;
  FastLED.show();
  //Serial.println("Exit Draw");
}

int Snake::handleOnPress(Event e, int dir_)
{
  if (e.button_idx == 0)
  {
    dir = dir_-1;
    if (dir < 0)
      dir = NUM_DIR - 1;
  }
  else if (e.button_idx == 1)
  {
    dir = dir_ + 1;
    if (dir >= NUM_DIR)
      dir = 0;
  }
  return 0;
}
int Snake::handleOnRelease(Event e, int dir_)
{
  return 0;
}

#endif
