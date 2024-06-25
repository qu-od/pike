#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

#include "sonar.h"
#include "wheel.h"



/*
void get_pulse_delay()
void get_distance()
void get_echo_energy()
void set_rangefinder_config()
void calibrate()
*/



void sonar_1_trig_high(){
	GPIOC -> BSRR = GPIO_Pin_14;  // поставили 1
}

void sonar_1_trig_low(){
	GPIOC -> BRR = GPIO_Pin_14;  // поставили 0
}

/*
void sonar_2_trig_high(){
	GPIOC -> BSRR = GPIO_Pin_15;
}

void sonar_2_trig_low(){
	GPIOC -> BRR = GPIO_Pin_15;
}
*/





void sonar_1_send_pulse_with_motors_pwm(uint16_t q_pwm_percent){
    // NEED IMPLEMENTING
    sonar_1_trig_high();
    delay(1, 3);
    sonar_1_trig_low();
}


/*
void sonar_2_send_pulse(){
    sonar_2_trig_high();
    delay(1, 3); // delay_us НЕ РАБОТАЕТ!!!
    sonar_2_trig_low();
}
*/



uint8_t input_check(){
    uint8_t bitstatus = 0x00;
    if ((GPIOA->IDR & GPIO_Pin_2) != (uint32_t)Bit_RESET)
    {
        bitstatus = (uint8_t)Bit_SET;
    }
    else
    {
        bitstatus = (uint8_t)Bit_RESET;
    }
    return bitstatus;
}


void sonar_1_send_pulse(){
    sonar_1_trig_high();  // лог. 1 на вход управления УЗДМ
    delay(1, 3);  // длительность импульса управления = 1мс
    sonar_1_trig_low();  // лог. 0 на вход управления УЗДМ
}

int32_t sonar_1_wait_distance(){
    int32_t dist_counter_of_10_us = 0;  // 10мкс-счётчик измерения отклика от сонара
    int32_t dist_in_mm = 0;  // определение переменной под расстояние
    for (uint32_t c=0; c<1000; c++){  // измерение отклика от сонара с частотой 100Гц
        // (сонар обрабатывает эхо порядка единиц мс!!!)
        if (input_check()){  // условие высокого уровня на пине отклика сонара
            dist_counter_of_10_us++;  // увеличиваем знач. счётчика, если отклик есть
        }
        delay(1, 5);  // задержка 10мкс после каждой итерации
    }
        // задержка_до_эха[мкс] / 58 = расстояние[мм]  
        dist_in_mm = (dist_counter_of_10_us * 100) / 58;
        // dist_in_mm = dist_counter_of_10_us * 2;  // вариант с округлением
    return dist_in_mm;  // возвращение результата
}


int32_t sonar_meas(){
    int32_t distance_1_mm = 0;
    sonar_1_send_pulse();   // генерация импульса запуска сонара
    distance_1_mm = sonar_1_wait_distance();  // обработка отклика сонара
    return distance_1_mm;  // оценка расстояния до цели
}


int32_t sonar_1_wait_distance_with_motors_pwm(uint16_t q_pwm_percent){
    // NEED IMPLEMENTING
    int32_t dist_counter_of_10_us = 0;
    int32_t dist_in_mm = 0;
    for (uint32_t c=0; c<1000; c++){ // 100 Hz measur-s
        // ВЕРНУТЬ с<1000, а то не померяет (УЗДМ обрабатывает эхо порядка единиц мс!!!)
        if (input_check()){  // if A high
            dist_counter_of_10_us++;  // ms or 100us?? CHECK IT!
        }
        delay(1, 5);
    }
        // us/58 = mm
        dist_in_mm = (dist_counter_of_10_us * 100) / 58;
        // dist_in_mm = dist_counter_of_10_us * 2;  // грубое округление!!
    return dist_in_mm;
}


int32_t sonar_test_with_motors_pwm(uint16_t q_pwm_percent){
    // на эту функцию затрачивается несколько десятых долей сек. ПОЧЕМУ.
    int32_t distance_1_mm = 0;
    sonar_1_send_pulse_with_motors_pwm(q_pwm_percent);
    distance_1_mm = sonar_1_wait_distance_with_motors_pwm(q_pwm_percent);
    return distance_1_mm;
}

