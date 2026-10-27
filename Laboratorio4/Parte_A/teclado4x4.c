```c
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#include "Teclado4x4.h"

// Distribucion de las teclas del teclado matricial 4x4
static const char teclas[4][4] = {
	{'1', '2', '3', 'A'},
	{'4', '5', '6', 'B'},
	{'7', '8', '9', 'C'},
	{'*', '0', '#', 'D'}
};

// Configurar los pines del teclado
void teclado4x4_inicio(void)
{
	// Filas: PD2, PD3, PD4 y PD5 como salidas
	DDRD |= 0x3C;

	// Columnas: PD6 y PD7 como entradas
	DDRD &= ~0xC0;

	// Columnas: PC0 y PC1 como entradas
	DDRC &= ~0x03;

	// Filas inicialmente en alto
	PORTD |= 0x3C;

	// Activar resistencias pull-up de las columnas
	PORTD |= 0xC0;
	PORTC |= 0x03;
}

// Buscar una tecla recorriendo las filas y columnas
static char teclado4x4_escanear(void)
{
	for (uint8_t fila = 0; fila < 4; fila++)
	{
		// Colocar todas las filas en alto
		PORTD |= 0x3C;

		// Activar la fila actual poniendola en bajo
		PORTD &= ~(1 << (fila + 2));

		_delay_us(5);

		// Columna 1: PD6
		if (!(PIND & (1 << PD6)))
		return teclas[fila][0];

		// Columna 2: PD7
		if (!(PIND & (1 << PD7)))
		return teclas[fila][1];

		// Columna 3: PC0
		if (!(PINC & (1 << PC0)))
		return teclas[fila][2];

		// Columna 4: PC1
		if (!(PINC & (1 << PC1)))
		return teclas[fila][3];
	}

	// No hay ninguna tecla presionada
	return '\0';
}

// Leer una tecla con antirrebote
char teclado4x4_leerTecla(void)
{
	char tecla = teclado4x4_escanear();

	if (tecla == '\0')
	return '\0';

	// Esperar para evitar rebotes mecanicos
	_delay_ms(20);

	// Confirmar que la tecla sigue presionada
	if (teclado4x4_escanear() != tecla)
	return '\0';

	// Esperar hasta que se suelte la tecla
	while (teclado4x4_escanear() != '\0')
	{
		_delay_ms(5);
	}

	return tecla;
}
```