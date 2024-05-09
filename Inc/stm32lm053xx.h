/*
 * stm32lm053xx.h
 *
 *      Author: hardware
 */

#ifndef STM32LM053XX_H_
#define STM32LM053XX_H_

//PERIPHERAL & BUS BASE ADDRESS
#define PERIPHERAL_BASE_ADDRESS 	0x40000000U
#define AHB_BASE_ADDRESS			(PERIPHERAL_BASE_ADDRESS + 0x00020000U)
#define APB1_BASE_ADDRESS			(PERIPHERAL_BASE_ADDRESS + 0x00000000U)

//RCC BASE ADDRESS
#define RCC_BASE_ADDRESS			(AHB_BASE_ADDRESS + 0x00001000U)
#define RCC_IOPENR_ADDRESS			(RCC_BASE_ADDRESS + 0x0000002CU)

//IOPORT BASE ADDRESS
#define IOPORT_ADDRESS				(PERIPHERAL_BASE_ADDRESS + 0x10000000U)
//GPIO BASE & SPECIFIC ADDRESS
#define GPIOA_BASE_ADDRESS			(IOPORT_ADDRESS + 0x00000000U)
#define GPIOB_BASE_ADDRESS			(IOPORT_ADDRESS + 0x00000400U)
#define GPIOC_BASE_ADDRESS			(IOPORT_ADDRESS + 0x00000800U)

//USART2 BASE ADDRESS
#define USART2_BASE_ADDRESS			(APB1_BASE_ADDRESS + 0x00004400U)

typedef struct
{
	uint32_t MODER;
	uint32_t OTYPER;
	uint32_t OSPEEDR;
	uint32_t PUPDR;
	uint32_t IDR;
	uint32_t ODR;
	uint32_t BSRR;
	uint32_t LCKR;
	uint32_t AFR[2];
	uint32_t BRR;
}GPIO_RegDef_t;

typedef struct
{
	uint32_t CR;
	uint32_t ICSCR;
	uint32_t CRRCR;
	uint32_t CFGR;
	uint32_t CIER;
	uint32_t CIFR;
	uint32_t CICR;
	uint32_t IOPRSTR;
	uint32_t AHBRSTR;
	uint32_t APB2RSTR;
	uint32_t APB1RSTR;
	uint32_t IOPENR;
	uint32_t AHBENR;
	uint32_t APB2ENR;
	uint32_t APB1ENR;
	uint32_t IOPSMENR;
	uint32_t AHBSMENR;
	uint32_t APB2SMENR;
	uint32_t APB1SMENR;
	uint32_t CCIPR;
	uint32_t CSR;
}RCC_RegDef_t;

struct Time_t {   // Structure declaration
	uint8_t hour_decimal ;
	uint8_t hour_unit ;
	uint8_t minute_decimal ;
	uint8_t minute_unit ;
	uint8_t second_decimal ;
	uint8_t second_unit ;
};

typedef struct
{
	uint32_t CR1;
	uint32_t CR2;
	uint32_t CR3;
	uint32_t BRR;
	uint32_t GTPR;
	uint32_t RTOR;
	uint32_t RQR;
	uint32_t ISR;
	uint32_t ICR;
	uint32_t RDR;
	uint32_t TDR;
}USART_RegDef_t;


#define GPIOA  				((GPIO_RegDef_t*)GPIOA_BASE_ADDRESS)
#define GPIOB  				((GPIO_RegDef_t*)GPIOB_BASE_ADDRESS)
#define GPIOC  				((GPIO_RegDef_t*)GPIOC_BASE_ADDRESS)

#define RCC  				((RCC_RegDef_t*)RCC_BASE_ADDRESS)

#define USART2				((USART_RegDef_t*)USART2_BASE_ADDRESS)

//User Macros
#define ENABLE_CLKA()		do{RCC->IOPENR |= 1<<0;}while(0)
#define ENABLE_ALL_IO()		do{RCC->IOPENR |= (1<<0);RCC->IOPENR |= 1<<1;/*etc etc*/}while(0)
//SEGMENT CC DEFINITION


#endif /* STM32LM053XX_H_ */
