#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <stdint.h>

#include "adc.h"

// Resultado de la conversión
static volatile uint16_t adc_valor = 0;

// Indica que existe un resultado disponible
static volatile uint8_t adc_listo = 0;

// Interrupción al finalizar la conversión
ISR(ADC_vect)
{
	adc_valor = ADC;
	adc_listo = 1;
}

// Configura ADC0 (PC0) con referencia AVCC, prescaler 128 (125 kHz) e interrupción
void adc_inicializar(void)
{
	// A0 / PC0 como entrada sin pull-up
	DDRC &= ~(1 << DDC0);
	PORTC &= ~(1 << PORTC0);

	// Referencia AVCC, canal ADC0, justificación a la derecha
	ADMUX = (1 << REFS0);
	ADMUX &= ~(1 << ADLAR);

	// Habilitar ADC e interrupción, prescaler 128
	ADCSRA = (1 << ADEN) | (1 << ADIE) |
	(1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

	// Sin disparo automático
	ADCSRB = 0;

	adc_valor = 0;
	adc_listo = 0;
}

// Inicia una conversión si no hay una en curso
void adc_iniciar_conversion(void)
{
	if (!(ADCSRA & (1 << ADSC)))
	{
		ADCSRA |= (1 << ADSC);
	}
}

// Entrega el resultado si está disponible (lectura atómica)
uint8_t adc_obtener_resultado(uint16_t *resultado)
{
	uint8_t listo = 0;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if (adc_listo)
		{
			*resultado = adc_valor;
			adc_listo = 0;
			listo = 1;
		}
	}

	return listo;
}