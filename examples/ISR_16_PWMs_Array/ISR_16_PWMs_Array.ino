/****************************************************************************************************************************
  ISR_16_PWMs_Array.ino
  For RP2040-based boards
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/RP2040_Slow_PWM
  Licensed under MIT license

  Now even you use all these new 16 ISR-based timers,with their maximum interval practically unlimited (limited only by
  unsigned long miliseconds), you just consume only one  RP2040-based timer and avoid conflicting with other cores' tasks.
  The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
  Therefore, their executions are not blocked by bad-behaving functions / tasks.
  This important feature is absolutely necessary for mission-critical tasks.
*****************************************************************************************************************************/

#if !( ( defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_RASPBERRY_PI_PICO) || defined(ARDUINO_ADAFRUIT_FEATHER_RP2040) || defined(ARDUINO_GENERIC_RP2040) ) && !defined(ARDUINO_ARCH_MBED) )
  #error This code is intended to run on the non-mbed RP2040 arduino-pico platform! Please check your Tools->Board setting.
#endif

// These define's must be placed at the beginning before #include "ESP32_PWM.h"
// _PWM_LOGLEVEL_ from 0 to 4
// Don't define _PWM_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
#define _PWM_LOGLEVEL_                4

#define USING_MICROS_RESOLUTION       true    //false

// Default is true, uncomment to false
//#define CHANGING_PWM_END_OF_CYCLE     false

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "RP2040_Slow_PWM.h"

#define LED_OFF             HIGH
#define LED_ON              LOW

#ifndef LED_BUILTIN
  #define LED_BUILTIN       25
#endif

#ifndef LED_BLUE
  #define LED_BLUE          10
#endif

#ifndef LED_RED
  #define LED_RED           11
#endif

#define HW_TIMER_INTERVAL_US      20L

volatile uint64_t startMicros = 0;

// Init RPI_PICO_Timer
RP2040_Timer ITimer(0);

// Init RP2040_Slow_PWM, each can service 16 different ISR-based PWM channels
RP2040_Slow_PWM ISR_PWM;

//////////////////////////////////////////////////////

bool TimerHandler(struct repeating_timer *t)
{ 
  (void) t;
  
  ISR_PWM.run();

  return true;
}

//////////////////////////////////////////////////////

#define NUMBER_ISR_PWMS         16

#define PIN_D0      0
#define PIN_D1      1
#define PIN_D2      2
#define PIN_D3      3
#define PIN_D4      4
#define PIN_D5      5
#define PIN_D6      6
#define PIN_D7      7
#define PIN_D8      8
#define PIN_D9      9
#define PIN_D10     10
#define PIN_D11     11
#define PIN_D12     12

//////////////////////////////////////////////////////

#define USING_PWM_FREQUENCY     true

//////////////////////////////////////////////////////

// You can assign pins here. Be carefull to select good pin to use or crash, e.g pin 6-11
uint32_t PWM_Pin[] =
{
   LED_BUILTIN, LED_BLUE, LED_RED, PIN_D0, PIN_D1,  PIN_D2,  PIN_D3,  PIN_D4,
        PIN_D5,   PIN_D6,  PIN_D7, PIN_D8, PIN_D9, PIN_D10, PIN_D11, PIN_D12
};

// You can assign any interval for any timer here, in microseconds
uint32_t PWM_Period[] =
{
  1000000L,   500000L,   333333L,   250000L,   200000L,   166667L,   142857L,   125000L,
   111111L,   100000L,    66667L,    50000L,    40000L,   33333L,     25000L,    20000L
};

// You can assign any interval for any timer here, in Hz
float PWM_Freq[] =
{
  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f,
  9.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 40.0f, 50.0f
};

// You can assign any interval for any timer here, in milliseconds
float PWM_DutyCycle[] =
{
   5.0, 10.0, 20.0, 30.0, 40.0, 45.0, 50.0, 55.0,
  60.0, 65.0, 70.0, 75.0, 80.0, 85.0, 90.0, 95.0
};

typedef void (*irqCallback)  ();


// In Portenta_H7, avoid doing something fancy in ISR, for example complex Serial.print with String() argument
// The pure simple Serial.prints here are just for demonstration and testing. Must be eliminate in working environment
// Or you can get this run-time error / crash
void doingSomething0()
{
}

void doingSomething1()
{
}

void doingSomething2()
{
}

void doingSomething3()
{
}

void doingSomething4()
{
}

void doingSomething5()
{
}

void doingSomething6()
{
}

void doingSomething7()
{
}

void doingSomething8()
{
}

void doingSomething9()
{
}

void doingSomething10()
{
}

void doingSomething11()
{
}

void doingSomething12()
{
}

void doingSomething13()
{
}

void doingSomething14()
{
}

void doingSomething15()
{
}

irqCallback irqCallbackStartFunc[] =
{
  doingSomething0,  doingSomething1,  doingSomething2,  doingSomething3, 
  doingSomething4,  doingSomething5,  doingSomething6,  doingSomething7, 
  doingSomething8,  doingSomething9,  doingSomething10, doingSomething11,
  doingSomething12, doingSomething13, doingSomething14, doingSomething15
};

////////////////////////////////////////////////

void setup()
{ 
  Serial.begin(115200);
  while (!Serial);

  delay(2000);

  Serial.print(F("\nStarting ISR_16_PWMs_Array on ")); Serial.println(BOARD_NAME);
  Serial.println(RP2040_SLOW_PWM_VERSION);

  // Interval in microsecs
  if (ITimer.attachInterruptInterval(HW_TIMER_INTERVAL_US, TimerHandler))
  {
    startMicros = micros();
    Serial.print(F("Starting ITimer OK, micros() = ")); Serial.println(startMicros);
  }
  else
    Serial.println(F("Can't set ITimer. Select another freq. or timer"));

#if 1
  // Just to demonstrate, don't use too many ISR Timers if not absolutely necessary
  // You can use up to 16 timer for each ISR_PWM
  for (uint16_t i = 0; i < NUMBER_ISR_PWMS; i++)
  {
    //void setPWM(uint32_t pin, float frequency, float dutycycle
    // , timer_callback_p StartCallback = nullptr, timer_callback_p StopCallback = nullptr)

#if USING_PWM_FREQUENCY

    // You can use this with PWM_Freq in Hz
    ISR_PWM.setPWM(PWM_Pin[i], PWM_Freq[i], PWM_DutyCycle[i], irqCallbackStartFunc[i]);

#else
  #if USING_MICROS_RESOLUTION
    // Or using period in microsecs resolution
    ISR_PWM.setPWM_Period(PWM_Pin[i], PWM_Period[i], PWM_DutyCycle[i], irqCallbackStartFunc[i]);
  #else
    // Or using period in millisecs resolution
    ISR_PWM.setPWM_Period(PWM_Pin[i], PWM_Period[i] / 1000, PWM_DutyCycle[i], irqCallbackStartFunc[i]);
  #endif
#endif
  }
#endif  
}

void loop()
{
}
