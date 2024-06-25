#include "stm32f10x_gpio.h"

#include "stepper.h"
#include "wheel.h"


/*
 * Step C0 C1 C2 C3
 *    1  1  0  1  0
 *    2  0  1  1  0
 *    3  0  1  0  1
 *    4  1  0  0  1
*/


const uint16_t STEPS_PER_REV = 2048;  // NEED CHECKING!!


void sm_in_1_high(){
	GPIOB -> BSRR = GPIO_Pin_10;  // поставили 1
}

void sm_in_2_high(){
	GPIOB -> BSRR = GPIO_Pin_11;
}

void sm_in_3_high(){
	GPIOB -> BSRR = GPIO_Pin_6;
}

void sm_in_4_high(){
	GPIOB -> BSRR = GPIO_Pin_7;
}

void sm_in_1_low(){
	GPIOB -> BRR = GPIO_Pin_10;  // поставили 0
}

void sm_in_2_low(){
	GPIOB -> BRR = GPIO_Pin_11;
}

void sm_in_3_low(){
	GPIOB -> BRR = GPIO_Pin_6;
}

void sm_in_4_low(){
	GPIOB -> BRR = GPIO_Pin_7;
}



/*
 * Moves the motor steps_to_move steps.  If the number is negative,
 * the motor moves in the reverse direction.
 */
void move_stepper(
    uint8_t direction,
    uint16_t revs_per_minute,  // speed
    uint32_t steps_to_move  // dist in steps
    )
{
    uint32_t step_delay_ms = ((60 * 1000) / revs_per_minute) / STEPS_PER_REV;
    // вынест потом вычисление коэффициента из функции для оптимизации

    uint32_t step_number = 0;  // UNSIGNED VAR!!!
    if (direction == 0)
    {
        step_number = STEPS_PER_REV;
    }

    uint8_t step_modulus = 0;

    // decrement the number of steps, moving one step each time:
    while (steps_to_move > 0)
    {
        delay(step_delay_ms, 3);  // CHECK UNITS!!
        // increment or decrement the step number,
        // depending on direction:

        if (direction == 1)
        {
            step_number++;
            if (step_number >= STEPS_PER_REV)  // >= instead of == just in case...
            {
                step_number = 0;
            }
        }

        else  // if (direction == 0)
        {
            if (step_number == 0)  //
            {
                step_number = STEPS_PER_REV;
            }
        step_number--;
        }

        // decrement the steps left:
        steps_to_move--;
        // step the motor to step number 0, 1, 2, 3
        step_modulus = step_number % 4;
        do_step(step_modulus);
    }
}



// Moves the motor forward or backwards.
void do_step(uint8_t step_modulus)
{
    switch (step_modulus)
    {
        case 0:  // 1010
            sm_in_1_high();
            sm_in_2_low();
            sm_in_3_high();
            sm_in_4_low();
        break;
        case 1:  // 0110
            sm_in_1_low();
            sm_in_2_high();
            sm_in_3_high();
            sm_in_4_low();
        break;
        case 2:  //0101
            sm_in_1_low();
            sm_in_2_high();
            sm_in_3_low();
            sm_in_4_high();
        break;
        case 3:  //1001
            sm_in_1_high();
            sm_in_2_low();
            sm_in_3_low();
            sm_in_4_high();
        break;
    }
}
