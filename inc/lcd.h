#include "stm32f10x.h"

//void pin_output(unsigned char start, unsigned char end);

void lcd_rs_high();
void lcd_rs_low();
void lcd_rw_low();
void lcd_rw_low();
void lcd_e_low();
void lcd_e_low();

void send_bits_type_1(unsigned char data);
void send_bits_type_2(unsigned char data);
void send_in_init_type_1();
void send_in_init_type_2();

void lcd_data(unsigned char data);
void lcd_cmd(unsigned char data);
void lcd_init(void);
void lcd_send(char str[]);
void lcd_msg(unsigned char line_1_2, unsigned char pos_0_16, char msg[]);
void lcd_demo(const uint8_t f_cpu_mhz, const uint32_t baudrate);
// void lcd_demo_old();

void show_guidance_data(
		int32_t target_distance,
		int32_t current_meas_distance,
		int32_t curr_distance_error_from_meas,
		char drive_profile_mode[3],
		int32_t velocity_bound,
		int32_t current_velocity,
		int32_t acceleration_bound,
		int32_t current_acceleration
	);

void show_system_data(  // data list to be completed
		uint8_t system_freq,
		uint16_t baudrate,
		int32_t v2,
		int32_t v3,
		int32_t program_time_secs,
		int32_t v5,
		int32_t v6,
		int32_t v8
	);

void show_project_data(char version_str[8]);

void show_custom_vars(
		char c1[1],
		int32_t v1,
		char c2[1],
		int32_t v2,
		char c3[1],
		int32_t v3,
		char c4[1],
		int32_t v4,
		char c5[1],
		int32_t v5,
		char c6[1],
		int32_t v6,
		char c7[1],
		int32_t v7,
		char c8[1],
		int32_t v8
	);
