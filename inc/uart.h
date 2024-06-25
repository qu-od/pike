#ifndef __UART_H__
#define __UART_H__

#include "stm32f10x.h"
#include "stm32f10x_usart.h"

#define USART_CR1_UE_Set                ((uint16_t)0x2000)  /*!< USART Enable Mask */
#define USART_CR1_UE_Reset              ((uint16_t)0xDFFF)  /*!< USART Disable Mask */

#define USART_CR1_WAKE_Mask             ((uint16_t)0xF7FF)  /*!< USART WakeUp Method Mask */

#define USART_CR1_RWU_Set               ((uint16_t)0x0002)  /*!< USART mute mode Enable Mask */
#define USART_CR1_RWU_Reset             ((uint16_t)0xFFFD)  /*!< USART mute mode Enable Mask */
#define USART_CR1_SBK_Set               ((uint16_t)0x0001)  /*!< USART Break Character send Mask */
#define USART_CR1_CLEAR_Mask            ((uint16_t)0xE9F3)  /*!< USART CR1 Mask */
#define USART_CR2_Address_Mask          ((uint16_t)0xFFF0)  /*!< USART address Mask */

#define USART_CR2_LINEN_Set              ((uint16_t)0x4000)  /*!< USART LIN Enable Mask */
#define USART_CR2_LINEN_Reset            ((uint16_t)0xBFFF)  /*!< USART LIN Disable Mask */

#define USART_CR2_LBDL_Mask             ((uint16_t)0xFFDF)  /*!< USART LIN Break detection Mask */
#define USART_CR2_STOP_CLEAR_Mask       ((uint16_t)0xCFFF)  /*!< USART CR2 STOP Bits Mask */
#define USART_CR2_CLOCK_CLEAR_Mask      ((uint16_t)0xF0FF)  /*!< USART CR2 Clock Mask */

#define USART_CR3_SCEN_Set              ((uint16_t)0x0020)  /*!< USART SC Enable Mask */
#define USART_CR3_SCEN_Reset            ((uint16_t)0xFFDF)  /*!< USART SC Disable Mask */

#define USART_CR3_NACK_Set              ((uint16_t)0x0010)  /*!< USART SC NACK Enable Mask */
#define USART_CR3_NACK_Reset            ((uint16_t)0xFFEF)  /*!< USART SC NACK Disable Mask */

#define USART_CR3_HDSEL_Set             ((uint16_t)0x0008)  /*!< USART Half-Duplex Enable Mask */
#define USART_CR3_HDSEL_Reset           ((uint16_t)0xFFF7)  /*!< USART Half-Duplex Disable Mask */

#define USART_CR3_IRLP_Mask             ((uint16_t)0xFFFB)  /*!< USART IrDA LowPower mode Mask */
#define USART_CR3_CLEAR_Mask            ((uint16_t)0xFCFF)  /*!< USART CR3 Mask */

#define USART_CR3_IREN_Set              ((uint16_t)0x0002)  /*!< USART IrDA Enable Mask */
#define USART_CR3_IREN_Reset            ((uint16_t)0xFFFD)  /*!< USART IrDA Disable Mask */
#define USART_GTPR_LSB_Mask             ((uint16_t)0x00FF)  /*!< Guard Time Register LSB Mask */
#define USART_GTPR_MSB_Mask             ((uint16_t)0xFF00)  /*!< Guard Time Register MSB Mask */
#define USART_IT_Mask                   ((uint16_t)0x001F)  /*!< USART Interrupt Mask */

/* USART OverSampling-8 Mask */
#define CR1_OVER8_Set             ((u16)0x8000)  /* USART OVER8 mode Enable Mask */
#define CR1_OVER8_Reset           ((u16)0x7FFF)  /* USART OVER8 mode Disable Mask */

/* USART One Bit Sampling Mask */
#define CR3_ONEBITE_Set           ((u16)0x0800)  /* USART ONEBITE mode Enable Mask */
#define CR3_ONEBITE_Reset         ((u16)0xF7FF)  /* USART ONEBITE mode Disable Mask */

void usart1_print_number(uint32_t num);
void usart1_send_char(uint32_t ch);
void usart1_print_string(char *str);

void usart1_send_char_8bit(uint8_t ch);
void sc(char c);
void usart1_send_string_of_30_chars(
    char c01, char c02, char c03, char c04, char c05,
    char c06, char c07, char c08, char c09, char c10,
    char c11, char c12, char c13, char c14, char c15,
    char c16, char c17, char c18, char c19, char c20,
    char c21, char c22, char c23, char c24, char c25,
    char c26, char c27, char c28, char c29, char c30
    );

#endif