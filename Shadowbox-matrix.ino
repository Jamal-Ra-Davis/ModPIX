#include <FastLED.h>
#include "ButtonHandler.h"
#include "Snake.h"
#include "Helper.h"
#include "Events.h"

#define LED_PIN  11

#define COLOR_ORDER GRB
#define CHIPSET     WS2811

#define BRIGHTNESS 128
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)




CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
CRGB* const leds( leds_plus_safety_pixel + 1);

Snake* snake = NULL;


// Demo that USES "XY" follows code below

int delay_diff = 0;
void loop()
{
  /*
    if (button_states[0] == ON_PRESS)
    {
      leds[XY(0, 0)] = 0xFF0000;
    }
    else if (button_states[0] == ON_RELEASE)
    {
      leds[XY(0, 0)] = 0x000000;
    }

    if (button_states[1] == ON_PRESS)
    {
      leds[XY(5, 0)] = 0x00FF00;
    }
    else if (button_states[1] == ON_RELEASE)
    {
      leds[XY(5, 0)] = 0x000000;
    }
    resetButtonStates();
    FastLED.show();
    delay(10);
    
    return;
  */
  
    //Serial.println("In Loop");
    //delay(200);
  
    snake->update();
    if (snake->game_over())
    {
      snake->reset();
      delay_diff -= 16;
      Serial.println("Snake reset");
    }
    snake->draw(leds);
    int del = 250 - delay_diff;
    if (del < 10)
      del = 10;
    delay_diff++;
    delay(del);
    return;

    
    FastLED.clear();
    FastLED.setBrightness(BRIGHTNESS);
    for (int j=0; j<8; j++)
    {
      for (int i=0; i<8; i++)
      {
        leds[XY(i, j)] = 0xFF0000;
        FastLED.show();
        delay(100);
      }
    }
    return;
  
    uint32_t ms = millis();
    int32_t yHueDelta32 = ((int32_t)cos16( ms * (27/1) ) * (350 / kMatrixWidth));
    int32_t xHueDelta32 = ((int32_t)cos16( ms * (39/1) ) * (310 / kMatrixHeight));
    DrawOneFrame( ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
    if( ms < 5000 ) {
      FastLED.setBrightness( scale8( BRIGHTNESS, (ms * 256) / 5000));
    } else {
      FastLED.setBrightness(BRIGHTNESS);
    }
    FastLED.show();
}

void DrawOneFrame( byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8)
{
  byte lineStartHue = startHue8;
  for( byte y = 0; y < kMatrixHeight; y++) {
    lineStartHue += yHueDelta8;
    byte pixelHue = lineStartHue;      
    for( byte x = 0; x < kMatrixWidth; x++) {
      pixelHue += xHueDelta8;
      leds[ XY(x, y)]  = CHSV( pixelHue, 255, 255);
    }
  }
}


void setup() {
  cli();
  //PCICR |= (1 <<PCIE2);
  //PCMSK2 |= ((1 << PCINT18) | (1 << PCINT19));
  //PCICR |= (1 << PCIE0);
  //  PCMSK2 |= ((1 << PCINT1) | (1 << PCINT2));
  enableButtonInterrupts();
  sei();
  initButtonHandler();
  
  Serial.begin(9600);
  delay(2000);
  //while(1)
  //{
    Serial.println("Starting up");
    delay(200);
  //}
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness( BRIGHTNESS );

  


  Serial.println("Init leds");

  snake = new Snake();
  snake->reset();
  snake->draw(leds);
  delay(250);
  FastLED.clear();
  FastLED.show();
}
