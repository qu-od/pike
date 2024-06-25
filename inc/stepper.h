#ifndef STEPPER_H
#define STEPPER_H

/* private funcs actually
void sm_in_1_high();
void sm_in_2_high();
void sm_in_3_high();
void sm_in_4_high();

void sm_in_1_low();
void sm_in_2_low();
void sm_in_3_low();
void sm_in_4_low();
*/

void move_stepper(
    uint8_t direction,
    uint16_t revs_per_minute,
    uint32_t steps_to_move
);
void do_step(uint8_t step_modulus);



#endif

