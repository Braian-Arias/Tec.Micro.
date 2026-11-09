#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <stdint.h>

#include "temporizadores.h"

// Contador general del sistema en milisegundos
static volatile uint32_t tiempo_sistema_ms = 0;

// Banderas de eventos
static volatile uint8_t solicitud_adc = 0;
static volatile uint8_t evento_boton = 0;

// Timer0: interrupción cada 1 ms
ISR(TIMER0_COMPA_vect)
{
	tiempo_sistema_ms++;
}

// Timer1: solicitud de ADC cada 100 ms
ISR(TIMER1_COMPA_vect)
{
	solicitud_adc = 1;
}

// INT0: pulsador
ISR(INT0_vect)
{
	evento_boton = 1;
}

void tmr_inicializar(void)
{
	cli();

	// TIMER0: 16 MHz / 64 = 250 kHz, OCR0A = 249 -> 1 ms (CTC)
	TCCR0A = 0;
	TCCR0B = 0;
	TCCR0A |= (1 << WGM01);
	OCR0A = 249;
	TCCR0B |= (1 << CS01) | (1 << CS00);
	TIFR0 = (1 << OCF0A);
	TIMSK0 |= (1 << OCIE0A);

	// TIMER1: 16 MHz / 64 = 250 kHz, OCR1A = 24999 -> 100 ms (CTC)
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1B |= (1 << WGM12);
	OCR1A = 24999;
	TCCR1B |= (1 << CS11) | (1 << CS10);
	TIFR1 = (1 << OCF1A);
	TIMSK1 |= (1 << OCIE1A);

	// PULSADOR: D2 / PD2 / INT0, pull-up, flanco descendente
	DDRD &= ~(1 << DDD2);
	PORTD |= (1 << PORTD2);
	EICRA &= ~(1 << ISC00);
	EICRA |= (1 << ISC01);
	EIFR = (1 << INTF0);
	EIMSK |= (1 << INT0);

	sei();
}

uint32_t tmr_obtener_tiempo_ms(void)
{
	uint32_t tiempo;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		tiempo = tiempo_sistema_ms;
	}

	return tiempo;
}

uint8_t tmr_obtener_solicitud_adc(void)
{
	uint8_t evento;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		evento = solicitud_adc;
		solicitud_adc = 0;
	}

	return evento;
}

uint8_t tmr_obtener_evento_boton(void)
{
	uint8_t evento;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		evento = evento_boton;
		evento_boton = 0;
	}

	return evento;
}