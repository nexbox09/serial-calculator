/*
 * stm32lm053xx.h
 *
 *  Created on: Apr 7, 2024
 *      Author: Andres Ramos
 */

#ifndef STM32L053XX_H_
#define STM32L053XX_H_

// Define base addresses for various peripherals and registers in the STM32's memory map
#define PERIPHERAL_BASE_ADDRESS     0x40000000U
#define AHB_BASE_ADDRESS            (PERIPHERAL_BASE_ADDRESS + 0x00020000U)
#define RCC_BASE_ADDRESS            (AHB_BASE_ADDRESS + 0x00001000U)
#define RCC_IOPENR_ADDRESS          (RCC_BASE_ADDRESS + 0x0000002CU)
#define IOPORT_ADDRESS              (PERIPHERAL_BASE_ADDRESS + 0x10000000U)
#define GPIOA_BASE_ADDRESS          (IOPORT_ADDRESS + 0x00000000U)
#define GPIOB_BASE_ADDRESS          (IOPORT_ADDRESS + 0x00000400U)
#define GPIOC_BASE_ADDRESS          (IOPORT_ADDRESS + 0x00000800U)

// Define pointers to structures that represent the registers of the GPIOA, GPIOB, GPIOC, and RCC peripherals
#define GPIOA ((GPIO_RegDef_t*)GPIOA_BASE_ADDRESS)
#define GPIOB ((GPIO_RegDef_t*)GPIOB_BASE_ADDRESS)
#define GPIOC ((GPIO_RegDef_t*)GPIOC_BASE_ADDRESS)
#define RCC ((RCC_RegDef_t*)RCC_BASE_ADDRESS)



// Define the structures representing the GPIO registers
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
} GPIO_RegDef_t;

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
} RCC_RegDef_t;


//Diccionario de los digitos del display
#define cero  0x3F
#define uno   0x06
#define dos   0x5B
#define tres  0x4F
#define cuatro  0x66
#define cinco   0x6D
#define seis   0x7D
#define siete  0x07
#define ocho   0x7F
#define nueve  0x67
#define A 	   0x77
#define B 	   0x7C
#define C 	   0x39
#define D 	   0x4E
#define E 	   0x79
#define F 	   0x71

#define KEY_A_INDEX 3  // Tecla "A" presionada
#define KEY_B_INDEX 7  // Tecla "B" presionada
#define KEY_C_INDEX 11 // Tecla "C" presionada
#define KEY_D_INDEX 15 // Tecla "D" presionada
#define KEY_F_INDEX 14 //Tecla "F" presionada que es el "#"
#define KEY_E_INDEX 12 //Tecla "E" presionada que es el "*"



uint32_t number_to_key(uint32_t key){
	switch (key){
		case 0: return 1;
		case 1: return 2;
		case 2: return 3;
		case 4: return 4;
		case 5: return 5;
		case 6: return 6;
		case 8: return 7;
		case 9: return 8;
		case 10: return 9;
		case 13: return 0;
		default: return 0;
	}
}

uint32_t index_to_key(uint32_t number){
	switch (number){
		case 1: return 0;
		case 2: return 1;
		case 3: return 2;
		case 4: return 4;
		case 5: return 5;
		case 6: return 6;
		case 7: return 8;
		case 8: return 9;
		case 9: return 10;
		case 0: return 13;
		default: return 13;
	}
}


#endif /* STM32L053XX_H_ */
