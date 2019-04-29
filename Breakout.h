#ifndef BREAKOUT_LIB
#define BREAKOUT_LIB

#include "Events.h"
#include "Helper.h"
#define GAME_WIDTH kMatrixWidth
#define GAME_HEIGHT kMatrixHeight

class Breakout
{
  private:
    const static uint8_t PADDLE_WIDTH = 3;
    const static int paddle_y = GAME_HEIGHT - 1;
    const static int N_ROWS = 3;
    uint8_t blocks[N_ROWS];
    uint32_t row_colors[N_ROWS];

    
    int paddle_x;
    int cnt;
    //int paddle_vel;
    
    int ball_pos_x, ball_pos_y;
    int ball_vel_x, ball_vel_y;

    int ball_trail[3][2];

    bool end_game;
    bool press_left, press_right;
    bool on_press_left, on_press_right;

    bool checkBlockCollision();

  public:

    Breakout();
    void reset();

    void update();
    void draw(CRGB* const leds);
    bool game_over() {return end_game;}
    int handleOnPress(Event e);
    int handleOnRelease(Event e);
};


Breakout::Breakout()
{
  reset();
}
void Breakout::reset()
{
  paddle_x = 0;
  //paddle_vel = 0;

  ball_pos_x = 1;
  ball_pos_y = paddle_y - 1;

  ball_vel_x = 1;
  ball_vel_y = -1;

  end_game = false;
  press_left = false;
  press_right = false;
  cnt = 0;

  for (int i=0; i<N_ROWS; i++)
  {
    blocks[i] = 0xFF;
    uint8_t color_idx0 = rand() % 3;
    uint8_t color_idx1 = rand() % 3;

    //row_colors[i] = (0xFF << 8*color_idx0) | (0xFF << 8*color_idx1);
    row_colors[i] = (((uint32_t)0xFF) << 8*i);
    Serial.print("Row color[");
    Serial.print(i);
    Serial.print("]: ");
    Serial.println(row_colors[i], HEX);
  }

  for (int i=0; i<3; i++)
  {
    ball_trail[i][0] = ball_pos_x;
    ball_trail[i][1] = ball_pos_y;
  }
  
  //row_colors[0] = 0x0000FF;
  //row_colors[1] = 0x00FF00;
  //row_colors[2] = 0xFF0000;

}
void Breakout::update()
{
  cnt++;
  if (cnt >= 2)
    cnt = 0;

  on_press_left = false;
  on_press_right = false;
  
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
        handleOnPress(e);
        break;
      }
      case Event::ON_RELEASE:
      {
        handleOnRelease(e);
        break;
      }
    }
  }

  //Serial.print("press_left: ");
  //Serial.println(press_left);
  //Serial.print("press_right: ");
  //Serial.println(press_right);

  //Determine paddle vel
  int paddle_vel = 0;
  if (press_left && !press_right)
  {
    paddle_vel = -1;
  }
  else if (!press_left && press_right)
  {
    paddle_vel = 1;
  }

  if (on_press_left && !on_press_right)
  {
    paddle_vel = -1;
  }
  else if (!on_press_left && on_press_right)
  {
    paddle_vel = 1;
  }

  //Update paddle pos based on paddle vel
  paddle_x += paddle_vel;
  if (paddle_x < 0)
  {
    paddle_x = 0;
    paddle_vel = 0;
  }
  else if ((paddle_x + PADDLE_WIDTH) > GAME_WIDTH)
  {
    paddle_x = GAME_WIDTH - PADDLE_WIDTH;
    paddle_vel = 0;
  }


  if (cnt % 2 == 0)
    return;


  for (int i=(3-1); i>=0; i--)
  {
    if (i==0)
    {
      ball_trail[i][0] = ball_pos_x;
      ball_trail[i][1] = ball_pos_y;
    }
    else
    {
      ball_trail[i][0] = ball_trail[i-1][0];
      ball_trail[i][1] = ball_trail[i-1][1];
    }
  }
    
  //Update ball pos
  ball_pos_x += ball_vel_x;
  ball_pos_y += ball_vel_y;

  if (ball_pos_x < 0)
  {
    ball_vel_x *= -1;
    ball_pos_x = 1;
  }
  else if (ball_pos_x >= GAME_WIDTH)
  {
    ball_vel_x *= -1;
    ball_pos_x = GAME_WIDTH-2;
  }

  if (ball_pos_y < 0)
  {
    ball_vel_y *= -1;
    ball_pos_y = 1;
  }
  else if (ball_pos_y >= GAME_HEIGHT)
  {
    ball_vel_y *= -1;
    ball_pos_y = GAME_HEIGHT-2;
  }

  //Handle paddle collisions
  for (int i=0; i<PADDLE_WIDTH; i++)
  {
    if (ball_pos_x == (paddle_x + i) && ball_pos_y == paddle_y)
    {
      //ball_pos_y = paddle_y - 1;
      //ball_pos_x = paddle_x + i - 1;
      //ball_vel_y *= -1;
      //break;
      if (i == 0 && ball_vel_x > 0)
      {
        ball_pos_y = paddle_y - 2;
        ball_pos_x = paddle_x + i - 2;
        ball_vel_x *= -1;
        ball_vel_y *= -1;
      }
      else if (i == (PADDLE_WIDTH-1) && ball_vel_x < 0)
      {
        ball_pos_y = paddle_y - 2;
        ball_pos_x = paddle_x + i + 2;
        ball_vel_x *= -1;
        ball_vel_y *= -1;
      }
      else
      {
        ball_pos_y = paddle_y - 2;
        ball_pos_x = paddle_x + i;
        ball_vel_y *= -1;
      }
    }
  }


  //Handle block Collisions
  bool check = checkBlockCollision();
  if (check)
  {
    int prev_x = ball_trail[0][0];
    int prev_y = ball_trail[0][1];

    //hits in corner group from right
    if ((prev_y < N_ROWS)&& (blocks[ball_pos_y] & (1 << prev_x)) && (blocks[prev_y] & (1 << ball_pos_x)))
    {
      blocks[ball_pos_y] &= ~(1 << ball_pos_x);
      blocks[prev_y] &= ~(1 << ball_pos_x);
      blocks[ball_pos_y] &= ~(1 << prev_x);

      if (ball_vel_x > 0)
        ball_pos_x -= 2;
      else
        ball_pos_x += 2;

      if (ball_vel_y > 0)
        ball_pos_y -= 2;
      else
        ball_pos_y += 2;
      
      ball_vel_y *= -1;
      ball_vel_x *= -1;
      
      Serial.println("In First");
    }
    //Hits flat on
    else if ((ball_pos_y < N_ROWS) && (blocks[ball_pos_y] & (1 << prev_x)))
    {
      blocks[ball_pos_y] &= ~(1 << prev_x);
      ball_vel_y *= -1;
      ball_pos_y += 2;

      Serial.println("In Second");
    }
    //hits in corner group from left
    else// if (false)
    {
      Serial.print("In 3rd: ");
      Serial.println(check);
      Serial.print("(x, y): (");
      Serial.print(ball_pos_x);
      Serial.print(",");
      Serial.print(ball_pos_y);
      Serial.println(")");
      Serial.println(blocks[ball_pos_y], BIN);
      Serial.println(checkBlockCollision());


      
      blocks[ball_pos_y] &= ~(1 << ball_pos_x);

      if (ball_vel_x > 0)
        ball_pos_x -= 2;
      else
        ball_pos_x += 2;

      if (ball_vel_y > 0)
        ball_pos_y -= 2;
      else
        ball_pos_y += 2;

      ball_vel_x *= -1;
      ball_vel_y *= -1;

     
    }
    /*
    //hits single from right
    else if(false)
    {
      
    }
    //hits single from left
    else if(false)
    {
      
    }
    */
  }
  
}

void Breakout::draw(CRGB* const leds)
{
  FastLED.clear();

  
  //Draw Paddle
  for (int i=0; i<PADDLE_WIDTH; i++)
  {
    leds[XY(i+paddle_x, paddle_y)] = 0xFFFFFF;
  }

  //Draw Ball Trail
  uint32_t ball_r = 0xFF; 
  uint32_t ball_g = 0x00;
  uint32_t ball_b = 0xFF;

  for (int i=(3-1); i>=0; i--)
  {
    uint32_t r = ball_r >> (i+2);
    uint32_t g = ball_g >> (i+2);
    uint32_t b = ball_b >> (i+2);
    leds[XY(ball_trail[i][0], ball_trail[i][1])] = (r << 16) | (g << 8) | b;
  }

  //Draw Blocks
  for (int i=0; i<N_ROWS; i++)
  {
    for (int j=0; j<8; j++)
    {
      if (blocks[i] & (1 << j))
        leds[XY(j, i)] = row_colors[i];
    }
  }


  //Draw Ball
  leds[XY(ball_pos_x, ball_pos_y)] = (ball_r << 16) | (ball_g << 8) | (ball_b);//0xFF00FF;
  FastLED.show();
}

int Breakout::handleOnPress(Event e)
{
  if (e.button_idx == 0)
  {
    press_left = true;
    on_press_left = true;
  }
  else if (e.button_idx == 1)
  {
    press_right = true;
    on_press_right = true;
  }
  return 0;
}
int Breakout::handleOnRelease(Event e)
{
  if (e.button_idx == 0)
  {
    press_left = false;
  }
  else if (e.button_idx == 1)
  {
    press_right = false;
  }
  return 0;
}
bool Breakout::checkBlockCollision()
{
  if (ball_pos_y >= N_ROWS)
    return false;
  if (blocks[ball_pos_y] & (1 << ball_pos_x))
    return true;
  return false;
}


#endif
