
#define F_CPU 16000000UL

#include <avr/io.h>
#include <stdint.h>
#include "Buzzer.h"

// Inicializar buzzer pasivo en PB3 (OC2A)
void buzzer_inicializar(void)
{
	DDRB |= (1 << PB3);

	TCCR2A = 0;
	TCCR2B = 0;
	TCNT2 = 0;
	PORTB &= ~(1 << PB3);
}

// Reproducir una nota usando Timer2 en modo CTC
void reproducir_nota(uint16_t frecuencia)
{
	if (frecuencia == 0)
	{
		buzzer_detener();
		return;
	}

	// Detener Timer2 antes de cambiar la frecuencia
	TCCR2B = 0;

	// Modo CTC y conmutación de OC2A
	TCCR2A = (1 << COM2A0) | (1 << WGM21);

	// Prescaler 128
	uint32_t divisor = (2UL * 128UL * frecuencia);
	uint32_t comparacion = (F_CPU / divisor) - 1;

	if (comparacion > 255)
	{
		comparacion = 255;
	}

	OCR2A = (uint8_t)comparacion;
	TCNT2 = 0;

	// Limpiar bandera de comparación pendiente
	TIFR2 = (1 << OCF2A);

	// Iniciar Timer2 con prescaler 128
	TCCR2B = (1 << CS22) | (1 << CS20);
}

// Detener el sonido
void buzzer_detener(void)
{
	TCCR2B = 0;
	TCCR2A = 0;

	// Desconectar OC2A y llevar PB3 a nivel bajo
	PORTB &= ~(1 << PB3);
}
