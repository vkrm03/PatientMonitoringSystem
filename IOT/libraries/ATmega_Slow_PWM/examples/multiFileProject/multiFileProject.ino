/****************************************************************************************************************************
  multiFileProject.ino
  
  For AVR ATmega164, ATmega324, ATmega644, ATmega1284 with MightyCore
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/ATmega_Slow_PWM
  Licensed under MIT license
*****************************************************************************************************************************/

// To demo how to include files in multi-file Projects

// Select just 1 TIMER to be true
#define USE_TIMER_1     true
#define USE_TIMER_2     false
// TIMER_3 Only valid for ATmega1284 and ATmega324PB (not ready in core yet)
#define USE_TIMER_3     false
// TIMER_4 Only valid for ATmega324PB, not ready in core yet
#define USE_TIMER_4     false

#define ATMEGA_SLOW_PWM_VERSION_MIN_TARGET      F("ATmega_Slow_PWM v1.0.0")
#define ATMEGA_SLOW_PWM_VERSION_MIN             1000000

#include "multiFileProject.h"

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "ATmega_Slow_PWM.h"

void setup() 
{
  Serial.begin(115200);
  while (!Serial && millis() < 5000);
  
  Serial.println(F("\nStart multiFileProject on ")); Serial.println(BOARD_NAME);
  Serial.println(ATMEGA_SLOW_PWM_VERSION);

#if defined(ATMEGA_SLOW_PWM_VERSION_MIN)
  if (ATMEGA_SLOW_PWM_VERSION_INT < ATMEGA_SLOW_PWM_VERSION_MIN)
  {
    Serial.print(F("Warning. Must use this example on Version equal or later than : "));
    Serial.println(ATMEGA_SLOW_PWM_VERSION_MIN_TARGET);
  }
#endif
}

void loop() 
{
  // put your main code here, to run repeatedly:
}
