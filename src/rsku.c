#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

/*
--- здесь помимо прочего определяется выбор режимов --- 
три режима по мотору:
- без ограничений 
- с ограничением на скорость, но не на ускорение ()
- с ограничением на скорость и на ускорение


--- три режима по слежению за расстоянием --- 
- свдение расстояния в ноль
- сведение расстояния к постоянному значению
- изменение требуемого расстояния по квазисинусоидальному закону 


Спецрежим "plan d", задёйствующий второй дальномер (расположен сзади)
    Целевая функция - равенство измеряемых дальностей
    Ограничение на Скорость и ускорение в моменте пропорциональны
        разнице измеряемых расстояний.


*/

// ------------------------------ CONSTS -----------------------------------
// все единицы измерения в системе СИ (даже RPS вместо RPM используем)
// time discretes
uint16_t DISTANCE_MEAS_PERIOD = 0;
uint32_t VOLTAGE_CORRECTION_PERIOD = 0;

// целевое расстояние
uint32_t TARGET_CONST_DISTANCE = 0;
uint32_t TARGET_DISTANCE_SINE_PERIOD = 0;  // для квазисинусоидального режима

// ограничения по мотору 
uint32_t MAX_VELOCITY = 0;
uint32_t MAX_ACCELERATION = 0;

// вольт-оборотная характеристика
// ТРЕБУЕТСЯ ЗАМЕНА ЗНАЧЕНИЙ ПААМЕТРОВ ПОСЛЕ ЗАМЕНЫ ДВИГАТЕЛЯ
// редукция в данном случае = (MOTOR_RPM / REDUCTION_RATE)
uint32_t MAX_MOTOR_VOLTAGE = 0;  // максимум используемого участка вольт-оборотной характеристики
uint32_t MIN_MOTOR_VOLTAGE = 0;  // соответственно минимум
uint32_t VOLTAGE_RANGE = 0;  // 
uint32_t VOLTS_TO_MOTOR_RPS_COEFF = 0;  // 
uint32_t REDUCTION_RATE = 0;  // редукция (понижающая, значение > 1)
uint32_t WHEEL_RPM_TO_LINEAR_VELOCITY_COEFF = 0;  // 
uint32_t DISTANCE_TO_TIME_DELAY_COEFF = 0;

// границы передвижения
uint32_t MOVEMENT_ZONE_RADIUS = 0;

// время, как вторая координата
uint32_t TARGET_TIME_NAVEDENYA = 0;

// прочие значения
uint32_t SPEED_OF_SOUND = 0;

//-------------------------- variables to update constantly ---------------------
/*
// префикс inertial значит, что значение оценено алгоритмически на МК
    (да, на основании показаний датчиков, но косвенно)
// префикс measured значит, что значение получено измерительным датчиком
    (в этом случае - напрямую.
        Ну разве что через одну пропорциональную зависимость)

ECHO_DELAY - значение задержки, которое приходит с УЗДМ
RANGE - расстояние, получаемое из ECHO_DELAY
VOLTAGE - значение управляемого напряжения

fluctuation_measured_range_error
dynamic_measured_range_error
measured_range

absolute_pike_coord
absolute_echo_coord
target_range
motor_voltage
motor_rps
reduction_rate





*/




// ------------------------ MODES FUNCS-------------------------------
void set_motor_mode(){}  // запись значения в регистр (определение переменной по сути)

void set_distance_mode(){}  //

void toggle_plan_d_mode(){}  //



// -----------------------------------------------------------------------------
void set_max_velocity(){}

void set_max_voltage(){}  // depends on velocity

void set_max_distance(){}

void set_max_acceleration(){}



// ---------------------- FUNCS-INDEP-CHECKERS ---------------------------------

void check_bounds(){}





// ---------------------------- funcs---------------------------------------







// -----------------------------------------------------------------------------



/*
void target_pos(){}

void measured_pos(){}

void current_pos_error(){}

void integral_pos_error(){}

void current_speed(){}

void recommended_speed(){}

void integral_rsku_quality(){}

void distance_function_mode(){}

void aiming_method_mode(){}

void automatic_or_director_mode(){}
*/
