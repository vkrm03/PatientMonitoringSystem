/****************************************************************************************************************************
  ATmega_Slow_PWM_ISR.hpp
  For AVR ATmega164, ATmega324, ATmega644, ATmega1284 with MightyCore
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/ATmega_Slow_PWM
  Licensed under MIT license

  Now with we can use these new 16 ISR-based PWM channels, while consuming only 1 hwarware Timer.
  Their independently-selected, maximum interval is practically unlimited (limited only by unsigned long miliseconds)
  The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
  Therefore, their executions are not blocked by bad-behaving functions / tasks.
  This important feature is absolutely necessary for mission-critical tasks.

  Version: 1.0.0

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K Hoang      23/08/2022 Initial coding for AVR ATmega164, ATmega324, ATmega644, ATmega1284 with MightyCore
*****************************************************************************************************************************/

#pragma once

#ifndef ATmega_Slow_PWM_ISR_HPP
#define ATmega_Slow_PWM_ISR_HPP

#if defined(BOARD_NAME)
  #undef BOARD_NAME
#endif

#ifndef _PWM_LOGLEVEL_
  #define _PWM_LOGLEVEL_        1
#endif


/////////////////////////////////////////////////////////////////////

#if ( defined(__AVR_ATmega164P__) || defined(__AVR_ATmega164A__)  || \
      defined(__AVR_ATmega324P__) || defined(__AVR_ATmega324PA__) || defined(__AVR_ATmega324A__) || defined(__AVR_ATmega324PB__) || \
      defined(__AVR_ATmega644__)  || defined(__AVR_ATmega644A__)  || defined(__AVR_ATmega644P__) || \
      defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) )

  #if ( defined(__AVR_ATmega164P__) || defined(__AVR_ATmega164A__) )
  
    #if defined(TIMER_INTERRUPT_USING_ATMEGA_164)
      #undef TIMER_INTERRUPT_USING_ATMEGA_164
    #endif
    #define TIMER_INTERRUPT_USING_ATMEGA_164      true
  
    #define BOARD_NAME    F("MightyCore ATmega164(A/P)")
    
  #elif ( defined(__AVR_ATmega324P__) || defined(__AVR_ATmega324PA__) || defined(__AVR_ATmega324A__) )
  
    #if defined(TIMER_INTERRUPT_USING_ATMEGA_324)
      #undef TIMER_INTERRUPT_USING_ATMEGA_324
    #endif
    #define TIMER_INTERRUPT_USING_ATMEGA_324      true
    
    #define BOARD_NAME    F("MightyCore ATmega324(A/P/PA)")
      
  #elif ( defined(__AVR_ATmega324PB__) )
  
    #if defined(TIMER_INTERRUPT_USING_ATMEGA_324PB)
      #undef TIMER_INTERRUPT_USING_ATMEGA_324PB
    #endif
    #define TIMER_INTERRUPT_USING_ATMEGA_324PB    true
    
    #define BOARD_NAME    F("MightyCore ATmega324PB")
    
  #elif ( defined(__AVR_ATmega644__)  || defined(__AVR_ATmega644A__)  || defined(__AVR_ATmega644P__) )
  
    #if defined(TIMER_INTERRUPT_USING_ATMEGA_644)
      #undef TIMER_INTERRUPT_USING_ATMEGA_644
    #endif
    #define TIMER_INTERRUPT_USING_ATMEGA_644    true
    
    #define BOARD_NAME    F("MightyCore ATmega644(A/P)")
    
  #elif ( defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) )
  
    #if defined(TIMER_INTERRUPT_USING_ATMEGA_1284)
      #undef TIMER_INTERRUPT_USING_ATMEGA_1284
    #endif
    #define TIMER_INTERRUPT_USING_ATMEGA_1284   true
    
    #define BOARD_NAME    F("MightyCore ATmega1284(P)")
    
  #else
    #define BOARD_NAME    F("Unknown Board")
  #endif
  
#elif ( defined(__AVR_ATmega8535__) || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__) )

  #error Old MightyCore AVR ATmega8535, ATmega16, ATmega32 not supported, try ATmega164, ATmega324, ATmega644, ATmega1284
  
#else
  #error This is designed only for MightyCore ATmega164, ATmega324, ATmega644, ATmega1284! Please check your Tools->Board setting.
#endif

#if (_PWM_LOGLEVEL_ > 2)
  #if ( defined(__AVR_ATmega164P__) || defined(__AVR_ATmega164A__) )
    #warning MightyCore ATmega164(A/P) with available Timer1-2
  #elif ( defined(__AVR_ATmega324P__) || defined(__AVR_ATmega324PA__) || defined(__AVR_ATmega324A__) || defined(__AVR_ATmega324PB__) )
    #warning MightyCore ATmega324(A/P/PA/PB) with available Timer1-2
  #elif ( defined(__AVR_ATmega324PB__) )
    #warning MightyCore ATmega324PB with available Timer1-4 
  #elif ( defined(__AVR_ATmega644__)  || defined(__AVR_ATmega644A__)  || defined(__AVR_ATmega644P__) )
    #warning MightyCore ATmega644(A/P) with available Timer1-2
  #elif TIMER_INTERRUPT_USING_ATMEGA_1284
    #warning MightyCore ATmega1284(P)  with available Timer1-3
  #else
    #warning Unknown Board
  #endif
#endif

/////////////////////////////////////////////////////////////////////

#ifndef ATMEGA_SLOW_PWM_VERSION
  #define ATMEGA_SLOW_PWM_VERSION           F("ATmega_Slow_PWM v1.0.0")
  
  #define ATMEGA_SLOW_PWM_VERSION_MAJOR     1
  #define ATMEGA_SLOW_PWM_VERSION_MINOR     0
  #define ATMEGA_SLOW_PWM_VERSION_PATCH     0

  #define ATMEGA_SLOW_PWM_VERSION_INT      1000000
#endif

#include "PWM_Generic_Debug.h"

#include <stddef.h>

#include <inttypes.h>

#if defined(ARDUINO)
  #if ARDUINO >= 100
    #include <Arduino.h>
  #else
    #include <WProgram.h>
  #endif
#endif

#define ATmega_Slow_PWM_ISR  ATmega_Slow_PWM

typedef void (*timer_callback)();
typedef void (*timer_callback_p)(void *);

#if !defined(USING_MICROS_RESOLUTION)

  #if (_PWM_LOGLEVEL_ > 3)
    #warning Not USING_MICROS_RESOLUTION, using millis resolution
  #endif
    
  #define USING_MICROS_RESOLUTION       false
#endif

#if !defined(CHANGING_PWM_END_OF_CYCLE)
  #if (_PWM_LOGLEVEL_ > 3)
    #warning Using default CHANGING_PWM_END_OF_CYCLE == true
  #endif
  
  #define CHANGING_PWM_END_OF_CYCLE     true
#endif

#define INVALID_AVR_PIN         255

//////////////////////////////////////////////////////////////////

class ATmega_Slow_PWM_ISR 
{

  public:
    // maximum number of PWM channels
#define MAX_NUMBER_CHANNELS        16

    // constructor
    ATmega_Slow_PWM_ISR();

    void init();

    // this function must be called inside loop()
    void run();
    
    //////////////////////////////////////////////////////////////////
    // PWM
    // Return the channelNum if OK, -1 if error
    int8_t setPWM(const uint32_t& pin, const float& frequency, const float& dutycycle, timer_callback StartCallback = nullptr, 
                timer_callback StopCallback = nullptr)
    {
      uint32_t period = 0;
      
      if ( ( frequency > 0.0 ) && ( frequency <= 1000.0 ) )
      {
#if USING_MICROS_RESOLUTION
      // period in us
      period = (uint32_t) (1000000.0f / frequency);
#else    
      // period in ms
      period = (uint32_t) (1000.0f / frequency);
#endif
      }
      else
      {       
        PWM_LOGERROR(F("Error: Invalid frequency, max is 1000Hz"));
        
        return -1;
      }
      
      return setupPWMChannel(pin, period, dutycycle, (void *) StartCallback, (void *) StopCallback);   
    }

    // period in us
    // Return the channelNum if OK, -1 if error
    int8_t setPWM_Period(const uint32_t& pin, const uint32_t& period, const float& dutycycle, 
                         timer_callback StartCallback = nullptr, timer_callback StopCallback = nullptr)  
    {     
      return setupPWMChannel(pin, period, dutycycle, (void *) StartCallback, (void *) StopCallback);      
    } 
    
    //////////////////////////////////////////////////////////////////
    
    // low level function to modify a PWM channel
    // returns the true on success or false on failure
    bool modifyPWMChannel(const uint8_t& channelNum, const uint32_t& pin, const float& frequency, const float& dutycycle)
    {
      uint32_t period = 0;
      
      if ( ( frequency > 0.0 ) && ( frequency <= 1000.0 ) )
      {
#if USING_MICROS_RESOLUTION
      // period in us
      period = (uint32_t) (1000000.0f / frequency);
#else    
      // period in ms
      period = (uint32_t) (1000.0f / frequency);
#endif
      }
      else
      {       
        PWM_LOGERROR(F("Error: Invalid frequency, max is 1000Hz"));
        return false;
      }
      
      return modifyPWMChannel_Period(channelNum, pin, period, dutycycle);
    }
    
    //////////////////////////////////////////////////////////////////
    
    //period in us
    bool modifyPWMChannel_Period(const uint8_t& channelNum, const uint32_t& pin, const uint32_t& period, const float& dutycycle);
    
    //////////////////////////////////////////////////////////////////

    // destroy the specified PWM channel
    void deleteChannel(const uint8_t& channelNum);

    // restart the specified PWM channel
    void restartChannel(const uint8_t& channelNum);

    // returns true if the specified PWM channel is enabled
    bool isEnabled(const uint8_t& channelNum);

    // enables the specified PWM channel
    void enable(const uint8_t& channelNum);

    // disables the specified PWM channel
    void disable(const uint8_t& channelNum);

    // enables all PWM channels
    void enableAll();

    // disables all PWM channels
    void disableAll();

    // enables the specified PWM channel if it's currently disabled, and vice-versa
    void toggle(const uint8_t& channelNum);

    // returns the number of used PWM channels
    int8_t getnumChannels();

    // returns the number of available PWM channels
    uint8_t getNumAvailablePWMChannels() 
    {
      return MAX_NUMBER_CHANNELS - numChannels;
    };

  private:

    // low level function to initialize and enable a new PWM channel
    // returns the PWM channel number (channelNum) on success or
    // -1 on failure (f == NULL) or no free PWM channels 
    int8_t setupPWMChannel(const uint32_t& pin, const uint32_t& period, const float& dutycycle, void* cbStartFunc = nullptr, void* cbStopFunc = nullptr);

    // find the first available slot
    int8_t findFirstFreeSlot();

    typedef struct 
    {
      ///////////////////////////////////
      
      uint32_t      prevTime;           // value returned by the micros() or millis() function in the previous run() call
      uint32_t      period;             // period value, in us / ms
      uint32_t      onTime;             // onTime value, ( period * dutyCycle / 100 ) us  / ms
      
      void*         callbackStart;      // pointer to the callback function when PWM pulse starts (HIGH)
      void*         callbackStop;       // pointer to the callback function when PWM pulse stops (LOW)
      
      ////////////////////////////////////////////////////////////
      
      uint32_t      pin;                // PWM pin
      bool          pinHigh;            // true if PWM pin is HIGH
      ////////////////////////////////////////////////////////////
      
      bool          enabled;            // true if enabled
      
      // New from v1.2.1
      uint32_t      newPeriod;          // period value, in us / ms
      uint32_t      newOnTime;          // onTime value, ( period * dutyCycle / 100 ) us  / ms
      float         newDutyCycle;       // from 0.00 to 100.00, float precision
      //////
    } PWM_t;

    volatile PWM_t PWM[MAX_NUMBER_CHANNELS];

    // actual number of PWM channels in use (-1 means uninitialized)
    volatile int8_t numChannels;
};

#endif    // ATmega_Slow_PWM_ISR_HPP


