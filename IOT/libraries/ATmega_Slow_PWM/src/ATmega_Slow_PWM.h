/****************************************************************************************************************************
  ATmega_Slow_PWM.h
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

#ifndef ATmega_Slow_PWM_H
#define ATmega_Slow_PWM_H

#include "ATmega_Slow_PWM.hpp"
#include "ATmega_Slow_PWM_Impl.h"

#include "ATmega_Slow_PWM_ISR.h"


#endif    // ATmega_Slow_PWM_H

