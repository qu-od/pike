#ifndef SONAR_H
#define SONAR_H

#include "stm32f10x_gpio.h"


void sonar_1_trig_high();
void sonar_1_trig_low();
void sonar_1_send_pulse();
void sonar_1_send_pulse_with_motors_pwm(uint16_t q_pwm_percent);

int32_t sonar_1_wait_distance();
// int32_t sonar_1_wait_distance_with_motors_pwm(uint16_t q_pwm_percent);
// uint32_t sonar_1_count_distance();

int32_t sonar_meas();
// int32_t sonar_test_with_motors_pwm(uint16_t q_pwm_percent);


#endif

