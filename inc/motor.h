#ifndef MOTOR_H
#define MOTOR_H

#include "stm32f10x.h"


void right_motor_enable();
void left_motor_enable();
void right_motor_disable();
void left_motor_disable();
void right_motor_gear_forward();
void left_motor_gear_forward();
void right_motor_gear_reverse();
void left_motor_gear_reverse();

void drive_starting_profile(uint8_t move_dir);
int32_t drive_linear_u_to_v_profile(
        int32_t dist_to_corr_mm,
        int32_t previous_q,
        int32_t software_max_q,
        int32_t software_max_q_diff_accel,
        int32_t software_max_q_diff_deccel
    );
void drive_slowing_to_stop_profile(uint8_t move_dir);

void fixed_drive_profile_test();
int32_t rangefinder_10Hz_corr(
        int32_t target_dist_mm,  // USER RANGE MODE
        uint32_t target_dist_sine_ampl_mm,   // USER RANGE MODE
        uint32_t target_dist_sine_period_secs,   // USER RANGE MODE
        int32_t software_max_q,  // USER MOTOR MODE CONSTRAINT
        int32_t software_max_q_diff_accel,  // USER MOTOR MODE CONSTRAINT
        int32_t software_max_q_diff_deccel,  // USER MOTOR MODE CONSTRAINT
        uint8_t plan_d_mode  // USER MODE FLAG (0 - off, 1 - on)
    );

void move_platform_trapz(
        int32_t distance_to_go,
        uint16_t peak_velocity,
        uint16_t peak_acceleration
    );

void trapz_experiment();

void revolute_motor_fastest_mode(
        uint8_t motor,
        int32_t distance_to_go
    );
void revolute_motor(
        int32_t distance_to_go,
        uint16_t q_percent_for_max_v,  
        uint16_t peak_acceleration
    );

void max_voltage_short_time_test();
void pwm_func_ladder_test();
void motors_dumb_test();
void movement_demo();

void kick_forward();
void kick_backward();

#endif

