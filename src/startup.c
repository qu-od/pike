#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"


/*
// -----------SET CLK TO HSE DIRECTLY -----------------------------
#ifdef FCPU_HSE
  // * @brief  Selects HSE as System clock source and configure HCLK, PCLK2
  // *         and PCLK1 prescalers.
  // * @note   This function should be used only after reset.
  // * @param  None
  // * @retval None
static void SetSysClockToHSE_Directly(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

  // SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------
  // Enable HSE
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);

  //  Wait till HSE is ready and if Time out is reached exit
  do
  {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CR & RCC_CR_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }

  if (HSEStatus == (uint32_t)0x01)
  {

#if !defined STM32F10X_LD_VL && !defined STM32F10X_MD_VL && !defined STM32F10X_HD_VL
    // Enable Prefetch Buffer
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    // Flash 0 wait state
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);

#ifndef STM32F10X_CL
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_0;
#else
    if (HSE_VALUE <= 24000000)
    {
      FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_0;
    }
    else
    {
      FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_1;
    }
#endif // STM32F10X_CL 
#endif

    // HCLK = SYSCLK
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

    // PCLK2 = HCLK
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;

    // PCLK1 = HCLK
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV1;

    // Select HSE as system clock source
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSE;

    // Wait till HSE is used as system clock source
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x04)
    {
    }
  }
  else
  { // If HSE fails to start-up, the application will have wrong clock 
       //  configuration. User can add here some code to deal with this error
  }
}
#endif
*/




// ----------------- SYS CLK TO REQUIRED FREQ ------------------------

#ifdef FCPU_8 /*  PLL configuration: PLLCLK = HSE * 1 = 8 MHz */
SetSysClockToHSE_Directly();  // uncomment func def later
#endif

/*
  Sets System clock frequency to desirable freq and configure HCLK, PCLK2 
  and PCLK1 prescalers.
  This function should be used only after reset.
  // allowed cpu_freq_in_MHz values: 16, 24, 32, 40, 48, 56, 64, 72
*/
void SetSysClockToReqFreq(uint8_t cpu_freq_in_MHz){
    
    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/
    /* Enable HSE */
    // включение внешнего кварцевого генератора
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);

    /* Wait till HSE is ready and if Time out is reached exit */
    // ожидание готовности внешнего кварцевого генератора
    while (!(RCC->CR & RCC_CR_HSERDY))

    /* Enable Prefetch Buffer */ // активировать память программ
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    /* Flash 2 wait state */ // память программ в состояние ожидания
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;

    /* HCLK = SYSCLK */
    // тактировка системной шины
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

    /* PCLK2 = HCLK */
    // тактировка шины периферии 2
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;

    /* PCLK1 = HCLK */
    // тактировка шины периферии 1
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;

    // формирование маски для регистра множителя частоты
    uint8_t req_pll_mull = cpu_freq_in_MHz >> 3;
    // 2 to 9 allowed  (16 to 72MHz)
    uint32_t pll_mull_ones_mask = ((uint32_t)(req_pll_mull - 2)) << 18;

    /*
    uint32_t pll_mull_2  = (uint32_t)0x00000000;  // 0000
    uint32_t pll_mull_3  = (uint32_t)0x00040000;  // 0001
    uint32_t pll_mull_4  = (uint32_t)0x00080000;  // 0010
    uint32_t pll_mull_5  = (uint32_t)0x000C0000;  // 0011
    uint32_t pll_mull_6  = (uint32_t)0x00100000;  // 0100
    uint32_t pll_mull_7  = (uint32_t)0x00140000;  // 0101
    uint32_t pll_mull_8  = (uint32_t)0x00180000;  // 0110
    uint32_t pll_mull_9  = (uint32_t)0x001C0000;  // 0111
    uint32_t pll_mull_10 = (uint32_t)0x00200000;  // 1000
    */

    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE |
                                        RCC_CFGR_PLLMULL));
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | pll_mull_ones_mask);

    /*
    // FCPU_16  PLL configuration: PLLCLK = HSE * 2 = 16 MHz
    // FCPU_24  PLL configuration: PLLCLK = HSE * 3 = 24 MHz
    // FCPU_32  PLL configuration: PLLCLK = HSE * 4 = 32 MHz
    // FCPU_40  PLL configuration: PLLCLK = HSE * 5 = 40 MHz
    // FCPU_48  PLL configuration: PLLCLK = HSE * 6 = 48 MHz
    // FCPU_56  PLL configuration: PLLCLK = HSE * 7 = 56 MHz
    // FCPU_64  PLL configuration: PLLCLK = HSE * 8 = 64 MHz
    // FCPU_72  PLL configuration: PLLCLK = HSE * 9 = 72 MHz
    */

    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till PLL is ready */
    while(!(RCC->CR & RCC_CR_PLLRDY));

    /* Select PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08);


    
    #ifdef VECT_TAB_SRAM  // костыльная вставка из systemInit()!
        SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET;
        /* Vector Table Relocation in Internal SRAM. */
    #else
    //  SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET;
    /* Vector Table Relocation in Internal FLASH. */
    #endif
}





// --------------------- SYSTEM INIT (called at reset) -----------------------------
void SystemInit(void)
{
  RCC->CR |= (uint32_t)0x00000001;  // enable HSI, 8 MHz

  /* Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits */
#ifndef STM32F10X_CL
  RCC->CFGR &= (uint32_t)0xF8FF0000;
#else
  RCC->CFGR &= (uint32_t)0xF0FF0000;
#endif /* STM32F10X_CL */

  /* Reset HSEON, CSSON and PLLON bits */
  RCC->CR &= (uint32_t)0xFEF6FFFF;

  /* Reset HSEBYP bit */
  RCC->CR &= (uint32_t)0xFFFBFFFF;

  /* Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits */
  RCC->CFGR &= (uint32_t)0xFF80FFFF;

#ifdef STM32F10X_CL
  /* Reset PLL2ON and PLL3ON bits */
  RCC->CR &= (uint32_t)0xEBFFFFFF;

  /* Disable all interrupts and clear pending bits  */
  RCC->CIR = 0x00FF0000;

  /* Reset CFGR2 register */
  RCC->CFGR2 = 0x00000000;
#elif defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || (defined STM32F10X_HD_VL)
  /* Disable all interrupts and clear pending bits  */
  RCC->CIR = 0x009F0000;

  /* Reset CFGR2 register */
  RCC->CFGR2 = 0x00000000;
#else
  /* Disable all interrupts and clear pending bits  */
  RCC->CIR = 0x009F0000;
#endif /* STM32F10X_CL */

#if defined (STM32F10X_HD) || (defined STM32F10X_XL) || (defined STM32F10X_HD_VL)
  #ifdef DATA_IN_ExtSRAM
    SystemInit_ExtMemCtl();
  #endif /* DATA_IN_ExtSRAM */
#endif

  /* Configure the System clock frequency, HCLK, PCLK2 and PCLK1 prescalers */
  /* Configure the Flash Latency cycles and enable prefetch buffer */
//  SetSysClock(); 


}