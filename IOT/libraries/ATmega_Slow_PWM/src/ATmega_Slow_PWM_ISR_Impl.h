/****************************************************************************************************************************
  ATmega_Slow_PWM_ISR_Impl.h
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

#ifndef ATmega_Slow_PWM_ISR_IMPL_H
#define ATmega_Slow_PWM_ISR_IMPL_H

#include <string.h>

/////////////////////////////////////////////////// 


uint32_t timeNow()
{
#if USING_MICROS_RESOLUTION  
  return ( (uint32_t) micros() );
#else
  return ( (uint32_t) millis() );
#endif    
}
 
/////////////////////////////////////////////////// 

ATmega_Slow_PWM_ISR::ATmega_Slow_PWM_ISR()
  : numChannels (-1)
{
}

///////////////////////////////////////////////////

void ATmega_Slow_PWM_ISR::init() 
{
  uint32_t currentTime = timeNow();
   
  for (uint8_t channelNum = 0; channelNum < MAX_NUMBER_CHANNELS; channelNum++) 
  {
    memset((void*) &PWM[channelNum], 0, sizeof (PWM_t));
    PWM[channelNum].prevTime = currentTime;
    PWM[channelNum].pin      = INVALID_AVR_PIN;
  }
  
  numChannels = 0;
}

///////////////////////////////////////////////////

void ATmega_Slow_PWM_ISR::run() 
{    
  //uint32_t currentTime = timeNow();
  uint32_t currentTime;

  for (uint8_t channelNum = 0; channelNum < MAX_NUMBER_CHANNELS; channelNum++) 
  {
    currentTime = timeNow();
    
    // If enabled => check
    // start period / dutyCycle => digitalWrite HIGH
    // end dutyCycle =>  digitalWrite LOW
    if (PWM[channelNum].enabled) 
    {
      if ( (uint32_t) (currentTime - PWM[channelNum].prevTime) <= PWM[channelNum].onTime ) 
      {              
        if (!PWM[channelNum].pinHigh)
        {
          digitalWrite(PWM[channelNum].pin, HIGH);
          PWM[channelNum].pinHigh = true;
          
          // callbackStart
          if (PWM[channelNum].callbackStart != nullptr)
          {
            (*(timer_callback) PWM[channelNum].callbackStart)();
          }
        }
      }
      else if ( (uint32_t) (currentTime - PWM[channelNum].prevTime) < PWM[channelNum].period ) 
      {
        if (PWM[channelNum].pinHigh)
        {
          digitalWrite(PWM[channelNum].pin, LOW);
          PWM[channelNum].pinHigh = false;
          
          // callback when PWM pulse stops (LOW)
          if (PWM[channelNum].callbackStop != nullptr)
          {
            (*(timer_callback) PWM[channelNum].callbackStop)();
          }
        }
      }
      //else 
      else if ( (uint32_t) (currentTime - PWM[channelNum].prevTime) >= PWM[channelNum].period )   
      {
        PWM[channelNum].prevTime = currentTime;
        
#if CHANGING_PWM_END_OF_CYCLE
        // Only update whenever having newPeriod
        if (PWM[channelNum].newPeriod != 0)
        {
          PWM[channelNum].period    = PWM[channelNum].newPeriod;
          PWM[channelNum].newPeriod = 0;
          
          PWM[channelNum].onTime  = PWM[channelNum].newOnTime;
        }
#endif
      }     
    }
  }
}


///////////////////////////////////////////////////

// find the first available slot
// return -1 if none found
int8_t ATmega_Slow_PWM_ISR::findFirstFreeSlot() 
{
  // all slots are used
  if (numChannels >= MAX_NUMBER_CHANNELS) 
  {
    return -1;
  }

  // return the first slot with no callbackStart (i.e. free)
  for (uint8_t channelNum = 0; channelNum < MAX_NUMBER_CHANNELS; channelNum++) 
  {
    if ( (PWM[channelNum].period == 0) && !PWM[channelNum].enabled )
    {
      return channelNum;
    }
  }

  // no free slots found
  return -1;
}

///////////////////////////////////////////////////

int8_t ATmega_Slow_PWM_ISR::setupPWMChannel(const uint32_t& pin, const uint32_t& period, const float& dutycycle, void* cbStartFunc, void* cbStopFunc)
{
  int channelNum;
  
  // Invalid input, such as period = 0, etc
  if ( (period == 0) || (dutycycle < 0.0) || (dutycycle > 100.0) )
  {
    PWM_LOGERROR(F("Error: Invalid period or dutycycle"));
    return -1;
  }

  if (numChannels < 0) 
  {
    init();
  }
 
  channelNum = findFirstFreeSlot();
  
  if (channelNum < 0) 
  {
    return -1;
  }

  PWM[channelNum].pin           = pin;
  PWM[channelNum].period        = period;
  
  // Must be 0 for new PWM channel
  PWM[channelNum].newPeriod     = 0;
  
  PWM[channelNum].onTime        = ( period * dutycycle ) / 100;
  
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  PWM[channelNum].pinHigh       = true;
  
  PWM[channelNum].prevTime      = timeNow();
  
  PWM[channelNum].callbackStart = cbStartFunc;
  PWM[channelNum].callbackStop  = cbStopFunc;
  
  PWM_LOGINFO0(F("Channel : "));      PWM_LOGINFO0(channelNum); 
  PWM_LOGINFO0(F("\t    Period : ")); PWM_LOGINFO0(PWM[channelNum].period);
  PWM_LOGINFO0(F("\t\tOnTime : "));   PWM_LOGINFO0(PWM[channelNum].onTime); 
  PWM_LOGINFO0(F("\tStart_Time : ")); PWM_LOGINFOLN0(PWM[channelNum].prevTime);
 
  numChannels++;
  
  PWM[channelNum].enabled      = true;
  
  return channelNum;
}

///////////////////////////////////////////////////

bool ATmega_Slow_PWM_ISR::modifyPWMChannel_Period(const uint8_t& channelNum, const uint32_t& pin, const uint32_t& period, const float& dutycycle)
{
  // Invalid input, such as period = 0, etc
  if ( (period == 0) || (dutycycle < 0.0) || (dutycycle > 100.0) )
  {
    PWM_LOGERROR(F("Error: Invalid period or dutycycle"));
    return false;
  }

  if (channelNum > MAX_NUMBER_CHANNELS) 
  {
    PWM_LOGERROR(F("Error: channelNum > MAX_NUMBER_CHANNELS"));
    return false;
  }
  
  if (PWM[channelNum].pin != pin) 
  {
    PWM_LOGERROR(F("Error: channelNum and pin mismatched"));
    return false;
  }
   
#if CHANGING_PWM_END_OF_CYCLE

  PWM[channelNum].newPeriod     = period;
  PWM[channelNum].newDutyCycle  = dutycycle;
  PWM[channelNum].newOnTime     = ( period * dutycycle ) / 100;
  
  PWM_LOGINFO0(F("Channel : "));      PWM_LOGINFO0(channelNum); 
  PWM_LOGINFO0(F("\t    Period : ")); PWM_LOGINFO0(period);
  PWM_LOGINFO0(F("\t\tOnTime : "));   PWM_LOGINFO0(PWM[channelNum].newOnTime); 
  PWM_LOGINFO0(F("\tStart_Time : ")); PWM_LOGINFOLN0(PWM[channelNum].prevTime);
  
#else

  PWM[channelNum].period        = period;        

  PWM[channelNum].onTime        = ( period * dutycycle ) / 100;
  
  digitalWrite(pin, HIGH);
  PWM[channelNum].pinHigh       = true;
  
  PWM[channelNum].prevTime      = timeNow();
   
  PWM_LOGINFO0(F("Channel : "));      PWM_LOGINFO0(channelNum); 
  PWM_LOGINFO0(F("\t    Period : ")); PWM_LOGINFO0(PWM[channelNum].period);
  PWM_LOGINFO0(F("\t\tOnTime : "));   PWM_LOGINFO0(PWM[channelNum].onTime); 
  PWM_LOGINFO0(F("\tStart_Time : ")); PWM_LOGINFOLN0(PWM[channelNum].prevTime);
  
#endif
  
  return true;
}


///////////////////////////////////////////////////

void ATmega_Slow_PWM_ISR::deleteChannel(const uint8_t& channelNum) 
{
  // nothing to delete if no timers are in use
  if ( (channelNum >= MAX_NUMBER_CHANNELS) || (numChannels == 0) )
  {
    return;
  }

  // don't decrease the number of timers if the specified slot is already empty (zero period, invalid)
  if ( (PWM[channelNum].pin != INVALID_AVR_PIN) && (PWM[channelNum].period != 0) )
  {
    memset((void*) &PWM[channelNum], 0, sizeof (PWM_t));
    
    PWM[channelNum].pin = INVALID_AVR_PIN;
    
    // update number of timers
    numChannels--;
  }
}

///////////////////////////////////////////////////

void ATmega_Slow_PWM_ISR::restartChannel(const uint8_t& channelNum) 
{
  if (channelNum >= MAX_NUMBER_CHANNELS) 
  {
    return;
  }

  PWM[channelNum].prevTime = timeNow();
}

///////////////////////////////////////////////////

bool ATmega_Slow_PWM_ISR::isEnabled(const uint8_t& channelNum) 
{
  if (channelNum >= MAX_NUMBER_CHANNELS) 
  {
    return false;
  }

  return PWM[channelNum].enabled;
}

///////////////////////////////////////////////////

void ATmega_Slow_PWM_ISR::enable(const uint8_t& channelNum) 
{
  if (channelNum >= MAX_NUMBER_CHANNELS) 
  {
    return;
  }

  PWM[channelNum].enabled = true;
}

///////////////////////////////////////////////////

void ATmega_Slow_PWM_ISR::disable(const uint8_t& channelNum) 
{
  if (channelNum >= MAX_NUMBER_CHANNELS) 
  {
    return;
  }

  PWM[channelNum].enabled = false;
}

///////////////////////////////////////////////////

void ATmega_Slow_PWM_ISR::enableAll() 
{
  // Enable all timers with a callbackStart assigned (used)

  for (uint8_t channelNum = 0; channelNum < MAX_NUMBER_CHANNELS; channelNum++) 
  {
    if (PWM[channelNum].period != 0)
    {
      PWM[channelNum].enabled = true;
    }
  }
}

///////////////////////////////////////////////////

void ATmega_Slow_PWM_ISR::disableAll() 
{
  // Disable all timers with a callbackStart assigned (used)
  for (uint8_t channelNum = 0; channelNum < MAX_NUMBER_CHANNELS; channelNum++) 
  {
    if (PWM[channelNum].period != 0)
    {
      PWM[channelNum].enabled = false;
    }
  }
}

///////////////////////////////////////////////////

void ATmega_Slow_PWM_ISR::toggle(const uint8_t& channelNum) 
{
  if (channelNum >= MAX_NUMBER_CHANNELS) 
  {
    return;
  }

  PWM[channelNum].enabled = !PWM[channelNum].enabled;
}

///////////////////////////////////////////////////

int8_t ATmega_Slow_PWM_ISR::getnumChannels() 
{
  return numChannels;
}

#endif    // ATmega_Slow_PWM_ISR_IMPL_H
