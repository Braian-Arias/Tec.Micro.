
#define F_CPU 16000000UL

#include <avr/io.h>
#include <stdint.h>
#include "Joystick.h"

// Límites para la zona muerta del joystick
#define JOYSTICK_MIN 350
#define JOYSTICK_MAX 650

// Inicializar ADC y botón
void joystick_inicializar(void)
{
	// ADC habilitado, prescaler 128
	// A 16 MHz, el ADC funciona a 125 kHz
	ADMUX = (1 << REFS0);
	ADCSRA = (1 << ADEN)
	| (1 << ADPS2)
	| (1 << ADPS1)
	| (1 << ADPS0);

	// Deshabilitar entradas digitales en ADC0 y ADC1
	DIDR0 |= (1 << ADC0D) | (1 << ADC1D);

	// Botón SW en PD2 con pull-up interna
	DDRD &= ~(1 << PD2);
	PORTD |= (1 << PD2);
}

// Leer un canal del ADC
static uint16_t adc_leer(uint8_t canal)
{
	// Seleccionar ADC0 o ADC1, manteniendo la referencia AVcc
	ADMUX = (1 << REFS0) | (canal & 0x07);

	// Iniciar conversión
	ADCSRA |= (1 << ADSC);

	// Esperar a que termine
	while (ADCSRA & (1 << ADSC))
	{
	}

	return ADC;
}

// Leer eje X: A0
uint16_t joystick_leer_x(void)
{
	return adc_leer(0);
}

// Leer eje Y: A1
uint16_t joystick_leer_y(void)
{
	return adc_leer(1);
}

// Leer pulsador: activo en nivel bajo
uint8_t joystick_boton_presionado(void)
{
	return !(PIND & (1 << PD2));
}

// Actualizar cursor respetando los límites 0 a 7
void joystick_actualizar_cursor(uint8_t *fila, uint8_t *columna)
{
	uint16_t x = joystick_leer_x();
	uint16_t y = joystick_leer_y();

	// Movimiento horizontal
	if (x < JOYSTICK_MIN)
	{
		if (*columna > 0)
		{
			(*columna)--;
		}
	}
	else if (x > JOYSTICK_MAX)
	{
		if (*columna < 7)
		{
			(*columna)++;
		}
	}

	// Movimiento vertical
	if (y < JOYSTICK_MIN)
	{
		if (*fila > 0)
		{
			(*fila)--;
		}
	}
	else if (y > JOYSTICK_MAX)
	{
		if (*fila < 7)
		{
			(*fila)++;
		}
	}
}
