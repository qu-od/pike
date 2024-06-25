#include "stm32f10x_gpio.h"

#include "lcd.h"
#include "wheel.h"


void lcd_rs_high(){
	GPIOB -> BSRR = GPIO_Pin_13;  // поставили 1
}

void lcd_rs_low(){
	GPIOB -> BRR =  GPIO_Pin_13;  // поставили 0
}

void lcd_rw_high(){
	GPIOB -> BSRR = GPIO_Pin_14;
}

void lcd_rw_low(){
	GPIOB -> BRR =  GPIO_Pin_14;
}

void lcd_e_high(){
	GPIOB -> BSRR = GPIO_Pin_15;
}

void lcd_e_low(){
	GPIOB -> BRR =  GPIO_Pin_15;
}



void send_bits_type_1(unsigned char data){
	GPIOA -> ODR &= 0xff0f;
	GPIOA -> ODR |= (data & 0x00f0);
}

void send_bits_type_2(unsigned char data){
	GPIOA -> ODR &= 0xff0f;
	GPIOA -> ODR |= ((data << 4) & 0x00f0);
}

void send_in_init_type_1(){
	GPIOA -> ODR &= 0xff0f;
	GPIOA -> ODR |= 0x30; // 8 bit communication mode 
}

void send_in_init_type_2(){
	GPIOA -> ODR &= 0xff0f;
	GPIOA -> ODR |= 0x20; // 4 bit communication mode 
}




void lcd_data(unsigned char data)
{
	//pin_output(4,7); // set it up in main

	lcd_rs_high();
	lcd_rw_low();
	delay(1, 5);
	lcd_e_high();
	delay(1, 5);
	send_bits_type_1(data);
	delay(1, 5);
	lcd_e_low();
	
	delay(1, 5);
	
	lcd_e_high();
	delay(1, 5);
	send_bits_type_2(data);
	delay(1, 5);
	lcd_e_low();
}


void lcd_cmd(unsigned char data)
{
	// pin_output(0,11); // set it up in main
	lcd_rs_low();
	lcd_rw_low();
	delay(1, 5);
	lcd_e_high();
	delay(1, 5);
	send_bits_type_1(data);
	delay(1, 5);
	lcd_e_low();
	
	delay(1, 5);
	
	lcd_e_high();
	delay(1, 5);
	send_bits_type_2(data);
	delay(1, 5);
	lcd_e_low();
}

void lcd_init(void)
{
	delay(2, 2);

	lcd_rs_low();
	lcd_rw_low();
	delay(1, 3);  // 10us should be!!
	lcd_e_high();
	delay(1, 3);  // 5us should be!!
	send_in_init_type_1();
	delay(1, 3);  // 10us should be!!
	lcd_e_low();
	
	delay(1, 3);
	
	lcd_rs_low();
	lcd_rw_low();
	delay(1, 3);  // 10us should be!!
	lcd_e_high();
	delay(1, 3);  // 5us should be!!
	send_in_init_type_2();
	delay(1, 3);  // 10us should be!!
	lcd_e_low();
	
	lcd_cmd(0x2C); // 4 bit communication mode / 2 lines
	delay(5, 3);
	lcd_cmd(0x0C); // Display ON
	delay(5, 3);
	lcd_cmd(0x01); // Clear Display
	delay(5, 3);
	lcd_cmd(0x02); // Get back to initial address
	delay(5, 3);
}


void lcd_send(char str[])
{
	int i = 0;
		while(str[i])
		{
			lcd_data(str[i]);
			i++;
			delay(1, 4);
		}
}

void lcd_msg(unsigned char line_1_2, unsigned char pos_0_16, char msg[])
{
	short pos = 0;
	if(line_1_2==1)
	{
		pos = 0;
	}
	else if(line_1_2==2)
	{
		pos = 0x40;
	}
	lcd_cmd(0x80 + pos + pos_0_16);
	delay(1, 4);
	lcd_send(msg);
}



// --------------------------------------------------------------
void lcd_demo(const uint8_t f_cpu_mhz, const uint32_t baudrate){
    // --- LCD setup ---
    /*
		Pin setup : 4 bits data communication

		PA8  -> RS
		PB10 -> RW
		PB11 -> E

		PA4 -> DB4
		PA5 -> DB5
		PA6 -> DB6
		PA7 -> DB7
    */	
	//Digital_Input(PA,11); // Initialize push button

	char string_for_f_cpu[2]; 
	char string_for_baudrate[6];

	lcd_init(); // Initialize the LCD screen to work with 4 bits data interface
    // lcd_msg(Line_number 1 or 2, Position within the line, String to be shown)
	lcd_msg(1, 1,"Rahimov, Raguzov");
	lcd_msg(2, 6,"RL1-114");
	delay(3, 1);
	lcd_msg(1,0, "Pike RSKU:   ");
	delay(3, 1);
	lcd_msg(2,0, "Ver ______:      ");  // PARAMETRIZE!
	delay(3, 1);
    lcd_msg(1,0, "CPU_FREQ = __MHz");
	delay(3, 1);
    lcd_msg(1,11, itoa(f_cpu_mhz, string_for_f_cpu, 10));
	delay(3, 1);
	lcd_msg(2,0, "INTRP_FREQ = 4Hz");
	delay(3, 1);
    lcd_msg(1,0, "BAUDRATE =");	
	delay(3, 1);
    lcd_msg(1,10, itoa(baudrate, string_for_baudrate, 10));
	delay(3, 1);
	lcd_msg(2,0, "Interrupts: ");
	delay(3, 1);
    lcd_msg(2,12, "0   ");
	delay(3, 1);
}


// --------------------------------------------------------------
void show_guidance_data(
		int32_t target_distance,
		int32_t current_meas_distance,
		int32_t curr_distance_error_from_meas,
		char drive_profile_mode[3],
		int32_t velocity_bound,
		int32_t current_velocity,
		int32_t acceleration_bound,
		int32_t current_acceleration
	){  // пока стесняюсь работать с массивами.
	// for (uint8_t s=0; s<=16; s+=4);

	char s1[3];
	char s2[3];
	char s3[3];
	char s4[3];

	char s5[3];
	char s6[3];
	char s7[3];

	lcd_msg(1, 0, "v");
	lcd_msg(1, 1, itoa(current_velocity, s1, 10));
	lcd_msg(1, 4, "a");
	lcd_msg(1, 5, itoa(current_acceleration, s2, 10));

	lcd_msg(1, 8, "r");
	lcd_msg(1, 9, itoa(current_meas_distance, s3, 10));
	lcd_msg(1, 12, "d");
	lcd_msg(1, 13, itoa(curr_distance_error_from_meas, s4, 10));

	lcd_msg(2, 0, "V");
	lcd_msg(2, 1, itoa(velocity_bound, s5, 10));
	lcd_msg(2, 4, "A");
	lcd_msg(2, 5, itoa(acceleration_bound, s6, 10));

	lcd_msg(2, 8, "R");
	lcd_msg(2, 9, itoa(target_distance, s7, 10));
	lcd_msg(2, 12, "M");
	lcd_msg(2, 13, drive_profile_mode);
}



void show_system_data(  // data list to be completed
		uint8_t system_freq_MHz,  // как itoa это переварит?
		uint16_t baudrate_kbod,  // как itoa это переварит?
		int32_t v3,
		int32_t v4,
		int32_t program_time_secs,  // TO BE IMPLEMENTED
		int32_t v6,
		int32_t v7,
		int32_t v8
	){  // пока стесняюсь работать с массивами.
	// for (uint8_t s=0; s<=16; s+=4);

	char s1[3];
	char s2[3];
	char s3[3];
	char s4[3];

	char s5[3];
	char s6[3];
	char s7[3];
	char s8[3];

	lcd_msg(1, 0, "F");
	lcd_msg(1, 1, itoa(system_freq_MHz, s1, 10));
	lcd_msg(1, 4, "B");
	lcd_msg(1, 5, itoa(baudrate_kbod, s2, 10));

	lcd_msg(1, 8, "O");
	lcd_msg(1, 9, itoa(v3, s3, 10));
	lcd_msg(1, 12, "O");
	lcd_msg(1, 13, itoa(v4, s4, 10));

	lcd_msg(2, 0, "T");
	lcd_msg(2, 1, itoa(program_time_secs, s5, 10));
	lcd_msg(2, 4, "O");
	lcd_msg(2, 5, itoa(v6, s6, 10));

	lcd_msg(2, 8, "O");
	lcd_msg(2, 9, itoa(v7, s7, 10));
	lcd_msg(2, 12, "O");
	lcd_msg(2, 13, itoa(v8, s8, 10));
}


void show_project_data(
		char version_str[8]
	){  // пока стесняюсь работать с массивами.
	// for (uint8_t s=0; s<=16; s+=4);
	lcd_msg(1, 0, "Rahimov ");
	lcd_msg(1, 8, " Raguzov");
	lcd_msg(2, 0, "PikeRSKU");
	lcd_msg(2, 8, version_str);
}


// --------------------------------------------------------------
void show_data_dummy_func(
		int32_t v1,
		int32_t v2,
		int32_t v3,
		int32_t v4,
		int32_t v5,
		int32_t v6,
		int32_t v7,
		int32_t v8
	){  // пока стесняюсь работать с массивами.
	// for (uint8_t s=0; s<=16; s+=4);

	char s1[3];
	char s2[3];
	char s3[3];
	char s4[3];

	char s5[3];
	char s6[3];
	char s7[3];
	char s8[3];

	lcd_msg(1, 0, "O");
	lcd_msg(1, 1, itoa(v1, s1, 10));
	lcd_msg(1, 4, "O");
	lcd_msg(1, 5, itoa(v2, s2, 10));

	lcd_msg(1, 8, "O");
	lcd_msg(1, 9, itoa(v3, s3, 10));
	lcd_msg(1, 12, "O");
	lcd_msg(1, 13, itoa(v4, s4, 10));

	lcd_msg(2, 0, "O");
	lcd_msg(2, 1, itoa(v5, s5, 10));
	lcd_msg(2, 4, "O");
	lcd_msg(2, 5, itoa(v6, s6, 10));

	lcd_msg(2, 8, "O");
	lcd_msg(2, 9, itoa(v7, s7, 10));
	lcd_msg(2, 12, "O");
	lcd_msg(2, 13, itoa(v8, s8, 10));
}



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
	){  // пока стесняюсь работать с массивами.
	// for (uint8_t s=0; s<=16; s+=4);

	char s1[3];
	char s2[3];
	char s3[3];
	char s4[3];

	char s5[3];
	char s6[3];
	char s7[3];
	char s8[3];

	lcd_msg(1, 0, c1);
	lcd_msg(1, 1, itoa(v1, s1, 10));
	lcd_msg(1, 4, c2);
	lcd_msg(1, 5, itoa(v2, s2, 10));

	lcd_msg(1, 8, c3);
	lcd_msg(1, 9, itoa(v3, s3, 10));
	lcd_msg(1, 12, c4);
	lcd_msg(1, 13, itoa(v4, s4, 10));

	lcd_msg(2, 0, c5);
	lcd_msg(2, 1, itoa(v5, s5, 10));
	lcd_msg(2, 4, c6);
	lcd_msg(2, 5, itoa(v6, s6, 10));

	lcd_msg(2, 8, c7);
	lcd_msg(2, 9, itoa(v7, s7, 10));
	lcd_msg(2, 12, c8);
	lcd_msg(2, 13, itoa(v8, s8, 10));
}

