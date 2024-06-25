#include "stm32f10x_gpio.h"

#include "motor.h"
#include "wheel.h"
#include "sonar.h"
#include "lcd.h"


const uint32_t Q_BOUND_UP = 30;
const uint32_t Q_BOUND_DOWN = 20;
const uint32_t DEAD_ZONE_HALF_MM = 5;

// -----------------------------------------------------------------------------
void left_motor_enable(){  //ENB
	GPIOB -> BSRR = GPIO_Pin_11;  // 1
    // регулирование скорости добавить позднее
}

void right_motor_enable(){  //ENA
	GPIOB -> BSRR = GPIO_Pin_10;  // 1
    // регулирование скорости добавить позднее
}

void left_motor_disable(){  //ENB
	GPIOB -> BRR = GPIO_Pin_11;  // 0
    // регулирование скорости добавить позднее
}

void right_motor_disable(){  //ENA
	GPIOB -> BRR = GPIO_Pin_10;  // 0
    // регулирование скорости добавить позднее
}



// -----------------------------------------------------------------------------
void left_motor_gear_forward(){ 
	GPIOB -> BSRR = GPIO_Pin_12;  // 1
	GPIOC -> BRR = GPIO_Pin_13;  // 0
}

void right_motor_gear_forward(){
	GPIOB -> BSRR = GPIO_Pin_6;  // 1
	GPIOB -> BRR = GPIO_Pin_7;  // 0
}

void left_motor_gear_reverse(){
	GPIOC -> BSRR = GPIO_Pin_13;  // 1
	GPIOB -> BRR = GPIO_Pin_12;  // 0
}

void right_motor_gear_reverse(){
	GPIOB -> BSRR = GPIO_Pin_7;  // 1
	GPIOB -> BRR = GPIO_Pin_6;  // 0
}



// -----------------------------------------------------------------------------
void pwm_to_motor(
        uint8_t motor, //1-right, 2-left, 12-both
        uint32_t duration_ms,  // длительность ШИМ-сигнала
        uint16_t pwm_high_level_percent  // заполнение ШИМ-а в процентах
        // НЕЛЬЗЯ СТАВИТЬ 0!!! (иначе моторы нестабильно управляются)
    ){
    uint32_t pwm_period_10th_ms = 20;  // Настройка периода ШИМ (2 мс)
    uint32_t num_of_pwm_periods = (duration_ms * 10) / pwm_period_10th_ms;
    // кол-во периодов ШИМ-сигнала
    uint32_t high_level_dur_10th_ms = (pwm_period_10th_ms * pwm_high_level_percent) / 100;
    // длительность высокого логического уровня в периоде ШИМ-а
    uint32_t low_level_dur_10th_ms = pwm_period_10th_ms - high_level_dur_10th_ms;
    // длительность высокого логического уровня в периоде ШИМ-а
    if (motor == 1){  // если выбран мотор №1 (правое заднее колесо)
        for(uint32_t i=0; i<num_of_pwm_periods; i++){
            right_motor_enable();  // лог.1 на пин ШИМ-управления мотором №1
            delay(high_level_dur_10th_ms, 4);  // длительность выс. лог. уровня 
            right_motor_disable();  // лог.0 на пин ШИМ-управления мотором №1
            delay(low_level_dur_10th_ms, 4);  // длительность низ. лог. уровня
        }
    }
    if (motor == 2){  // аналогично для мотора №2 (левое заднее колесо)
        for(uint32_t i=0; i<num_of_pwm_periods; i++){
            left_motor_enable();
            delay(high_level_dur_10th_ms, 4);
            left_motor_disable();
            delay(low_level_dur_10th_ms, 4);
        }
    }
    if (motor == 12){  // и далее для двух моторов сразу
        for(uint32_t i=0; i<num_of_pwm_periods; i++){
            right_motor_enable();
            left_motor_enable();
            delay(high_level_dur_10th_ms, 4);
            right_motor_disable();
            left_motor_disable();
            delay(low_level_dur_10th_ms, 4);
        }
    }
}


// -----------------------------------------------------------------------------
void drive_starting_profile(uint8_t move_dir){  // NEED CHECKING!
    // быстро нагонять скважность до минимальной стартовой
        // чтобы толкать платформу не надо было
    // а в процессе движения можно и опускаться ниже 
        // этой планки Q_BOUND_UP
            // но аккуратно. А то платформа может "заглохнуть"
    // ИСПОЛЬЗОВАТЬ ПЕРЕРЕГУЛИРОВАНИЕ
        // (тип дёрнуть до q=30 (Q_BOUND_UP),
            // а потом опустить до q=10 (Q_BOUND_DOWN))
    // главное - не заглохнуть
    // а если заглох, то повторить процедуру старта,
        // но уже не опускаться потом так сильно по q

    if (move_dir == 1){  // вперёд
        right_motor_gear_forward();
        left_motor_gear_forward();
    }
    if (move_dir == 2){  // наазад
        right_motor_gear_reverse();
        left_motor_gear_reverse();
    }
    for (uint16_t q=5; q<=Q_BOUND_UP; q+=5){
        pwm_to_motor(12, 100, q);  // ШИМ с возрастающим заполнением
    }
    for (uint16_t q=Q_BOUND_UP; q>=Q_BOUND_DOWN; q-=5){
        pwm_to_motor(12, 100, q);  // ШИМ с убывающим заполнением
    }
}


int32_t drive_linear_u_to_v_profile(   // NEED CHECKING!
        int32_t dist_to_corr_mm,  // distance to go (ideally) 
        int32_t previous_q,  // current driving q
        int32_t software_max_q,  // max velocity
        int32_t software_max_q_diff_accel,  // max acceleration (>0)
        int32_t software_max_q_diff_deccel  // max decceleration (<0) (abs > than max acc)
    ){
    // возвращает new_q либо спец_значение:
    // new_q > 0  -->  оставаться на линейном режиме
    // new_q = -1  -->  достигли порога торможения по q
    // new_q = -2  -->  уже приехали! хардстоп!
    // new_q = -3  -->  заглохли! нужно стартануть!
        //  ДОБАВИТЬ ПРОВЕРКУ НА СТОЯНИЕ НА МЕСТЕ!

    // эта процедура принимает работу от процедуры старта
    // если пика глохнет (расст перестаёт меняться), прога переходит
    // обратно к процедуре старта
    // если подъёзжаем и скорость меньше Q_BOUND_DOWN надо будет останавливаться, то
    // прога переходит к процедуре остановки
    // будем корректировать скорость каждые 100мс (для начала)
    // здесь (надеемся) зона линейной Вольт-оборотной характеристики 
    // в идеале - держать скорость пропорционально расстоянию
    // требуемый конечный промах - 20мм.
    // (пусть) минимальная скорость соотв q=20
    // тогда q будет численно равен рассогл. по R в мм
    // если рассогл по R больше 100мм, то максимальной
    // потом сделать поправку на инерцию!
    // сделать это вводом отрицательного слагаемого в расчёт q

    // линейная область соотв q=[Q_BOUND_DOWN, 100]

    if ((dist_to_corr_mm > -DEAD_ZONE_HALF_MM) && (dist_to_corr_mm < DEAD_ZONE_HALF_MM)){
        return -2;  // флаг "приехали"
    }

    int32_t new_q_inf_accel;
    int32_t new_q;
    int32_t q_diff;
    int32_t abs_dist_to_corr_mm = my_abs_int32_t(dist_to_corr_mm);

    if (abs_dist_to_corr_mm <= Q_BOUND_DOWN){
        return -1;  // flag to proceed to stop procedure (lol)
    }  // исключили случай конца маршрута. Дальше точно знаем, что мы ещё в пути.
    if (abs_dist_to_corr_mm >= 100){
        new_q_inf_accel = 100;
    }
    else{
        new_q_inf_accel = abs_dist_to_corr_mm;  // скорость проп. рассогл по R
        // случай малых q уже был исключён ранее!
        // поэтому условие малости уже не проверяем!
    }
    // ---------- применение ограничения на по скорости -----------------
    if (new_q_inf_accel >= software_max_q){
        new_q_inf_accel = software_max_q;
    }
    // ---------- применение ограничения на ускорение от юзера -----------------
    new_q = new_q_inf_accel;  // предварительное неогр. значение скорости
    q_diff = new_q_inf_accel - previous_q;  // приращение q (ускорение)
    if (q_diff > software_max_q_diff_accel){  // ограничиваем разгон
        new_q = previous_q + software_max_q_diff_accel;  // q c учётом огр. на ускор.
    }
    if (q_diff < software_max_q_diff_deccel){  // ограничиваем ториожение
        new_q = previous_q + software_max_q_diff_deccel;  // ЗНАК УЧТЁН!        
    }
    // ------------- проверка на мёртвую зону и направление движения -------------
    if (dist_to_corr_mm > DEAD_ZONE_HALF_MM){  // проверка на мёртвую зону
        right_motor_gear_forward();
        left_motor_gear_forward();
    }
    if (dist_to_corr_mm < -DEAD_ZONE_HALF_MM){
        right_motor_gear_reverse();
        left_motor_gear_reverse();
    }    
    // ------------- ШИМ на драйвер------------------
    pwm_to_motor(12, 300, new_q);  // УМЕНЬШИТЬ ДЛИТЕЛЬНОСТЬ ОБРАТНО! 
    // длительностью этого ШИМА также определяет период коррекции в тупом режиме

    // ------------- возвращаемое значение ------------------
    return new_q;  // флагов не кидаем, остаёмся на линейном участке
}


void drive_slowing_to_stop_profile(uint8_t move_dir){  // NEED CHECKING!
    // когда уже точно приближаемся к концу маршрута
    // порог активации этой функции по q (Q_BOUND_DOWN)
    // будет ниже, чем порог "троганья с места" (Q_BOUND_UP)
    // и скорость изменения q д.б. плавнее, чем в функ. старта

    if (move_dir == 1){  // вперёд
        right_motor_gear_forward();
        left_motor_gear_forward();
    }
    if (move_dir == 2){  // наазад
        right_motor_gear_reverse();
        left_motor_gear_reverse();
    }

    for (uint16_t q=Q_BOUND_DOWN; q>=5; q-=5){
        pwm_to_motor(12, 100, q);
    }
}


// -----------------------------------------------------------------------------
void fixed_drive_profile_test(){
    // коррекции НЕТ
    // демонстрация начального и конечного участков

    drive_starting_profile(1);  // вперёд
    drive_slowing_to_stop_profile(1);  // вперёд
    delay(1, 0);
    drive_starting_profile(2);  // назад
    drive_slowing_to_stop_profile(2);  // назад
    delay(1, 0);
}



int32_t rangefinder_10Hz_corr(
        int32_t target_dist_mm,  // USER RANGE MODE
        uint32_t target_dist_sine_ampl_mm,   // USER RANGE MODE
        uint32_t target_dist_sine_period_secs,   // USER RANGE MODE

        int32_t software_max_q,  // USER MOTOR MODE CONSTRAINT
        int32_t software_max_q_diff_accel,  // USER MOTOR MODE CONSTRAINT
        int32_t software_max_q_diff_deccel,  // USER MOTOR MODE CONSTRAINT

        uint8_t plan_d_mode  // USER MODE FLAG  (0 - off, 1 - on)
    ){

    // ИПМЛЕМЕНТИТЬ ОСТАВШИЕСЯ РЕЖИМЫ!!
    // ОТЛАДИТЬ!
    // коррекция производится 10 раз в секунду. (ВРЕМЕННО - ТОЛЬКО РАЗ В СЕКУНДУ)
    // Да, двигатели прекращают работу на считанные миллисекунды.
    // Это не повлияет на их работу.
    // потом сгладим их ещё дополнительно

    // возвращает флаги:
    // 0   -->  успешное завершение
    // -1  -->  ошибка

    int32_t meas_dist_mm = sonar_meas();
    int32_t dist_to_corr_mm = meas_dist_mm - target_dist_mm;
    int32_t previous_q = 1;  // initial value
    uint8_t move_dir;
    int32_t current_q_or_flag;
    int32_t current_q;
    int32_t current_q_diff;
    int32_t current_acceleration;
    char str_1[7];  // for lcd
    char str_2[7];  // for lcd

    // ------------ выбор направления НАЧАЛА движения ------------
    if ((dist_to_corr_mm > -DEAD_ZONE_HALF_MM) && (dist_to_corr_mm < DEAD_ZONE_HALF_MM)){
        return 0;  // уже на месте. никуда не надо ехать
    }
    if (dist_to_corr_mm > 0){
        move_dir = 1;  // вперёд
    }
    else{
        move_dir = 2;  // назад
    }
    drive_starting_profile(move_dir);
    // delay(1, 0)  // TEST!

    // --------- цикл линейной зоны регулирования (с выходами) ----------
    while (1){
        meas_dist_mm = sonar_meas();
        dist_to_corr_mm = meas_dist_mm - target_dist_mm;
        if (dist_to_corr_mm > 0){
            move_dir = 1;  // вперёд
        }
        else{
            move_dir = 2;  // назад
        }
        // lcd_msg(1, 9, "     ");
	    // lcd_msg(2, 9, "     ");
        // lcd_msg(1, 9, itoa(meas_dist_mm, str_1, 10));
        // lcd_msg(2, 9, itoa(dist_to_corr_mm, str_2, 10));
        current_q_or_flag = drive_linear_u_to_v_profile(
            dist_to_corr_mm,  // ЗНАК ВАЖЕН!
            previous_q,
            software_max_q,
            software_max_q_diff_accel,
            software_max_q_diff_deccel
        );
        if (current_q_or_flag == -1){
            drive_slowing_to_stop_profile(move_dir);  // пора тормозить
            return 0;
        }
        if (current_q_or_flag == -2){
            return 0;  // уже приехали, а ещё не тормозили! Нужен хардстоп.
        }
        if (current_q_or_flag == -3){
            drive_starting_profile(move_dir);  // заглохли. надо ещё (минимум) раз стартануь
        }
        current_q = current_q_or_flag;  // флаги уже исключили
        current_q_diff = previous_q - current_q;
        show_guidance_data(
	    	target_dist_mm,
		    meas_dist_mm,
		    dist_to_corr_mm,  // CHECK IT!
		    "LIN",
		    software_max_q,
		    current_q_or_flag,
		    software_max_q_diff_accel,
		    current_q_diff
	    );
        previous_q = current_q;
    }
}



int32_t smooth_rangefinder_1000Hz_corr(){

}



// -----------------------------------------------------------------------------
void kick_forward(){
    right_motor_gear_forward();
    left_motor_gear_forward();
    pwm_to_motor(12, 150, 30);
}

void kick_backward(){
    right_motor_gear_reverse();
    left_motor_gear_reverse();
    pwm_to_motor(12, 150, 30);
}


// -----------------------------------------------------------------------------
/*
void move_platform_trapz(
        int32_t distance_to_go,  // if <0 then move backwards
        uint16_t q_percent_for_max_v,  // proportional to velocity
        uint16_t peak_acceleration
    ){
        // нужно отрабатывать в зависимости от кинематики

        // q_max, dist_in_mm
        //    30,        140
        //    40,        380
        //    50,        700
        //    60,       1040
        //    70,       1420
        //    80,       1830
        //    90,       2240
        //   100,       2750

        if (distance_to_go > 0){
            right_motor_gear_forward();
            left_motor_gear_forward();
        }
        if (distance_to_go < 0){
            right_motor_gear_reverse();
            left_motor_gear_reverse();
        }

        uint16_t eval_distance = 0;  // NEED IMPLEMENTING
        // экспериментально снять график зависимости
        // между этими двумя переменными.

        for (uint16_t k=1; k<=30; k+=10){
            pwm_to_motor(12, 300, k);
        } // разгон до минималки около секунды 
        // ускорение было бы постоянное,
        // если бы не требовался пороговый момент для преодоления трения

        // пока ускорение ХАРДКОДИМ
        for (uint16_t k=30; k<=q_percent_for_max_v; k+=10){
            pwm_to_motor(12, 300, k);
        } // разгон до макималки ПОКА ХАРДКОДИМ


        for (uint16_t k=q_percent_for_max_v; k>=30; k-=10){
            pwm_to_motor(12, 300, k);
        } // торможение с максималки до минималки


        for (uint16_t k=30; k>0; k-=10){
            pwm_to_motor(12, 300, k);
        } // сброс напряжения с минимально скорости (да) до нуля 
    }


void trapz_experiment(){  // only forward
    for (uint16_t q=30; q<=100; q+=10){
        move_platform_trapz(1, q, 0);
        // delay(30, 0);  // время на измерение пройденного пути
        delay(10, 0);
    }

}


void max_voltage_short_time_test(){
    // short var time 
    // const max voltage

    right_motor_gear_forward();
    left_motor_gear_forward();

    pwm_to_motor(12, 50, 100);
    delay(1, 1);
    pwm_to_motor(12, 100, 100);
    delay(1, 1);
    pwm_to_motor(12, 200, 100);
    delay(1, 1);
    pwm_to_motor(12, 300, 100);
    delay(1, 1);

    delay(1, 0);

    right_motor_gear_reverse();
    left_motor_gear_reverse();

    pwm_to_motor(12, 50, 100);
    delay(1, 1);
    pwm_to_motor(12, 100, 100);
    delay(1, 1);
    pwm_to_motor(12, 200, 100);  // забился первый раз
    delay(1, 1);
    pwm_to_motor(12, 300, 100);  // забился первый раз
    delay(1, 1);

    delay(1, 0);
}


void pwm_func_ladder_test(){
    // long time 
    // var voltage

    // вместо переключения
    // направления вращения или 
    // смены мотора,
    // левый мотор совершает
    // кратковременное движение вперёд
    // и программа останавливается.
    // а правый мотор наоборот
    // начинает вращаться без остановки

    right_motor_gear_forward();
    left_motor_gear_forward();
    for (uint16_t k=20; k<=40; k+=2){
        pwm_to_motor(12, 200, k);
    }
    for (uint16_t k=40; k>=20; k-=2){
        pwm_to_motor(12, 200, k);
    }

    delay(1, 1);  // на всякий

    right_motor_gear_reverse();
    left_motor_gear_reverse();
    for (uint16_t k=20; k<=40; k+=2){
        pwm_to_motor(12, 200, k);
    }
    for (uint16_t k=40; k>=20; k-=2){
        pwm_to_motor(12, 200, k);
    }

    delay(1, 1);  // на всякий
}




// -----------------------------------------------------------------------------
void left_mortor_pwm_period_duty_cycle_2(){ // takes 2ms 
    left_motor_enable();
    delay(1, 3);
    left_motor_disable();
    delay(1, 3);
}

void right_mortor_pwm_period_duty_cycle_2(){ // takes 2ms 
    right_motor_enable();
    delay(1, 3);
    right_motor_disable();
    delay(1, 3);
}

void right_mortor_1_sec_pwm_duty_cycle_2(){
    // for(uint32_t i=0; i<n; i++)
    for(uint32_t i=0; i<500; i++){
        right_mortor_pwm_period_duty_cycle_2();
    }
}

void left_mortor_1_sec_pwm_duty_cycle_2(){
    // for(uint32_t i=0; i<n; i++)
    for(uint32_t i=0; i<500; i++){
        left_mortor_pwm_period_duty_cycle_2();
    }
}


// -----------------------------------------------------------------------------
void motors_dumb_test(){
    left_motor_gear_reverse();
    left_motor_enable();
    delay(1, 0);
    left_motor_disable();
    delay(1, 0);

    left_motor_gear_forward();
    left_motor_enable();
    delay(1, 0);
    left_motor_disable();
    delay(1, 0);

    left_motor_gear_reverse();
    left_mortor_1_sec_pwm_duty_cycle_2();
    delay(1, 0);

    left_motor_gear_forward();
    left_mortor_1_sec_pwm_duty_cycle_2();
    delay(1, 0);

    right_motor_gear_reverse();
    right_motor_enable();
    delay(1, 0);
    right_motor_disable();
    delay(1, 0);

    right_motor_gear_forward();
    right_motor_enable();
    delay(1, 0);
    right_motor_disable();
    delay(1, 0);
    
    right_motor_gear_reverse();
    right_mortor_1_sec_pwm_duty_cycle_2();
    delay(1, 0);

    right_motor_gear_forward();
    right_mortor_1_sec_pwm_duty_cycle_2();
    delay(1, 0);
}


void movement_demo(){

}
*/


