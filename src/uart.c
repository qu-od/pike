#include "uart.h"
#include "wheel.h"
#define len 8


// ------------------- LIB FUNCS ------------------------
void usart1_print_number(uint32_t num)
{
    uint8_t n[len];
    char *s=n+(len-1);
    *s=0;           // EOL
    do {
        *(--s) = (uint32_t)(num%10 + 0x30);
        num=num/10;
    } while (num > 0);
    usart1_print_string(s);
}


void usart1_send_char(uint32_t ch)
{
    USART1->DR=ch;
    while(!(USART1->SR & USART_FLAG_TXE));
}


void usart1_print_string(char *str)
{
    while (*str)
    {
        usart1_send_char((uint32_t)*str++);
    }
}


// ------------------- FUNCS ------------------------
void usart1_send_char_8bit(uint8_t ch)
{
    USART1 -> DR = ch;
    while(!(USART1 -> SR & USART_FLAG_TXE));
}


void sc(char c)
{
    usart1_send_char_8bit((uint8_t)(c));
    delay(1, 3);
}


void usart1_send_string_of_30_chars(
    char c01, char c02, char c03, char c04, char c05,
    char c06, char c07, char c08, char c09, char c10,
    char c11, char c12, char c13, char c14, char c15,
    char c16, char c17, char c18, char c19, char c20,
    char c21, char c22, char c23, char c24, char c25,
    char c26, char c27, char c28, char c29, char c30
    ) // cyrrilics not supported yet
{
    // ПЕРЕДАЧА СТРОКИ В ЦИКЛЕ РАБОТАЕТ НЕКОРРЕКТНО!
    // В этом случае на ПК приходят случайные символы
    // параметризация не удалась. странно
    sc(c01);
    sc(c02);
    sc(c03);
    sc(c04);
    sc(c05);
    sc(c06);
    sc(c07);
    sc(c08);
    sc(c09);
    sc(c10);
    sc(c11);
    sc(c12);
    sc(c13);
    sc(c14);
    sc(c15);
    sc(c16);
    sc(c17);
    sc(c18);
    sc(c19);
    sc(c20);
    sc(c21);
    sc(c22);
    sc(c23);
    sc(c24);
    sc(c25);
    sc(c26);
    sc(c27);
    sc(c28);
    sc(c29);
    sc(c30);
}



// ---------------------------- UNSUCC TEST ------------------------------
/*
void usart1_send_test_msg()  // работает отлично
{
    send_symbol_and_delay_test((uint8_t)(0xEE));  // о
    send_symbol_and_delay_test((uint8_t)(0xF2));  // т
    send_symbol_and_delay_test((uint8_t)(0xEB));  // л
    send_symbol_and_delay_test((uint8_t)(0x21));  // !
    send_symbol_and_delay_test((uint8_t)(0x0D));  // carriage return
}



char test_string[5] = "otl!\n";

void usart1_send_char_array_test(char stri[]) // NEED TETSING!
{
    
    send_symbol_and_delay_test((uint8_t)(stri[0])); // PROBLEM ARISES!
    send_symbol_and_delay_test((uint8_t)(stri[1])); // RANDOM BITS!!
    send_symbol_and_delay_test((uint8_t)(stri[2]));
    send_symbol_and_delay_test((uint8_t)(stri[3]));
    send_symbol_and_delay_test((uint8_t)(stri[4]));
    
    
    while (*chars) // PROBLEM ARISES!
    {
        send_symbol_and_delay_test((uint8_t)*chars++);
    }
    
}

void usart1_send_test_msg_in_string()
{
    usart1_send_char_array_test(test_string);
}
*/



