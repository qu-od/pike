// #include <stdio.h>  // избавиться от библиотек
// #include <stdlib.h>
// #include <string.h>

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"



static __IO uint32_t four_hz_timer;
static __IO uint32_t ms_timer;
static __IO uint32_t us_timer;

#define F_CPU_MHZ_DEF 48  // SECOND DEFINITION!
#define IR_FREQ_4_HZ  // SECOND DEFINITION!
#define TICKS_IN_SECS (((uint32_t)F_CPU_MHZ_DEF) * 1000000)
#define TICKS_IN_MS (((uint32_t)F_CPU_MHZ_DEF) * 1000)
#define TICKS_IN_100_US (((uint32_t)F_CPU_MHZ_DEF) * 100)  // TEST
#define TICKS_IN_10_US (((uint32_t)F_CPU_MHZ_DEF) * 10)  // TEST
#define TICKS_IN_1_US ((uint32_t)F_CPU_MHZ_DEF)

#ifdef IR_FREQ_4_HZ
#define TICKS_BEFORE_CYCLIC_INTERRUPT ((uint32_t)TICKS_IN_SECS >> 2)
#endif


void tim_4Hz_delay(){
	    // ------- SysTick CONFIG --------------
    if (SysTick_Config(TICKS_BEFORE_CYCLIC_INTERRUPT)) //
    {
        while(1); // error
    }
    four_hz_timer = 1;
    while(four_hz_timer) {
        asm("wfi");
    };
    SysTick -> LOAD &= ~(SysTick_CTRL_ENABLE_Msk);    // disable SysTick
}

void delay(__IO uint32_t mantissa, __IO int8_t exponent) {
    /*
        exp = 0 --> val in seconds
        exp = 3 --> val in ms
        exp = 5 --> val in 10us
        allowed values are [0, 1, 2, 3, 4, 5, 6]
        lower limit depends on on F_CPU
    */
    uint8_t max_allowed_exponent_abs = 5;
    if ((exponent > max_allowed_exponent_abs) || (exponent < 0)){
        mantissa = 0;  // make no delay then
    }
    uint32_t exp_term = 1;
    for (int8_t i=max_allowed_exponent_abs-exponent; i>0; i--){
        exp_term = 10 * exp_term;
    }
    uint32_t delay_val_in_10_us = mantissa * exp_term;  // NEED IMPLEMENTING!!
    // ------- SysTick CONFIG --------------
    if (SysTick_Config(TICKS_IN_10_US))
    // DO NOT WORK WITH TICKS_IN_1_US ALREADY!!!
    {
        while(1);  // error
    }
    ms_timer = delay_val_in_10_us;
    while(ms_timer){
        asm("wfi");
    };
    SysTick -> LOAD &= ~(SysTick_CTRL_ENABLE_Msk);    // disable SysTick
}


/*
void delay(__IO uint32_t mantissa, __IO int8_t exponent) {

    // NEED TESTING!!!

    // ------- SysTick CONFIG --------------
    uint8_t max_allowed_exponent_abs = 6;
    if ((exponent > max_allowed_exponent_abs) || (exponent < 0)){
        mantissa = 0;  // make no delay then
    }
    uint32_t exp_term = 1;
    for (int8_t i=max_allowed_exponent_abs-exponent; i>0; i--){
        exp_term = 10 * exp_term;
    }
    uint32_t delay_val_in_us = mantissa * exp_term;
    if (SysTick_Config(TICKS_IN_1_US))
    {
        while(1); // error
    }
    us_timer = delay_val_in_us;
    while(us_timer) {
        asm("wfi");
    };
    SysTick -> LOAD &= ~(SysTick_CTRL_ENABLE_Msk);    // disable SysTick
}
*/


/*
void delay_us(__IO uint32_t val) { // NEED TESTING!!
//ЛОМАЕТ ВСЮ ПРОГУ НЕОДНОКРАТНО!  ОСТОРОЖНО!
    // ------- SysTick CONFIG --------------
    if (SysTick_Config(TICKS_IN_US)) //
    {
        while(1); // error
    }
    us_timer = val;
    while(us_timer) {
        asm("wfi");
    };
    SysTick -> LOAD &= ~(SysTick_CTRL_ENABLE_Msk);    // disable SysTick
}
*/


void SysTick_Handler(void) // КОСТЫЛЬ! РАЗОБРАТЬСЯ!
// так скорее всего нельзя делать. Таймеры плохо друг на друга влияют?
{
	if (four_hz_timer)
        four_hz_timer--;
    if (ms_timer)
        ms_timer--;
	if (us_timer)
        us_timer--;
}


// --------------- LED FUNCS ---------------------
void led_blink_once()
{
    GPIOC -> BSRR = GPIO_BSRR_BS13;  // включить светодиод
	delay(5, 1);  // задержка 500мс
    GPIOC -> BSRR = GPIO_BSRR_BR13;  // погасить светодиод
	delay(5, 1);  // задержка 500мс
}


void led_flash_once()  
{
    GPIOC -> BSRR = GPIO_BSRR_BS13;  // включить светодиод
	delay(1, 1);  // задержка 100мс
    GPIOC -> BSRR = GPIO_BSRR_BR13;  // погасить светодиод
	delay(4, 1);  // задержка 400мс
}

void led_dim_once()
{       
    GPIOC -> BSRR = GPIO_BSRR_BS13;  // включить светодиод
    delay(4, 1);  // задержка 400мс
	GPIOC -> BSRR = GPIO_BSRR_BR13;  // погасить светодиод
    delay(1, 1);  // задержка 100мс
}

void led_blink(uint8_t n)
{
	for(uint8_t i=0; i<n; i++)  // повторить n раз в цикле
	{
		led_blink_once();
	}
}

void led_flash(uint8_t n)
{
	for(uint8_t i=0; i<n; i++)
	{
		led_flash_once();
	}	
}

void led_dim(uint8_t n)
{
	for(uint8_t i=0; i<n; i++)
	{
		led_dim_once();
	}
	
}



void dummy_loop(uint32_t count)
{
    while(--count);
}


void dummy_led_dim(uint8_t n)
{
	for(uint8_t i=0; i<n; i++)
	{
		GPIOC -> BSRR = GPIO_BSRR_BS13;
	    dummy_loop(2500000);  // NEED CHECKING!
	    GPIOC -> BSRR = GPIO_BSRR_BR13;
	    dummy_loop(500000);  // NEED CHECKING!
	}
}


uint32_t calc_brr_bits(uint8_t F_CPU_MHZ, uint32_t BAUDRATE)
{   // BRR_total_value = AHBxClK / TrueBaudRate
    // all AHB mults must be 1 for result to be correct
    uint32_t f_cpu_in_hz = ((uint32_t)F_CPU_MHZ) * 1000000;
    uint32_t brr_mantissa = f_cpu_in_hz / (BAUDRATE * 16);
    uint32_t brr_fraction = 0;  // округлим пока. боимся деления с плав. точкой
    // APPROX INT DEVISION. NEEDS CHECKING!
    return (brr_mantissa << 4) + brr_fraction; // mantissa and fraction
}


char char_digit_0_from_int8(const uint8_t f_cpu_freq)
// нужно из-за ошибок в передаче целых строк
{
	uint8_t digit_pos_0 = f_cpu_freq % 10;
	return ((char)digit_pos_0 + '0');
}

char char_digit_1_from_int8(const uint8_t f_cpu_freq)
//f_cpu must ne less than 100 for this func to work
{
	uint8_t digit_pos_1 = f_cpu_freq / 10;
	return ((char)digit_pos_1 + '0');
}


char* itoa(int value, char* result, int base) {
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }
    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;
    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );
    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    // Reverse the string
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}


int32_t my_abs_int32_t(int32_t raw_val){
    int32_t abs_val;
    if (raw_val < 0){
        return (-1) * raw_val;
    }
    else{
        return raw_val;
    }
}

/*
char* concat(char* ans_str, char *s1, char *s2)
{
    return ans_str;
}

char* int_to_str(char* ans_str, uint16_t num, uint8_t num_of_positions)
{
	sprintf(ans_str, "%d", num);
	return ans_str;
}
*/