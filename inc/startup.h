#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

void SetSysClockToReqFreq(uint8_t cpu_freq_in_MHz);
void SystemInit(void);