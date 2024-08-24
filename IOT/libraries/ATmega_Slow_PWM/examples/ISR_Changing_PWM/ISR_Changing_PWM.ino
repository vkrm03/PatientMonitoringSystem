/****************************************************************************************************************************
  ISR_Changing_PWM.ino
  For AVR ATmega164, ATmega324, ATmega644, ATmega1284 with MightyCore
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/ATmega_Slow_PWM
  Licensed under MIT license

  Now with we can use these new 16 ISR-based PWM channels, while consuming only 1 hwarware Timer.
  Their independently-selected, maximum interval is practically unlimited (limited only by unsigned long miliseconds)
  The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
  Therefore, their executions are not blocked by bad-behaving functions / tasks.
  This important feature is absolutely necessary for mission-critical tasks.
*****************************************************************************************************************************/

// Select just 1 TIMER to be true
#define USE_TIMER_1     true
#define USE_TIMER_2     false
// TIMER_3 Only valid for ATmega1284 and ATmega324PB (not ready in core yet)
#define USE_TIMER_3     false
// TIMER_4 Only valid for ATmega324PB, not ready in core yet
#define USE_TIMER_4     false
// These define's must be placed at the beginning before #include "ATmega_Slow_PWM.h"
// _PWM_LOGLEVEL_ from 0 to 4
// Don't define _PWM_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
#define _PWM_LOGLEVEL_      3

#if (_PWM_LOGLEVEL_ > 3)
  #if USE_TIMER_1
    #warning Using Timer1
  #elif USE_TIMER_1
    #warning Using Timer3
  #endif
#endif

#define USING_MICROS_RESOLUTION       true    //false

// Default is true, uncomment to false
//#define CHANGING_PWM_END_OF_CYCLE     false

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "ATmega_Slow_PWM.h"

#define LED_OFF             HIGH
#define LED_ON              LOW

#ifndef LED_BUILTIN
  #define LED_BUILTIN       13
#endif

#ifndef LED_BLUE
  #define LED_BLUE          10
#endif

#ifndef LED_RED
  #define LED_RED           11
#endif

// Don't change these numbers to make higher Timer freq. System can hang
#define HW_TIMER_INTERVAL_MS        0.1f
#define HW_TIMER_INTERVAL_FREQ      10000L

volatile uint32_t startMicros = 0;

// Init ATmega_Slow_PWM, each can service 16 different ISR-based PWM channels
ATmega_Slow_PWM ISR_PWM;

//////////////////////////////////////////////////////

void TimerHandler()
{ 
  ISR_PWM.run();
}

//////////////////////////////////////////////////////

#define USING_PWM_FREQUENCY     true

//////////////////////////////////////////////////////

// You can assign pins here. Be carefull to select good pin to use or crash
uint32_t PWM_Pin    = LED_BUILTIN;

// You can assign any interval for any timer here, in Hz
float PWM_Freq1   = 1.0f;
// You can assign any interval for any timer here, in Hz
float PWM_Freq2   = 2.0f;

// You can assign any interval for any timer here, in microseconds
uint32_t PWM_Period1 = 1000000 / PWM_Freq1;
// You can assign any interval for any timer here, in microseconds
uint32_t PWM_Period2 = 1000000 / PWM_Freq2;

// You can assign any duty_cycle for any PWM here, from 0-100
float PWM_DutyCycle1  = 50.0;
// You can assign any duty_cycle for any PWM here, from 0-100
float PWM_DutyCycle2  = 90.0;

// Channel number used to identify associated channel
int channelNum;

////////////////////////////////////////////////

void setup()
{
  Serial.begin(115200);
  while (!Serial && millis() < 5000);

  Serial.print(F("\nStarting ISR_Changing_PWM on ")); Serial.println(BOARD_NAME);
    Serial.println(ATMEGA_SLOW_PWM_VERSION);
  Serial.print(F("CPU Frequency = ")); Serial.print(F_CPU / 1000000); Serial.println(F(" MHz"));

  // Timer0 is already used for micros(), millis(), delay(), etc and can't be used
  // Select Timer 1-2
  // Timer 2 is 8-bit timer, only for higher frequency

#if USING_HW_TIMER_INTERVAL_MS

/////////////////////////////////////////

#if USE_TIMER_1

  ITimer1.init();

  // Using ATmega324 with 16MHz CPU clock ,
  // For 16-bit timer 1, set frequency from 0.2385 to some KHz
  // For 8-bit timer 2 (prescaler up to 1024, set frequency from 61.5Hz to some KHz

  if (ITimer1.attachInterruptInterval(HW_TIMER_INTERVAL_MS, TimerHandler))
  {
    Serial.print(F("Starting  ITimer1 OK, micros() = ")); Serial.println(micros());
  }
  else
    Serial.println(F("Can't set ITimer1. Select another freq. or timer"));

#elif USE_TIMER_2

  ITimer2.init();

  if (ITimer2.attachInterruptInterval(HW_TIMER_INTERVAL_MS, TimerHandler))
  {
    Serial.print(F("Starting  ITimer2 OK, micros() = ")); Serial.println(micros());
  }
  else
    Serial.println(F("Can't set ITimer2. Select another freq. or timer"));
    
#elif USE_TIMER_3

  ITimer3.init();

  if (ITimer3.attachInterruptInterval(HW_TIMER_INTERVAL_MS, TimerHandler))
  {
    Serial.print(F("Starting  ITimer3 OK, micros() = ")); Serial.println(micros());
  }
  else
    Serial.println(F("Can't set ITimer3. Select another freq. or timer"));

#endif

/////////////////////////////////////////

#else

/////////////////////////////////////////

#if USE_TIMER_1

  ITimer1.init();

  // Using ATmega324 with 16MHz CPU clock ,
  // For 16-bit timer 1, set frequency from 0.2385 to some KHz
  // For 8-bit timer 2 (prescaler up to 1024, set frequency from 61.5Hz to some KHz

  if (ITimer1.attachInterrupt(HW_TIMER_INTERVAL_FREQ, TimerHandler))
  {
    Serial.print(F("Starting  ITimer1 OK, micros() = ")); Serial.println(micros());
  }
  else
    Serial.println(F("Can't set ITimer1. Select another freq. or timer"));

#elif USE_TIMER_2

  ITimer2.init();

  if (ITimer2.attachInterrupt(HW_TIMER_INTERVAL_FREQ, TimerHandler))
  {
    Serial.print(F("Starting  ITimer2 OK, micros() = ")); Serial.println(micros());
  }
  else
    Serial.println(F("Can't set ITimer2. Select another freq. or timer"));

#elif USE_TIMER_3

  ITimer3.init();

  if (ITimer3.attachInterrupt(HW_TIMER_INTERVAL_FREQ, TimerHandler))
  {
    Serial.print(F("Starting  ITimer3 OK, micros() = ")); Serial.println(micros());
  }
  else
    Serial.println(F("Can't set ITimer3. Select another freq. or timer"));

#endif

#endif
}

void loop()
{
  Serial.print(F("Using PWM Freq = ")); Serial.print(PWM_Freq1); Serial.print(F(", PWM DutyCycle = ")); Serial.println(PWM_DutyCycle1);

#if USING_PWM_FREQUENCY

  // You can use this with PWM_Freq in Hz
  channelNum = ISR_PWM.setPWM(PWM_Pin, PWM_Freq1, PWM_DutyCycle1);

#else
#if USING_MICROS_RESOLUTION
  // Or using period in microsecs resolution
  channelNum = ISR_PWM.setPWM_Period(PWM_Pin, PWM_Period1, PWM_DutyCycle1);
#else
  // Or using period in millisecs resolution
  channelNum = ISR_PWM.setPWM_Period(PWM_Pin, PWM_Period1 / 1000.0, PWM_DutyCycle1);
#endif
#endif

  delay(10000);

  ISR_PWM.deleteChannel((unsigned) channelNum);

  Serial.print(F("Using PWM Freq = ")); Serial.print(PWM_Freq2); Serial.print(F(", PWM DutyCycle = ")); Serial.println(PWM_DutyCycle2);

#if USING_PWM_FREQUENCY

  // You can use this with PWM_Freq in Hz
  channelNum = ISR_PWM.setPWM(PWM_Pin, PWM_Freq2, PWM_DutyCycle2);

#else
#if USING_MICROS_RESOLUTION
  // Or using period in microsecs resolution
  channelNum = ISR_PWM.setPWM_Period(PWM_Pin, PWM_Period2, PWM_DutyCycle2);
#else
  // Or using period in millisecs resolution
  channelNum = ISR_PWM.setPWM_Period(PWM_Pin, PWM_Period2 / 1000.0, PWM_DutyCycle2);
#endif
#endif

  delay(10000);

  ISR_PWM.deleteChannel((unsigned) channelNum);
}
