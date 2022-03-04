/****************************************************************************************************************************
  RP2040_Slow_PWM.hpp
  For RP2040-based boards
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/RP2040_Slow_PWM
  Licensed under MIT license

  Now even you use all these new 16 ISR-based timers,with their maximum interval practically unlimited (limited only by
  unsigned long miliseconds), you just consume only one  RP2040-based timer and avoid conflicting with other cores' tasks.
  The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
  Therefore, their executions are not blocked by bad-behaving functions / tasks.
  This important feature is absolutely necessary for mission-critical tasks.

  Version: 1.2.1

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K.Hoang      22/09/2021 Initial coding for RP2040-based boards using RP2040 arduino-pico core
  1.0.1   K Hoang      22/10/2021 Fix platform in library.json for PIO
  1.1.0   K Hoang      10/11/2021 Add functions to modify PWM settings on-the-fly
  1.2.0   K Hoang      02/02/2022 Fix multiple-definitions linker error. Improve accuracy. Optimize code
  1.2.1   K Hoang      03/03/2022 Fix `DutyCycle` and `New Period` display bugs. Display warning only when debug level > 3
*****************************************************************************************************************************/

#pragma once

#ifndef RP2040_SLOW_PWM_HPP
#define RP2040_SLOW_PWM_HPP

#if !( ( defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_RASPBERRY_PI_PICO) || defined(ARDUINO_ADAFRUIT_FEATHER_RP2040) || defined(ARDUINO_GENERIC_RP2040) ) && !defined(ARDUINO_ARCH_MBED) )
  #error This code is intended to run on the non-mbed RP2040 arduino-pico platform! Please check your Tools->Board setting.
#endif

#ifndef RP2040_SLOW_PWM_VERSION
  #define RP2040_SLOW_PWM_VERSION           "RP2040_Slow_PWM v1.2.1"
  
  #define RP2040_SLOW_PWM_VERSION_MAJOR     1
  #define RP2040_SLOW_PWM_VERSION_MINOR     2
  #define RP2040_SLOW_PWM_VERSION_PATCH     1

  #define RP2040_SLOW_PWM_VERSION_INT       1002001
#endif

#ifndef _PWM_LOGLEVEL_
  #define _PWM_LOGLEVEL_        1
#endif

#if defined(ARDUINO_ARCH_MBED)

  #if (_PWM_LOGLEVEL_ > 3)
    #warning Using MBED RP2040 platform
  #endif
  
  #include "pico.h"
  #include "pico/time.h"
  #include "hardware/gpio.h"
  #include "hardware/uart.h"
  
  #include "hardware/timer.h"
  #include "hardware/irq.h"
#else

  #if (_PWM_LOGLEVEL_ > 3)
    #warning Using RP2040 platform
  #endif
  
  #include <stdio.h>
  #include "pico/stdlib.h"
  #include "hardware/timer.h"
  #include "hardware/irq.h"
#endif

#if defined(ARDUINO)
  #if ARDUINO >= 100
    #include <Arduino.h>
  #else
    #include <WProgram.h>
  #endif
#endif

#include "PWM_Generic_Debug.h"

/*
  To enable an alarm:
  • Enable the interrupt at the timer with a write to the appropriate alarm bit in INTE: i.e. (1 << 0) for ALARM0
  • Enable the appropriate timer interrupt at the processor (see Section 2.3.2)
  • Write the time you would like the interrupt to fire to ALARM0 (i.e. the current value in TIMERAWL plus your desired
    alarm time in microseconds). Writing the time to the ALARM register sets the ARMED bit as a side effect.
  Once the alarm has fired, the ARMED bit will be set to 0 . To clear the latched interrupt, write a 1 to the appropriate bit in
  INTR.
*/


class RP2040_TimerInterrupt;

typedef RP2040_TimerInterrupt RP2040_Timer;

// We can use many timers here
#define MAX_RP2040_NUM_TIMERS      4

typedef bool (*pico_timer_callback)  (struct repeating_timer *t);


class RP2040_TimerInterrupt
{
  private:
   
    uint8_t                 _timerNo;

    pico_timer_callback     _callback;        // pointer to the callback function
    float                   _frequency;       // Timer frequency
    uint64_t                _timerCount;        // count to activate timer, in us
      
    struct repeating_timer  _timer;

  public:

    RP2040_TimerInterrupt(uint8_t timerNo)
    {     
      _timerNo  = timerNo;
      _callback = NULL;
    };
    
    #define TIM_CLOCK_FREQ      ( (float) 1000000.0f )

    // frequency (in hertz) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    // No params and duration now. To be added in the future by adding similar functions here
    bool setFrequency(const float& frequency, pico_timer_callback callback)
    {
      if (_timerNo < MAX_RP2040_NUM_TIMERS)
      {
        if ( (frequency == 0.0f) || (frequency > 100000.0f) || (callback == NULL) )
        {
          PWM_LOGERROR(F("Error. frequency == 0, higher than 100KHz or callback == NULL "));
        
          return false;
        }
                     
        // select timer frequency is 1MHz for better accuracy. We don't use 16-bit prescaler for now.
        // Will use later if very low frequency is needed.
        _frequency  = frequency;
        _timerCount = (uint64_t) TIM_CLOCK_FREQ / frequency;
        
        PWM_LOGWARN5(F("_timerNo = "), _timerNo, F(", Clock (Hz) = "), TIM_CLOCK_FREQ, F(", _fre (Hz) = "), _frequency);
        PWM_LOGWARN3(F("_count = "), (uint32_t) (_timerCount >> 32) , F("-"), (uint32_t) (_timerCount));
        
        _callback = callback;
  
        // static bool add_repeating_timer_us(int64_t delay_us, repeating_timer_callback_t callback, void *user_data, repeating_timer_t *out);
        // static bool add_repeating_timer_ms(int64_t delay_ms, repeating_timer_callback_t callback, void *user_data, repeating_timer_t *out);
        // bool cancel_repeating_timer (repeating_timer_t *timer);
        cancel_repeating_timer(&_timer);
        add_repeating_timer_us(_timerCount, _callback, NULL, &_timer);
               
        PWM_LOGWARN1(F("add_repeating_timer_us ="), _timerCount);

        return true;
      }
      else
      {
        PWM_LOGERROR(F("Error. Timer must be 0-3"));
        
        return false;
      }
    }

    // interval (in microseconds) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    // No params and duration now. To be added in the future by adding similar functions here
    bool setInterval(const unsigned long& interval, pico_timer_callback callback)
    {
      return setFrequency((float) (1000000.0f / interval), callback);
    }

    bool attachInterrupt(const float& frequency, pico_timer_callback callback)
    {
      return setFrequency(frequency, callback);
    }

    // interval (in microseconds) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    // No params and duration now. To be added in the future by adding similar functions here
    bool attachInterruptInterval(const unsigned long& interval, pico_timer_callback callback)
    {
      return setFrequency( (float) ( 1000000.0f / interval), callback);
    }

    void detachInterrupt()
    {
      cancel_repeating_timer(&_timer);
    }

    void disableTimer()
    {
      cancel_repeating_timer(&_timer);
    }

    // Duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    void reattachInterrupt()
    {
      add_repeating_timer_us(_timerCount, _callback, NULL, &_timer);
    }

    // Duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    void enableTimer()
    {
      add_repeating_timer_us(_timerCount, _callback, NULL, &_timer);
    }

    // Just stop clock source, clear the count
    void stopTimer()
    {
      cancel_repeating_timer(&_timer);
    }

    // Just reconnect clock source, start current count from 0
    void restartTimer()
    {
      cancel_repeating_timer(&_timer);
      add_repeating_timer_us(_timerCount, _callback, NULL, &_timer);
    }

    int8_t getTimer() __attribute__((always_inline))
    {
      return _timerNo;
    };
}; // class RP2040_TimerInterrupt


#endif    // RP2040_SLOW_PWM_HPP

