#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#include "wheel.h"
#include "startup.h"
#include "uart.h"
#include "lcd.h"
#include "sonar.h"
#include "motor.h"
#include "rsku.h"


// 1602 LCD library was used ("lcd_1602_drive.h")
// #define TEST_DEFINE ((uint32_t)0x00000080)
// const uint8_t F_CPU_MHZ = 72;  // test freq
// only 16, 24, 32, 40, 48, 56, 64, 72 allowed
const uint8_t F_CPU_MHZ = 48;  // 13 var freq
#define IR_FREQ_4_HZ

// const uint32_t BAUDRATE = 115200;  // test baudrate (bits per sec)
// any sane value allowed
const uint32_t BAUDRATE = 38400;  // (bits per second)
const uint16_t BAUDRATE_KBOD = BAUDRATE / 1000;  // (bits per second)

// char f_cpu_pos_0 = '0';
// char_digit_0_from_int8(F_CPU_MHZ);
// char f_cpu_pos_1 = '0';
// char_digit_1_from_int8(F_CPU_MHZ);


void EXTI1_IRQHandler(void)  // Обработчик EXTI 1
{
    dummy_led_dim(7);
    // Сбрасываем флаг прерывания единичкой
    EXTI -> PR |= EXTI_PR_PR1;
}
 
 
void EXTI2_IRQHandler(void)  // Обработчик EXTI 2
{
    // dummy_led_dim(4);
    // dummy_led_dim(1);  // АХАХАХАХАХАХАХАХАХАХААХ
    // Сбрасываем флаг прерывания
    EXTI -> PR |= EXTI_PR_PR2;
} 


void gpio_init()
{
    // --- enable GPIO sync ----
    RCC -> APB2ENR |= RCC_APB2Periph_GPIOA;
    RCC -> APB2ENR |= RCC_APB2Periph_GPIOB;
    RCC -> APB2ENR |= RCC_APB2Periph_GPIOC;

    // --- GPIO setup ----
    GPIOC -> CRH &= ~(uint32_t)(0xf<<20);  // Режим 00 - Push-Pull   // DEPRECATED
    GPIOC -> CRH |=  (uint32_t)(0x3<<20);  // Скорость 11. (Max Speed 50MHz)  // DEPRECATED
    GPIOC -> BSRR = GPIO_BSRR_BR13; // LED OFF??

    GPIOA -> CRL = (uint32_t)(0x33333333);  // Тоже Push-Pull 50MHz
    GPIOA -> CRH = (uint32_t)(0x33333333);
    GPIOB -> CRL = (uint32_t)(0x33333333);
    GPIOB -> CRH = (uint32_t)(0x33333333);
    GPIOC -> CRL = (uint32_t)(0x33333333);  // не нужно так-то
    GPIOC -> CRH = (uint32_t)(0x33333333);
}


void uart_init()
{
    // --- UART setup ----
	/* 
        PA8  - CK1  (пока не нужен)
        PA9  - TX1
        PA10 - RX1
        PA11 - CTS1 (пока не нужен)
        PA12 - RTS1 (пока не нужен)
	*/
    RCC -> APB2ENR |= RCC_APB2Periph_USART1;
    GPIOA -> CRH &= ~(uint32_t)(0xf<<4);
    // тут медленный альтернативный пуш-пулл для PA9
    GPIOA -> CRH |=  (uint32_t)(0xa<<4);
    uint32_t brr_bits = calc_brr_bits(F_CPU_MHZ, BAUDRATE);
    USART1 -> BRR = ((uint16_t)brr_bits);  // BaudRate Register
    // 0x271 --> 0000 0010 0111 0001 --Ю 115200 при F_cpu_MHz=72
    USART1 -> CR1 |= (USART_CR1_UE_Set | USART_Mode_Tx);
}


void buttons_intrpts_init()
{
    // --- BUTTONS setup ----
    /*
        PA1_pulled_up - button 1 to gnd
        PA2_pulled_up - button 2 to gnd
    */
    // Надо  AFIO затактировать. Иначе будет трудно уловимый глюк! 
    RCC -> APB2ENR 	|= (
        RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN
    );	
        
    // Выставляем режим порта в CNF для битов 1,2 Режим 10 = PullUp(Down)
    //Ставим первый бит CRL
    GPIOA -> CRL	&= ~(GPIO_CRL_CNF1 | GPIO_CRL_CNF2);   // Занулим заранее
    GPIOA -> CRL 	|= GPIO_CRL_CNF1_1 | GPIO_CRL_CNF2_1;  // Выставим бит 1
    
    // Выставляем режим для 1,2 пина. Режим MODE_00 = Input
    GPIOA -> CRL	&= ~(GPIO_CRL_MODE1 | GPIO_CRL_MODE2);	
    
    // Установили бит 1,2 в ODR включив PULL UP.
    GPIOA -> BSRR = GPIO_BSRR_BS2 | GPIO_BSRR_BS1;		
 
    /*
    Настраиваем EXTI1 и EXTI2  на выводы порта А. Они в первом регистре оба.
    А регистры в виде массива описаны в CMSIS
    Регистры EXTICR сгруппированы в массив, а массив считается с нуля!
    Т.е. первый EXTICR регистр в 0 ячейке
    второй регистр в первой, третий во 2
    */
    AFIO -> EXTICR[0] |= AFIO_EXTICR1_EXTI1_PA | AFIO_EXTICR1_EXTI2_PA;
    
    // Прерывание по падению уровня на пинах 1 и 2 порта привязанного к EXTI
    EXTI -> FTSR |= (EXTI_FTSR_TR1 | EXTI_FTSR_TR2);
    
    NVIC_EnableIRQ(EXTI1_IRQn);
    NVIC_EnableIRQ(EXTI2_IRQn);
    NVIC_SetPriority(EXTI2_IRQn, 2);
    
    // Разрешаем прерывания в периферии
    EXTI -> IMR |= (EXTI_IMR_MR1 | EXTI_IMR_MR2);
}







// --------------------------- MAIN -----------------------------
int main()
{
    // system_init called from startup directly
    // но для красоты можно ещё раз вызвать здесь
    SystemInit();  // настройка процессорп
    SetSysClockToReqFreq(F_CPU_MHZ);  // тактировка процессора
    gpio_init();  // настройка портов ввода-вывода
    uart_init();  // 
    buttons_intrpts_init();
    __enable_irq();  // включение обработчика прерываний
      // IRQ NEED TESTING!

    led_dim(3);  // моргнуть светодиодом три раза
    // lcd_demo(F_CPU_MHZ, BAUDRATE);  // п.4 ДЗ
    show_project_data(" v.1.3.0");
    delay(1, 0);
    show_system_data(
        F_CPU_MHZ,
        BAUDRATE_KBOD,
        0, 0, 0, 0, 0, 0
    );  // вывод на дисплей
    // move_platform_trapz(1, 30, 0);
    // move_platform_trapz(-1, 30, 0);
    delay(3, 0);  // задержка 3 секунды

    uint32_t target_dist_mm = 300;  // USER RANGE MODE
    uint32_t target_dist_sine_ampl_mm = 0;   // USER RANGE MODE
    uint32_t target_dist_sine_period_secs = 1;   // USER RANGE MODE
    int32_t software_max_q = 50;  // USER MOTOR MODE CONSTRAINT
    int32_t software_max_q_diff_accel = 10;  // USER MOTOR MODE CONSTRAINT
    int32_t software_max_q_diff_deccel = -10;  // USER MOTOR MODE CONSTRAINT
    uint8_t plan_d_mode = 0;  // USER MODE FLAG (0 - off, 1 - on)

    int32_t rangefinder_result;
    fixed_drive_profile_test();
    delay(1, 0);
    rangefinder_result = rangefinder_10Hz_corr(
        target_dist_mm,
        target_dist_sine_ampl_mm,
        target_dist_sine_period_secs,
        software_max_q,
        software_max_q_diff_accel,
        software_max_q_diff_deccel,
        plan_d_mode
    );
    delay(1, 0);

    int32_t distance_1_mm;
    int32_t dist_to_corr_mm;
    char sonar_1_val_str[7];  // check length
    char dist_to_corr_val_str[7];  // check length

    for (uint8_t kk = 0; kk<1000; kk++){
        // GPIOC -> BSRR = GPIO_Pin_13;  // led on
        distance_1_mm = sonar_meas();
        // GPIOC -> BRR = GPIO_Pin_13;  // led off
        dist_to_corr_mm = distance_1_mm - target_dist_mm;
        // delay(1, 1);

        // GPIOC -> BSRR = GPIO_Pin_13;  // led on
        lcd_msg(1, 9, "     M3");
	    lcd_msg(2, 9, "     M3");
        lcd_msg(1, 9, itoa(distance_1_mm, sonar_1_val_str, 10));
        lcd_msg(2, 9, itoa(dist_to_corr_mm, dist_to_corr_val_str, 10));
        // GPIOC -> BRR = GPIO_Pin_13;  // led off
        // delay(1, 1);
        // lcd_msg(1, 9, "                  ");
	    // lcd_msg(2, 9, "                  ");
        // delay(1, 1);

        // GPIOC -> BSRR = GPIO_Pin_13;  // led on
        if (dist_to_corr_mm > 3){  // dead zone = 2cm
            kick_forward();
        }
        if (dist_to_corr_mm < -3){
            kick_backward();
        }
        // GPIOC -> BSRR = GPIO_Pin_13;  // led off
    }

    char string_for_ic[4];
    uint16_t itrpts_counter = 0;
    for(;;)
    {
        delay(25, 2);  // 4 Hz
        itrpts_counter = itrpts_counter + 1;
        GPIOC -> ODR ^= GPIO_Pin_13;  // led toggle

        lcd_msg(2,12, itoa(itrpts_counter, string_for_ic, 10));  // PARAMETRIZE
        usart1_send_string_of_30_chars(  // п.3 ДЗ
            // string transmittion fails
            'C','u','r','r','e','n','t',
            ' ','F','_','C','P','U',
            '=', '4', '8',
            'M','H','z',
            ' ','-','-','-','-','-',
            '-','-','-','-', 0x0000000D
            );
        usart1_send_string_of_30_chars(  // п.3 ДЗ
            // string transmittion fails
            'D','e','b','u','g',' ',
            '_',' ','_',' ','_',' ', '_',
            0x000000F0, 0x000000E0, 0x000000E1, 0x000000EE,
            0x000000F2, 0x000000E0, 0x000000E5, 0x000000F2,
            ' ','-','-','-',
            '-','-','-','-', 0x0000000D
            );
    }
}








 

 
