#include <stdint.h>
#include "stm32lm053xx.h"
#include "diccionario.h"


void delay_ms(uint16_t n);
void actualizar_valores_display(void);
uint32_t decodificador(uint32_t value_to_decode);
void decodificador_decimal(uint32_t value);

uint32_t primer_digito = 0xFF; // Primer dígito para mostrar
uint32_t segundo_digito_display = 0xFF; // Segundo dígito para mostrar
uint32_t tercer_digito_display = 0xFF; // Tercer dígito para mostrar
uint32_t valor_almacenado = 0; // Variable para almacenar el valor mostrado en los displays

// Añadir nuevas variables globales para la entrada serial
static char serial_buffer[10];  // Buffer para almacenar entrada serial
static uint8_t serial_index = 0; // Índice para el buffer

const uint32_t tecla_a_mostrar[] = {
    uno, dos, tres, A, // Columna 1: 1, 2, 3, A
    cuatro, cinco, seis, B, // Columna 2: 4, 5, 6, B
    siete, ocho, nueve, C, // Columna 3: 7, 8, 9, C
    E, cero, F, D // Columna 4: *, 0, #, D
};

uint32_t get_number_from_key(uint32_t key) {
    // Tabla de mapeo de teclas a números
    static const uint8_t key_map[] = {
        1, 2, 3, 0xFF,  // 0xFF significa 'no válido'
        4, 5, 6, 0xFF,
        7, 8, 9, 0xFF,
        0xFF, 0, 0xFF, 0xFF
    };

    // Asegúrate de que la clave está dentro del rango del array
    if (key < sizeof(key_map)) {
        uint32_t number = key_map[key];
        return (number == 0xFF ? 0 : number);  // Retorna 0 si la tecla no es válida
    }
    return 0;  // Retorna 0 si la clave está fuera de rango
}

uint32_t get_key_from_number(uint32_t number) {
    // Tabla de mapeo de números a índices de teclas
    static const uint8_t number_to_key_map[10] = {
        13,  // 0
        0,   // 1
        1,   // 2
        2,   // 3
        4,   // 4
        5,   // 5
        6,   // 6
        8,   // 7
        9,   // 8
        10   // 9
    };

    // Asegúrate de que el número está dentro del rango de la tabla
    if (number < sizeof(number_to_key_map)) {
        return number_to_key_map[number];
    }
    return 0xFF;  // Retorna 0xFF si el número está fuera de rango
}

int main(void)
{

    // Habilitar el reloj para GPIOC y GPIOB
    RCC->IOPENR |= (1 << 2) | (1 << 1) | (1 << 0);

//////////////////////////////////////////////////////
////////////// HABILITAR USART2 //////////////////////
//////////////////////////////////////////////////////

    	//Enable HSI 16MHz
		//HSI on
	RCC->CR |= (1<<0);
		//HSI16 as SYSCLK
	RCC->CFGR |= (1<<0);


	//Enable CLK
		//For the Peripherals under use via RCC reg
		//PORTA
	RCC->IOPENR |= 1<<0;


	//USART 2 pin configuration
		//Alternate Function MODE for PA2 and PA3
	GPIOA->MODER &= ~(1<<4);//PA2 as AF, MODER [10]
	GPIOA->MODER &= ~(1<<6);//PA3 as AF, MODER [10]
		//Select which MODER, AF4 for USART2, TX on PA2 :: RX on PA3
	GPIOA->AFR[0] |= 1<<10; //PA2 mapped as AF4
	GPIOA->AFR[0] |= 1<<14; //PA3 mapped as AF4

	//USART2 peripheral configuration
		//USART2 clock enable on RCC APB1ENR register
	RCC->APB1ENR |= 1<<17;
		//Baud Rate calculation @9600 with 16Mhz
	USART2->BRR = 1667;
		//Enable TX and RX
	USART2->CR1 |= (1<<2)|(1<<3);
		//Enable Peripheral
	USART2->CR1 |= 1<<0;

/////////////////////////////////////////////////////////////////////
/////////////////// Habilitar salidas para displays      ////////////
/////////////////// y entradas para el teclado matricial ////////////
/////////////////////////////////////////////////////////////////////

    // Configurar los pines de GPIOC como salida para los displays
    uint32_t portC_masks = (0b01 << 8) | (0b01 << 10) | (0b01 << 12) | (0b01 << 14) | (0b01 << 16) | (0b01 << 18);
    GPIOC->MODER &= ~(0b11 << 8 | 0b11 << 10 | 0b11 << 12 | 0b11 << 14 | 0b11 << 16 | 0b11 << 18);
    GPIOC->MODER |= portC_masks;



    // Configurar los pines de GPIOB como salida para los segmentos de los displays
    GPIOB->MODER &= ~(0xFFFF); // Limpiar los modos de los primeros 8 pines (pines 0 a 7)
    uint32_t portB_display_masks = (0b01 << 0) | (0b01 << 2) | (0b01 << 4) | (0b01 << 6) |
                                   (0b01 << 8) | (0b01 << 10) | (0b01 << 12) | (0b01 << 14);
    GPIOB->MODER |= portB_display_masks; // Establecer los pines del display como salida

    // Configurar los pines de GPIOB como salida para controlar las columnas de la matriz del teclado
    GPIOB->MODER &= ~((0b11 << 24) | (0b11 << 26) | (0b11 << 28) | (0b11 << 30)); // Limpiar la configuración actual para los pines 12 a 15
    GPIOB->MODER |= ((0b01 << 24) | (0b01 << 26) | (0b01 << 28) | (0b01 << 30)); // Establecer los pines PB12 a PB15 como salidas

    // Configurar los pines del 16 al 22 de GPIOB con resistencias pull-up
    GPIOB->PUPDR &= ~((0b11 << 16) | (0b11 << 18) | (0b11 << 20) | (0b11 << 22)); // Limpiar configuración de pull-up/pull-down
    GPIOB->PUPDR |= ((0b01 << 16) | (0b01 << 18) | (0b01 << 20) | (0b01 << 22)); // Establecer pull-up para pines de entrada

    // Configurar los pines PB8 a PB11 como entrada (teclado matricial)
    GPIOB->MODER &= ~((0b11 << 16) | (0b11 << 18) | (0b11 << 20) | (0b11 << 22)); // Limpiar la configuración actual para los pines 8 a 11



    while (1)
    {
        for (int col = 0; col < 4; col++)
        {
            GPIOB->ODR = (0xF << 12); // Desactiva todas las columnas
            GPIOB->ODR &= ~(1 << (12 + col)); // Activa solo la columna actual

            delay_ms(10); // Deja tiempo para la estabilización

            uint32_t rows = GPIOB->IDR & (0xF << 8); // Lee el estado de las filas

           for (int row = 0; row < 4; row++)
{
    // Verifica si alguna fila está activa (presionada)
    if (!(rows & (1 << (row + 8))))
    {
        // Calcula el número de tecla basado en la columna y fila actual
        uint32_t key_number = col * 4 + row;

        // Verifica si la tecla presionada es la tecla D
        if (key_number == TECLA_D)
        {
            int sum_value = 0;

            // Obtiene el valor del primer dígito mostrado (unidades)
            int u_value = get_number_from_key(primer_digito);
            // Obtiene el valor del segundo dígito mostrado (decenas)
            int d_value = get_number_from_key(segundo_digito_display);
            // Obtiene el valor del tercer dígito mostrado (centenas)
            int c_value = get_number_from_key(tercer_digito_display);

            // Calcula el valor total representado en los displays
            sum_value = u_value + (d_value * 10) + (c_value * 100);

            // Suma el valor calculado al valor almacenado
            valor_almacenado = (uint32_t)(valor_almacenado + sum_value);

            // Resetea todos los dígitos mostrados en el display
            primer_digito = segundo_digito_display = tercer_digito_display = 0xff;
        }
        // Verifica si la tecla presionada es la tecla A
        else if (key_number == TECLA_A)
        {
            // Verifica si hay un valor previo almacenado antes de decodificar
            if (valor_almacenado != 0)
            {
                int sum_value = 0;

                // Obtiene valores de los dígitos mostrados similar a la tecla D
                int u_value = get_number_from_key(primer_digito);
                int d_value = get_number_from_key(segundo_digito_display);
                int c_value = get_number_from_key(tercer_digito_display);

                // Calcula la suma de los valores mostrados
                sum_value = u_value + (d_value * 10) + (c_value * 100);

                // Muestra la suma en curso mediante USART
                char info_msg[50];
                sprintf(info_msg, "Sumando: %d + %d", valor_almacenado, sum_value);
                USART2_putstring_E((uint8_t *)info_msg);

                // Actualiza el valor almacenado con la nueva suma
                valor_almacenado = (uint32_t)(valor_almacenado + sum_value);

                // Descompone el nuevo valor almacenado en dígitos individuales
                u_value = valor_almacenado % 10;
                d_value = ((valor_almacenado - u_value) % 100) / 10;
                c_value = ((valor_almacenado - d_value - u_value) % 1000) / 100;

                // Actualiza los displays con los nuevos valores de los dígitos
                primer_digito = get_key_from_number(u_value);
                segundo_digito_display = get_key_from_number(d_value);
                tercer_digito_display = get_key_from_number(c_value);

                // Muestra el resultado final de la suma mediante USART
                char result_msg[50];
                sprintf(result_msg, "Resultado de la suma: %d", valor_almacenado);
                USART2_putstring_E((uint8_t *)result_msg);

                // Resetea el valor almacenado después de mostrar el resultado
                valor_almacenado = 0;
            }
        }
else if (key_number == TECLA_B)
{
    // Procesa la resta de valores almacenados en los displays
    if (valor_almacenado != 0)
    {
        int sum_value = 0;

        // Obtiene valores de los dígitos individuales de los displays
        int u_value = get_number_from_key(primer_digito);  // Unidades
        int d_value = get_number_from_key(segundo_digito_display); // Decenas
        int c_value = get_number_from_key(tercer_digito_display);  // Centenas

        // Calcula el valor numérico total de los dígitos mostrados
        sum_value = sum_value + u_value + (d_value * 10) + (c_value * 100);

        // Muestra el proceso de resta en USART
        char info_msg[50];
        sprintf(info_msg, "Restando: %d - %d", valor_almacenado, sum_value);
        USART2_putstring_E((uint8_t *)info_msg);

        // Realiza la operación de resta
        valor_almacenado = (uint32_t)(valor_almacenado - sum_value);

        // Asegura que el valor no exceda 999
        if (valor_almacenado > 999)
            valor_almacenado = 999;

        // Descompone el valor almacenado actualizado en dígitos individuales
        u_value = valor_almacenado % 10;
        d_value = ((valor_almacenado - u_value) % 100) / 10;
        c_value = ((valor_almacenado - d_value - u_value) % 1000) / 100;

        // Actualiza los displays con los nuevos dígitos
        primer_digito = get_key_from_number(u_value);
        segundo_digito_display = get_key_from_number(d_value);
        tercer_digito_display = get_key_from_number(c_value);

        // Muestra el resultado final de la resta
        char result_msg[50];
        sprintf(result_msg, "Resultado de la resta: %d", valor_almacenado);
        USART2_putstring_E((uint8_t *)result_msg);

        // Resetea el valor almacenado después de la operación
        valor_almacenado = 0;
    }
}
else if (key_number == TECLA_C)
{
    // Procesa la multiplicación de valores almacenados en los displays
    if (valor_almacenado != 0)
    {
        int product_value = 0;

        // Obtiene valores de los dígitos individuales de los displays
        int u_value = get_number_from_key(primer_digito);  // Unidades
        int d_value = get_number_from_key(segundo_digito_display); // Decenas
        int c_value = get_number_from_key(tercer_digito_display);  // Centenas

        // Calcula el valor numérico total de los dígitos mostrados
        int input_value = u_value + (d_value * 10) + (c_value * 100);

        // Muestra el proceso de multiplicación en USART
        char info_msg[50];
        sprintf(info_msg, "Multiplicando: %d * %d", valor_almacenado, input_value);
        USART2_putstring_E((uint8_t *)info_msg);

        // Realiza la operación de multiplicación
        product_value = valor_almacenado * input_value;

        // Descompone el producto en dígitos individuales para los displays
        u_value = product_value % 10;
        d_value = (product_value / 10) % 10;
        c_value = (product_value / 100) % 10;

        // Actualiza los displays con los nuevos dígitos
        primer_digito = get_key_from_number(u_value);
        segundo_digito_display = get_key_from_number(d_value);
        tercer_digito_display = get_key_from_number(c_value);

        // Muestra el resultado final de la multiplicación
        char result_msg[50];
        sprintf(result_msg, "Resultado de la multiplicación: %d", product_value);
        USART2_putstring_E((uint8_t *)result_msg);

        // Resetea el valor almacenado después de la operación
        valor_almacenado = 0;
    }
}

                    else if (key_number == TECLA_F) {
                        // Verifica si valor_almacenado y el nuevo valor no son cero para evitar división por cero
                        int div_value = 0;
                        int u_value = get_number_from_key(primer_digito);
                        int d_value = get_number_from_key(segundo_digito_display);
                        int c_value = get_number_from_key(tercer_digito_display);

                        div_value = u_value + (d_value * 10) + (c_value * 100);

                        char info_msg[50];
                        sprintf(info_msg, "Diviendo: %d dentro de %d", valor_almacenado, div_value);
                        USART2_putstring_E((uint8_t *)info_msg);

                        if (valor_almacenado != 0 && div_value != 0) {
                            valor_almacenado = (uint32_t)(valor_almacenado / div_value);

                            u_value = valor_almacenado % 10;
                            d_value = (valor_almacenado / 10) % 10;
                            c_value = (valor_almacenado / 100) % 10;

                            primer_digito = get_key_from_number(u_value);
                            segundo_digito_display = get_key_from_number(d_value);
                            tercer_digito_display = get_key_from_number(c_value);
                        }

                        char result_msg[50];
                        sprintf(result_msg, "Resultado de la división: %d", valor_almacenado);
                        USART2_putstring_E((uint8_t *)result_msg);

                        valor_almacenado = 0; // Reset valor_almacenado para evitar acumulación de resultados
                    }
                    else if (key_number == 12) { // Assuming 12 is the index for the "E" or "*" button
    // Reset the display values and any relevant state variables
    primer_digito = 0xFF;
    segundo_digito_display = 0xFF;
    tercer_digito_display = 0xFF;
    char result_msg[50];
    USART2_putstring_E("Digitos borrados"); 
    valor_almacenado = 0; // Reset the stored value
}



                    else
                    {
                    	tercer_digito_display = segundo_digito_display;
                        // Actualiza el segundo display con el valor del primer display
                        segundo_digito_display = primer_digito;
                        // Actualiza el primer display con el nuevo dígito
                        primer_digito = key_number;
                    }

                    while (!(GPIOB->IDR & (1 << (row + 8)))); // Espera a que la tecla se suelte
                    break; // Sale del bucle de filas para evitar múltiples lecturas
                }
            }
            GPIOB->ODR |= (1 << (12 + col)); // Desactiva la columna actual antes de continuar
        }

        // Actualizar los valores de los displays
        actualizar_valores_display();
        valor_almacenado = valor_almacenado %1000;
    }


}



void delay_ms(uint16_t n) {
    for (uint32_t i = 0; i < n * 10; i++) {
        __asm__("NOP");
    }
}


void actualizar_valores_display(void) {

    // Asumiendo que el cuarto dígito se activa con un pin específico, por ejemplo GPIOC->BSRR = 1 << 7 para activarlo
    // Asumiendo que el cuarto dígito se activa con un pin específico, aquí PC9

    if (tercer_digito_display != 0xFF) {
            GPIOC->BSRR = 1 << (6 + 16); // Apaga el segundo dígito
            GPIOC->BSRR = 1 << (5 + 16); // Apaga el primer dígito
            GPIOC->BSRR = 1 << 8; // Enciende el tercer dígito
            GPIOB->BSRR = 0xFF << 16; // Apaga todos los segmentos
            GPIOB->BSRR = decodificador(tercer_digito_display); // Muestra el tercer dígito
            delay_ms(5); // Retardo para la visualización
    }
    // Multiplexación para mostrar los números en los displays
    if (segundo_digito_display != 0xFF) {
    	GPIOC->BSRR = 1 << (8 + 16); // Apaga el tercer dígito
        GPIOC->BSRR = 1 << (5 + 16); // Apaga el primer dígito
        GPIOC->BSRR = 1 << 6; // Enciende el segundo dígito
        GPIOB->BSRR = 0xFF << 16; // Apaga todos los segmentos
        GPIOB->BSRR = decodificador(segundo_digito_display); // Muestra el segundo dígito
        delay_ms(5); // Retardo para la visualización
    }

    if (primer_digito != 0xFF) {
    	GPIOC->BSRR = 1 << (8 + 16); // Apaga el tercer dígito
        GPIOC->BSRR = 1 << (6 + 16); // Apaga el segundo dígito
        GPIOC->BSRR = 1 << 5; // Enciende el primer dígito
        GPIOB->BSRR = 0xFF << 16; // Apaga todos los segmentos
        GPIOB->BSRR = decodificador(primer_digito); // Muestra el primer dígito
        delay_ms(5); // Retardo para la visualización
    }

}

uint32_t decodificador(uint32_t number) {
	uint32_t key = 0;
	switch (number){
		case 0: key = 0; break;
		case 1: key = 1; break;
		case 2: key = 2; break;
		case 4: key = 4; break;
		case 5: key = 5; break;
		case 6: key = 6; break;
		case 8: key = 8; break;
		case 9: key = 9; break;
		case 10: key = 10; break;
		case 13: key = 13; break;
		default: return 13;
	}
	return tecla_a_mostrar[key];
}



void decodificador_decimal(uint32_t value) {
    // Verifica si el valor almacenado es un dígito válido (excluyendo A, B, C, D, * y #)
    if (value < 10) {
        // Actualiza los displays con el valor almacenado
    	tercer_digito_display = segundo_digito_display;
        segundo_digito_display = primer_digito; // Mueve el primer dígito al segundo dígito
        primer_digito = value; // Establece el primer dígito como el valor almacenado
    }
}


uint8_t USART2_read(void)
{
	while(!(USART2->ISR & 0x0020)){}
	return USART2->RDR;
}

void USART2_write(uint8_t ch)
{
	while(!(USART2->ISR & 0x0080)){}
	USART2->TDR = ch;
}

void USART2_putstring(uint8_t* StringPtr)
{
	while(*StringPtr != 0x00)
	{
		USART2_write(*StringPtr);
		StringPtr++;
	}
}

void USART2_putstring_E(uint8_t* StringPtr)
{
	while(*StringPtr != 0x00)
	{
		USART2_write(*StringPtr);
		StringPtr++;
	}
	USART2_write(0x0D);//CR
	USART2_write(0x0A);//NL
}



