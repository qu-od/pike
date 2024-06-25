#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"


static __IO uint32_t ms_timer;
static __IO uint32_t us_timer;

void tim_4Hz_delay();
// void delay_ms(__IO uint32_t val);
void delay(__IO uint32_t mantissa, __IO int8_t exponent);
// void delay_us(__IO uint32_t val);  // НЕ РАБОТАЕТ
void SysTick_Handler(void);


void brr_flash_test_5();
void led_blink_once();
void led_flash_once();
void led_dim_once();
void led_blink(uint8_t n);
void led_flash(uint8_t n);
void led_dim(uint8_t n);

void dummy_loop(uint32_t count);
void dummy_led_dim(uint8_t n);

uint32_t calc_brr_bits(uint8_t F_CPU_MHZ, uint32_t BAUDRATE);

char char_digit_0_from_int8(uint8_t f_cpu_freq);
char char_digit_1_from_int8(uint8_t f_cpu_freq);

char* itoa(int value, char* result, int base);
int32_t my_abs_int32_t(int32_t raw_val);

// need revision!