# RP2040_Slow_PWM Library

[![arduino-library-badge](https://www.ardu-badge.com/badge/RP2040_Slow_PWM.svg?)](https://www.ardu-badge.com/RP2040_Slow_PWM)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/RP2040_Slow_PWM.svg)](https://github.com/khoih-prog/RP2040_Slow_PWM/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/RP2040_Slow_PWM/blob/main/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/RP2040_Slow_PWM.svg)](http://github.com/khoih-prog/RP2040_Slow_PWM/issues)

<a href="https://www.buymeacoffee.com/khoihprog6" title="Donate to my libraries using BuyMeACoffee"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" alt="Donate to my libraries using BuyMeACoffee" style="height: 50px !important;width: 181px !important;" ></a>
<a href="https://www.buymeacoffee.com/khoihprog6" title="Donate to my libraries using BuyMeACoffee"><img src="https://img.shields.io/badge/buy%20me%20a%20coffee-donate-orange.svg?logo=buy-me-a-coffee&logoColor=FFDD00" style="height: 20px !important;width: 200px !important;" ></a>


---
---

## Table of Contents

* [Important Change from v1.2.0](#Important-Change-from-v120)
* [Why do we need this RP2040_Slow_PWM library](#why-do-we-need-this-RP2040_Slow_PWM-library)
  * [Features](#features)
  * [Why using ISR-based PWM is better](#why-using-isr-based-pwm-is-better)
  * [Currently supported Boards](#currently-supported-boards)
  * [Important Notes about ISR](#important-notes-about-isr)
* [Changelog](changelog.md)
* [Prerequisites](#prerequisites)
* [Installation](#installation)
  * [Use Arduino Library Manager](#use-arduino-library-manager)
  * [Manual Install](#manual-install)
  * [VS Code & PlatformIO](#vs-code--platformio)
* [HOWTO Fix `Multiple Definitions` Linker Error](#howto-fix-multiple-definitions-linker-error)
* [More useful Information](#more-useful-information)
* [Usage](#usage)
  * [1. Init Hardware Timer](#1-init-hardware-timer)
  * [2. Set PWM Frequency, dutycycle, attach irqCallbackStartFunc and irqCallbackStopFunc functions](#2-Set-PWM-Frequency-dutycycle-attach-irqCallbackStartFunc-and-irqCallbackStopFunc-functions)
* [Examples](#examples)
  * [ 1. ISR_16_PWMs_Array](examples/ISR_16_PWMs_Array)
  * [ 2. ISR_16_PWMs_Array_Complex](examples/ISR_16_PWMs_Array_Complex)
  * [ 3. ISR_16_PWMs_Array_Simple](examples/ISR_16_PWMs_Array_Simple)
  * [ 4. ISR_Changing_PWM](examples/ISR_Changing_PWM)
  * [ 5. ISR_Modify_PWM](examples/ISR_Modify_PWM)
  * [ 6. multiFileProject](examples/multiFileProject). **New**
* [Example ISR_16_PWMs_Array_Complex](#Example-ISR_16_PWMs_Array_Complex)
* [Debug Terminal Output Samples](#debug-terminal-output-samples)
  * [1. ISR_16_PWMs_Array_Complex on RASPBERRY_PI_PICO](#1-ISR_16_PWMs_Array_Complex-on-RASPBERRY_PI_PICO)
  * [2. ISR_16_PWMs_Array on RASPBERRY_PI_PICO](#2-ISR_16_PWMs_Array-on-RASPBERRY_PI_PICO)
  * [3. ISR_16_PWMs_Array_Simple on RASPBERRY_PI_PICO](#3-ISR_16_PWMs_Array_Simple-on-RASPBERRY_PI_PICO)
  * [4. ISR_Modify_PWM on RASPBERRY_PI_PICO](#4-ISR_Modify_PWM-on-RASPBERRY_PI_PICO)
  * [5. ISR_Changing_PWM on RASPBERRY_PI_PICO](#5-ISR_Changing_PWM-on-RASPBERRY_PI_PICO)
* [Debug](#debug)
* [Troubleshooting](#troubleshooting)
* [Issues](#issues)
* [TO DO](#to-do)
* [DONE](#done)
* [Contributions and Thanks](#contributions-and-thanks)
* [Contributing](#contributing)
* [License](#license)
* [Copyright](#copyright)

---
---

### Important Change from v1.2.0

Please have a look at [HOWTO Fix `Multiple Definitions` Linker Error](#howto-fix-multiple-definitions-linker-error)

As more complex calculation and check **inside ISR** are introduced from v1.2.0, there is possibly some crash depending on use-case.

You can modify to use larger `HW_TIMER_INTERVAL_US`, (from current 20uS), according to your board and use-case if crash happens.


```
// Current 20uS
#define HW_TIMER_INTERVAL_US      20L
```

---
---

### Why do we need this [RP2040_Slow_PWM library](https://github.com/khoih-prog/RP2040_Slow_PWM)

### Features

This library enables you to use Hardware Timers on RP2040-based boards to create and output PWM to pins. Because this library doesn't use the powerful hardware-controlled PWM with limitations, the maximum PWM frequency is currently limited at **1000Hz**, which is still suitable for many real-life applications.

---

This library enables you to use Interrupt from Hardware Timers on RP2040-based boards to create and output PWM to pins. It now supports 16 ISR-based synchronized PWM channels, while consuming only 1 Hardware Timer. PWM interval can be very long (uint32_t millisecs). The most important feature is they're ISR-based PWM channels. Therefore, their executions are not blocked by bad-behaving functions or tasks. This important feature is absolutely necessary for mission-critical tasks. These hardware PWM channels, using interrupt, still work even if other functions are blocking. Moreover, they are much more precise (certainly depending on clock frequency accuracy) than other software PWM using millis() or micros(). That's necessary if you need to measure some data requiring better accuracy.

As **Hardware Timers are rare, and very precious assets** of any board, this library now enables you to use up to **16 ISR-based synchronized PWM channels, while consuming only 1 Hardware Timer**. Timers' interval is very long (**ulong millisecs**).

Now with these new **16 ISR-based PWM-channels**, the maximum interval is **practically unlimited** (limited only by unsigned long miliseconds) while **the accuracy is nearly perfect** compared to software PWM channels. 

The most important feature is they're ISR-based PWM channels. Therefore, their executions are **not blocked by bad-behaving functions / tasks**. This important feature is absolutely necessary for mission-critical tasks. 

The [**ISR_16_PWMs_Array_Complex**](examples/ISR_16_PWMs_Array_Complex) example will demonstrate the nearly perfect accuracy, compared to software PWM, by printing the actual period / duty-cycle in `microsecs` of each of PWM-channels.

Being ISR-based PWM, their executions are not blocked by bad-behaving functions / tasks, such as connecting to WiFi, Internet or Blynk services. You can also have many `(up to 16)` PWM channels to use.

This non-being-blocked important feature is absolutely necessary for mission-critical tasks.

You'll see `software-based` SimpleTimer is blocked while system is connecting to WiFi / Internet / Blynk, as well as by blocking task 
in loop(), using delay() function as an example. The elapsed time then is very unaccurate

---

#### Why using ISR-based PWM is better

Imagine you have a system with a **mission-critical** function, measuring water level and control the sump pump or doing something much more important. You normally use a software timer to poll, or even place the function in loop(). But what if another function is **blocking** the loop() or setup().

So your function **might not be executed, and the result would be disastrous.**

You'd prefer to have your function called, no matter what happening with other functions (busy loop, bug, etc.).

The correct choice is to use a Hardware Timer with **Interrupt** to call your function.

These hardware PWM channels, using interrupt, still work even if other functions are blocking. Moreover, they are much more **precise** (certainly depending on clock frequency accuracy) than other software PWM channels using millis() or micros(). That's necessary if you need to measure some data requiring better accuracy.

Functions using normal software PWM channels, relying on loop() and calling millis(), won't work if the loop() or setup() is blocked by certain operation. For example, certain function is blocking while it's connecting to WiFi or some services.

The catch is **your function is now part of an ISR (Interrupt Service Routine), and must be lean / mean, and follow certain rules.** More to read on:

[**HOWTO Attach Interrupt**](https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/)

---

### Currently supported Boards

1. **RP2040-based boards** such as ADAFRUIT_FEATHER_RP2040, RASPBERRY_PI_PICO, etc., using [**arduino-pico core**](https://github.com/earlephilhower/arduino-pico)

---

#### Important Notes about ISR

1. Inside the attached function, **delay() won’t work and the value returned by millis() will not increment.** Serial data received while in the function may be lost. You should declare as **volatile any variables that you modify within the attached function.**

2. Typically global variables are used to pass data between an ISR and the main program. To make sure variables shared between an ISR and the main program are updated correctly, declare them as volatile.

---
---

## Prerequisites

 1. [`Arduino IDE 1.8.19+` for Arduino](https://github.com/arduino/Arduino). [![GitHub release](https://img.shields.io/github/release/arduino/Arduino.svg)](https://github.com/arduino/Arduino/releases/latest)
 2. [**Earle Philhower's arduino-pico core v1.12.0+**](https://github.com/earlephilhower/arduino-pico) for RP2040-based boards such as **RASPBERRY_PI_PICO, ADAFRUIT_FEATHER_RP2040 and GENERIC_RP2040**, etc. [![GitHub release](https://img.shields.io/github/release/earlephilhower/arduino-pico.svg)](https://github.com/earlephilhower/arduino-pico/releases/latest)

 3. To use with certain example
   - [`SimpleTimer library`](https://github.com/jfturcot/SimpleTimer) to use with some examples.
   
   
---
---

## Installation

### Use Arduino Library Manager

The best and easiest way is to use `Arduino Library Manager`. Search for [**RP2040_Slow_PWM**](https://github.com/khoih-prog/RP2040_Slow_PWM), then select / install the latest version.
You can also use this link [![arduino-library-badge](https://www.ardu-badge.com/badge/RP2040_Slow_PWM.svg?)](https://www.ardu-badge.com/RP2040_Slow_PWM) for more detailed instructions.

### Manual Install

Another way to install is to:

1. Navigate to [**RP2040_Slow_PWM**](https://github.com/khoih-prog/RP2040_Slow_PWM) page.
2. Download the latest release `RP2040_Slow_PWM-master.zip`.
3. Extract the zip file to `RP2040_Slow_PWM-master` directory 
4. Copy whole `RP2040_Slow_PWM-master` folder to Arduino libraries' directory such as `~/Arduino/libraries/`.

### VS Code & PlatformIO

1. Install [VS Code](https://code.visualstudio.com/)
2. Install [PlatformIO](https://platformio.org/platformio-ide)
3. Install [**RP2040_Slow_PWM** library](https://registry.platformio.org/libraries/khoih-prog/RP2040_Slow_PWM) by using [Library Manager](https://registry.platformio.org/libraries/khoih-prog/RP2040_Slow_PWM/installation). Search for **RP2040_Slow_PWM** in [Platform.io Author's Libraries](https://platformio.org/lib/search?query=author:%22Khoi%20Hoang%22)
4. Use included [platformio.ini](platformio/platformio.ini) file from examples to ensure that all dependent libraries will installed automatically. Please visit documentation for the other options and examples at [Project Configuration File](https://docs.platformio.org/page/projectconf.html)


---
---

### HOWTO Fix `Multiple Definitions` Linker Error

The current library implementation, using `xyz-Impl.h` instead of standard `xyz.cpp`, possibly creates certain `Multiple Definitions` Linker error in certain use cases.

You can include this `.hpp` file

```
// Can be included as many times as necessary, without `Multiple Definitions` Linker Error
#include "RP2040_Slow_PWM.hpp"     //https://github.com/khoih-prog/RP2040_Slow_PWM
```

in many files. But be sure to use the following `.h` file **in just 1 `.h`, `.cpp` or `.ino` file**, which must **not be included in any other file**, to avoid `Multiple Definitions` Linker Error

```
// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "RP2040_Slow_PWM.h"           //https://github.com/khoih-prog/RP2040_Slow_PWM
```

Check the new [**multiFileProject** example](examples/multiFileProject) for a `HOWTO` demo.

Have a look at the discussion in [Different behaviour using the src_cpp or src_h lib #80](https://github.com/khoih-prog/ESPAsync_WiFiManager/discussions/80)

---
---

## More useful Information

The RP2040-based system timer peripheral provides a `global microsecond timebase` for the system, and generates interrupts based on this timebase. It supports the following features:

  • A single 64-bit counter, incrementing once per microsecond
  • This counter can be read from a pair of latching registers, for race-free reads over a 32-bit bus.
  • Four alarms: match on the lower 32 bits of counter, IRQ on match: **TIMER_IRQ_0-TIMER_IRQ_3**

---

Now with these new `16 ISR-based PWM channels` (while consuming only **1 hardware timer**), the maximum interval is practically unlimited (limited only by unsigned long miliseconds). The accuracy is nearly perfect compared to software PWM channels. The most important feature is they're ISR-based PWM channels Therefore, their executions are not blocked by bad-behaving functions / tasks.
This important feature is absolutely necessary for mission-critical tasks. 

The [ISR_16_PWMs_Array_Complex](examples/ISR_16_PWMs_Array_Complex) example will demonstrate the nearly perfect accuracy compared to software PWM channels by printing the actual elapsed microsecs / millisecs of each type of PWM channels.

Being ISR-based PWM channels, their executions are not blocked by bad-behaving functions / tasks, such as connecting to WiFi, Internet and Blynk services. You can also have many `(up to 16)` PWM channels to use.
This non-being-blocked important feature is absolutely necessary for mission-critical tasks. 
You'll see blynkTimer Software is blocked while system is connecting to WiFi / Internet / Blynk, as well as by blocking task in loop(), using delay() function as an example. The elapsed time then is very unaccurate


---
---

## Usage

Before using any Timer, you have to make sure the Timer has not been used by any other purpose.
`TIMER_IRQ_0, TIMER_IRQ_1, TIMER_IRQ_2 and TIMER_IRQ_3` are supported for RP2040-based boards.

#### 1. Init Hardware Timer

```
// Init RPI_PICO_Timer
RP2040_Timer ITimer(0);

// Init RP2040_Slow_PWM, each can service 16 different ISR-based PWM channels
RP2040_Slow_PWM ISR_PWM;
```

#### 2. Set PWM Frequency, dutycycle, attach irqCallbackStartFunc and irqCallbackStopFunc functions

```
void irqCallbackStartFunc()
{

}

void irqCallbackStopFunc()
{

}

void setup()
{
  ....
  
  // You can use this with PWM_Freq in Hz
  ISR_PWM.setPWM(PWM_Pin, PWM_Freq, PWM_DutyCycle, irqCallbackStartFunc, irqCallbackStopFunc);
                   
  ....                 
}  
```

---
---

### Examples: 

 1. [ISR_16_PWMs_Array](examples/ISR_16_PWMs_Array)
 2. [ISR_16_PWMs_Array_Complex](examples/ISR_16_PWMs_Array_Complex)
 3. [ISR_16_PWMs_Array_Simple](examples/ISR_16_PWMs_Array_Simple)
 4. [ISR_Changing_PWM](examples/ISR_Changing_PWM)
 5. [ISR_Modify_PWM](examples/ISR_Modify_PWM)
 6. [**multiFileProject**](examples/multiFileProject) **New** 

 
---
---

### Example [ISR_16_PWMs_Array_Complex](examples/ISR_16_PWMs_Array_Complex)

https://github.com/khoih-prog/RP2040_Slow_PWM/blob/1c170849098772e46c9dcd2b3e43af0816dc83e7/examples/ISR_16_PWMs_Array_Complex/ISR_16_PWMs_Array_Complex.ino#L16-L581


---
---

### Debug Terminal Output Samples

### 1. ISR_16_PWMs_Array_Complex on RASPBERRY_PI_PICO

The following is the sample terminal output when running example [ISR_16_PWMs_Array_Complex](examples/ISR_16_PWMs_Array_Complex) to demonstrate how to use multiple PWM channels with complex callback functions, the accuracy of ISR Hardware PWM-channels, **especially when system is very busy**.  The ISR PWM-channels is **running exactly according to corresponding programmed periods and duty-cycles**


```
Starting ISR_16_PWMs_Array_Complex on RASPBERRY_PI_PICO
RP2040_Slow_PWM v1.2.1
[PWM] _timerNo =  0 , Clock (Hz) =  1000000.00 , _fre (Hz) =  50000.00
[PWM] _count =  0 - 20
[PWM] add_repeating_timer_us = 20
Starting ITimer OK, micros() = 2785656
Channel : 0	    Period : 1000000		OnTime : 50000	Start_Time : 2786499
Channel : 1	    Period : 500000		OnTime : 50000	Start_Time : 2787719
Channel : 2	    Period : 333333		OnTime : 66666	Start_Time : 2788751
Channel : 3	    Period : 250000		OnTime : 75000	Start_Time : 2789774
Channel : 4	    Period : 200000		OnTime : 80000	Start_Time : 2790825
Channel : 5	    Period : 166666		OnTime : 74999	Start_Time : 2791865
Channel : 6	    Period : 142857		OnTime : 71428	Start_Time : 2792858
Channel : 7	    Period : 125000		OnTime : 68750	Start_Time : 2793852
Channel : 8	    Period : 111111		OnTime : 66666	Start_Time : 2794834
Channel : 9	    Period : 100000		OnTime : 65000	Start_Time : 2795930
Channel : 10	    Period : 66666		OnTime : 46666	Start_Time : 2796986
Channel : 11	    Period : 50000		OnTime : 37500	Start_Time : 2798025
Channel : 12	    Period : 40000		OnTime : 32000	Start_Time : 2799056
Channel : 13	    Period : 33333		OnTime : 28333	Start_Time : 2800049
Channel : 14	    Period : 25000		OnTime : 22500	Start_Time : 2801121
Channel : 15	    Period : 20000		OnTime : 19000	Start_Time : 2802222
SimpleTimer (ms): 2000, us : 12803462, Dus : 10017146
PWM Channel : 0, programmed Period (us): 1000000.00, actual : 1000006, programmed DutyCycle : 5.00, actual : 5.00
PWM Channel : 1, programmed Period (us): 500000.00, actual : 500019, programmed DutyCycle : 10.00, actual : 10.00
PWM Channel : 2, programmed Period (us): 333333.34, actual : 333336, programmed DutyCycle : 20.00, actual : 20.00
PWM Channel : 3, programmed Period (us): 250000.00, actual : 250015, programmed DutyCycle : 30.00, actual : 30.00
PWM Channel : 4, programmed Period (us): 200000.00, actual : 200010, programmed DutyCycle : 40.00, actual : 39.99
PWM Channel : 5, programmed Period (us): 166666.67, actual : 166669, programmed DutyCycle : 45.00, actual : 45.00
PWM Channel : 6, programmed Period (us): 142857.14, actual : 142880, programmed DutyCycle : 50.00, actual : 49.99
PWM Channel : 7, programmed Period (us): 125000.00, actual : 125018, programmed DutyCycle : 55.00, actual : 54.99
PWM Channel : 8, programmed Period (us): 111111.11, actual : 111117, programmed DutyCycle : 60.00, actual : 60.00
PWM Channel : 9, programmed Period (us): 100000.00, actual : 100016, programmed DutyCycle : 65.00, actual : 64.98
PWM Channel : 10, programmed Period (us): 66666.66, actual : 66680, programmed DutyCycle : 70.00, actual : 69.96
PWM Channel : 11, programmed Period (us): 50000.00, actual : 50015, programmed DutyCycle : 75.00, actual : 74.94
PWM Channel : 12, programmed Period (us): 40000.00, actual : 40016, programmed DutyCycle : 80.00, actual : 79.96
PWM Channel : 13, programmed Period (us): 33333.33, actual : 33348, programmed DutyCycle : 85.00, actual : 84.90
PWM Channel : 14, programmed Period (us): 25000.00, actual : 25018, programmed DutyCycle : 90.00, actual : 89.88
PWM Channel : 15, programmed Period (us): 20000.00, actual : 20022, programmed DutyCycle : 95.00, actual : 94.80
SimpleTimer (ms): 2000, us : 22835351, Dus : 10031889
PWM Channel : 0, programmed Period (us): 1000000.00, actual : 1000010, programmed DutyCycle : 5.00, actual : 5.00
PWM Channel : 1, programmed Period (us): 500000.00, actual : 500017, programmed DutyCycle : 10.00, actual : 10.00
PWM Channel : 2, programmed Period (us): 333333.34, actual : 333346, programmed DutyCycle : 20.00, actual : 20.00
PWM Channel : 3, programmed Period (us): 250000.00, actual : 250021, programmed DutyCycle : 30.00, actual : 30.00
PWM Channel : 4, programmed Period (us): 200000.00, actual : 200016, programmed DutyCycle : 40.00, actual : 39.99
PWM Channel : 5, programmed Period (us): 166666.67, actual : 166673, programmed DutyCycle : 45.00, actual : 44.98
PWM Channel : 6, programmed Period (us): 142857.14, actual : 142882, programmed DutyCycle : 50.00, actual : 49.99
PWM Channel : 7, programmed Period (us): 125000.00, actual : 125019, programmed DutyCycle : 55.00, actual : 54.99
PWM Channel : 8, programmed Period (us): 111111.11, actual : 111130, programmed DutyCycle : 60.00, actual : 59.97
PWM Channel : 9, programmed Period (us): 100000.00, actual : 100022, programmed DutyCycle : 65.00, actual : 64.97
PWM Channel : 10, programmed Period (us): 66666.66, actual : 66674, programmed DutyCycle : 70.00, actual : 69.97
PWM Channel : 11, programmed Period (us): 50000.00, actual : 50025, programmed DutyCycle : 75.00, actual : 74.95
PWM Channel : 12, programmed Period (us): 40000.00, actual : 40021, programmed DutyCycle : 80.00, actual : 79.93
PWM Channel : 13, programmed Period (us): 33333.33, actual : 33353, programmed DutyCycle : 85.00, actual : 84.88
PWM Channel : 14, programmed Period (us): 25000.00, actual : 25030, programmed DutyCycle : 90.00, actual : 89.83
PWM Channel : 15, programmed Period (us): 20000.00, actual : 20002, programmed DutyCycle : 95.00, actual : 94.98
```

---

### 2. ISR_16_PWMs_Array on RASPBERRY_PI_PICO

The following is the sample terminal output when running example [**ISR_16_PWMs_Array**](examples/ISR_16_PWMs_Array) on **RP2040-based RASPBERRY_PI_PICO** to demonstrate how to use multiple PWM channels with simple callback functions.

```
Starting ISR_16_PWMs_Array on RASPBERRY_PI_PICO
RP2040_Slow_PWM v1.2.1
[PWM] _timerNo =  0 , Clock (Hz) =  1000000.00 , _fre (Hz) =  50000.00
[PWM] _count =  0 - 20
[PWM] add_repeating_timer_us = 20
Starting ITimer OK, micros() = 3375080
Channel : 0	    Period : 1000000		OnTime : 50000	Start_Time : 3375848
Channel : 1	    Period : 500000		OnTime : 50000	Start_Time : 3376896
Channel : 2	    Period : 333333		OnTime : 66666	Start_Time : 3377917
Channel : 3	    Period : 250000		OnTime : 75000	Start_Time : 3378836
Channel : 4	    Period : 200000		OnTime : 80000	Start_Time : 3379840
Channel : 5	    Period : 166666		OnTime : 74999	Start_Time : 3380910
Channel : 6	    Period : 142857		OnTime : 71428	Start_Time : 3381965
Channel : 7	    Period : 125000		OnTime : 68750	Start_Time : 3383038
Channel : 8	    Period : 111111		OnTime : 66666	Start_Time : 3384070
Channel : 9	    Period : 100000		OnTime : 65000	Start_Time : 3385188
Channel : 10	    Period : 66666		OnTime : 46666	Start_Time : 3386372
Channel : 11	    Period : 50000		OnTime : 37500	Start_Time : 3387391
Channel : 12	    Period : 40000		OnTime : 32000	Start_Time : 3388472
Channel : 13	    Period : 33333		OnTime : 28333	Start_Time : 3389601
Channel : 14	    Period : 25000		OnTime : 22500	Start_Time : 3390754
Channel : 15	    Period : 20000		OnTime : 19000	Start_Time : 3391754
```

---

### 3. ISR_16_PWMs_Array_Simple on RASPBERRY_PI_PICO

The following is the sample terminal output when running example [**ISR_16_PWMs_Array_Simple**](examples/ISR_16_PWMs_Array_Simple) on **RP2040-based RASPBERRY_PI_PICO** to demonstrate how to use multiple PWM channels.

```
Starting ISR_16_PWMs_Array_Simple on RASPBERRY_PI_PICO
RP2040_Slow_PWM v1.2.1
[PWM] _timerNo =  0 , Clock (Hz) =  1000000.00 , _fre (Hz) =  50000.00
[PWM] _count =  0 - 20
[PWM] add_repeating_timer_us = 20
Starting ITimer OK, micros() = 3256238
Channel : 0	    Period : 1000000		OnTime : 50000	Start_Time : 3257002
Channel : 1	    Period : 500000		OnTime : 50000	Start_Time : 3258118
Channel : 2	    Period : 333333		OnTime : 66666	Start_Time : 3259117
Channel : 3	    Period : 250000		OnTime : 75000	Start_Time : 3260125
Channel : 4	    Period : 200000		OnTime : 80000	Start_Time : 3261113
Channel : 5	    Period : 166666		OnTime : 74999	Start_Time : 3261964
Channel : 6	    Period : 142857		OnTime : 71428	Start_Time : 3262970
Channel : 7	    Period : 125000		OnTime : 68750	Start_Time : 3263893
Channel : 8	    Period : 111111		OnTime : 66666	Start_Time : 3264919
Channel : 9	    Period : 100000		OnTime : 65000	Start_Time : 3265906
Channel : 10	    Period : 66666		OnTime : 46666	Start_Time : 3266883
Channel : 11	    Period : 50000		OnTime : 37500	Start_Time : 3267903
Channel : 12	    Period : 40000		OnTime : 32000	Start_Time : 3268882
Channel : 13	    Period : 33333		OnTime : 28333	Start_Time : 3269972
Channel : 14	    Period : 25000		OnTime : 22500	Start_Time : 3271036
Channel : 15	    Period : 20000		OnTime : 19000	Start_Time : 3272042
```

---

### 4. ISR_Modify_PWM on RASPBERRY_PI_PICO

The following is the sample terminal output when running example [ISR_Modify_PWM](examples/ISR_Modify_PWM) on **RASPBERRY_PI_PICO** to demonstrate how to modify PWM settings on-the-fly without deleting the PWM channel

```
Starting ISR_Modify_PWM on RASPBERRY_PI_PICO
RP2040_Slow_PWM v1.2.1
[PWM] _timerNo =  0 , Clock (Hz) =  1000000.00 , _fre (Hz) =  50000.00
[PWM] _count =  0 - 20
[PWM] add_repeating_timer_us = 20
Starting ITimer OK, micros() = 2791911
Using PWM Freq = 200.00, PWM DutyCycle = 1.00
Channel : 0	    Period : 5000		OnTime : 50	Start_Time : 2793180
Channel : 0	    Period : 10000		OnTime : 555	Start_Time : 12794374
Channel : 0	    Period : 5000		OnTime : 50	Start_Time : 22794212
Channel : 0	    Period : 10000		OnTime : 555	Start_Time : 32795264
Channel : 0	    Period : 5000		OnTime : 50	Start_Time : 42794822
Channel : 0	    Period : 10000		OnTime : 555	Start_Time : 52796069
Channel : 0	    Period : 5000		OnTime : 50	Start_Time : 62795977
Channel : 0	    Period : 10000		OnTime : 555	Start_Time : 72796753
Channel : 0	    Period : 5000		OnTime : 50	Start_Time : 82796835
Channel : 0	    Period : 10000		OnTime : 555	Start_Time : 92802985
```

---

### 5. ISR_Changing_PWM on RASPBERRY_PI_PICO

The following is the sample terminal output when running example [ISR_Changing_PWM](examples/ISR_Changing_PWM) on **RASPBERRY_PI_PICO** to demonstrate how to modify PWM settings on-the-fly by deleting the PWM channel and reinit the PWM channel

```
Starting ISR_Changing_PWM on RASPBERRY_PI_PICO
RP2040_Slow_PWM v1.2.1
[PWM] _timerNo =  0 , Clock (Hz) =  1000000.00 , _fre (Hz) =  50000.00
[PWM] _count =  0 - 20
[PWM] add_repeating_timer_us = 20
Starting ITimer OK, micros() = 3321829
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 3323139
Using PWM Freq = 2.00, PWM DutyCycle = 90.00
Channel : 0	    Period : 500000		OnTime : 450000	Start_Time : 13325144
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 23327067
Using PWM Freq = 2.00, PWM DutyCycle = 90.00
Channel : 0	    Period : 500000		OnTime : 450000	Start_Time : 33328995
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 43330843
```

---
---

### Debug

Debug is enabled by default on Serial.

You can also change the debugging level `_PWM_LOGLEVEL_` from 0 to 4

```cpp
// Don't define _PWM_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
#define _PWM_LOGLEVEL_     0
```

---

### Troubleshooting

If you get compilation errors, more often than not, you may need to install a newer version of the core for Arduino boards.

Sometimes, the library will only work if you update the board core to the latest version because I am using newly added functions.


---
---

### Issues

Submit issues to: [RP2040_Slow_PWM issues](https://github.com/khoih-prog/RP2040_Slow_PWM/issues)

---

## TO DO

1. Search for bug and improvement.
2. Similar features for remaining Arduino boards

---

## DONE

1. Basic hardware multi-channel PWM for **RP2040-based RASPBERRY_PI_PICO, ADAFRUIT_FEATHER_RP2040, etc.** using [**Earle Philhower's arduino-pico core**](https://github.com/earlephilhower/arduino-pico)
2. Add Table of Contents
3. Add functions to modify PWM settings on-the-fly
4. Improve accuracy by using `float`, instead of `uint32_t` for `dutycycle`
5. Optimize library code by using `reference-passing` instead of `value-passing`
6. DutyCycle to be optionally updated at the end current PWM period instead of immediately.
7. Display informational warning only when `_PWM_LOGLEVEL_` > 3

---
---

### Contributions and Thanks

Many thanks for everyone for bug reporting, new feature suggesting, testing and contributing to the development of this library.


---

## Contributing

If you want to contribute to this project:
- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell other people about this library

---

### License

- The library is licensed under [MIT](https://github.com/khoih-prog/RP2040_Slow_PWM/blob/main/LICENSE)

---

## Copyright

Copyright 2021- Khoi Hoang


