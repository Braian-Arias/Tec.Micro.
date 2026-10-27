
#include "config.h"

#if TIPO_MATRIZ == 2

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "Matriz.h"

// Pines para MAX7219
#define MAX_DIN PD4
#define MAX_CLK PD5
#define MAX_CS  PD6

#define MAX_DDR  DDRD
#define MAX_PORT PORTD

static uint8_t buffer[8];

// Enviar un byte al MAX7219
static void max_enviar(uint8_t registro, uint8_t dato)
{
	MAX_PORT &= ~(1 << MAX_CS);

	for (int8_t i = 7; i >= 0; i--)
	{
		MAX_PORT &= ~(1 << MAX_CLK);

		if (registro & (1 << i))
		MAX_PORT |= (1 << MAX_DIN);
		else
		MAX_PORT &= ~(1 << MAX_DIN);

		MAX_PORT |= (1 << MAX_CLK);
	}

	for (int8_t i = 7; i >= 0; i--)
	{
		MAX_PORT &= ~(1 << MAX_CLK);

		if (dato & (1 << i))
		MAX_PORT |= (1 << MAX_DIN);
		else
		MAX_PORT &= ~(1 << MAX_DIN);

		MAX_PORT |= (1 << MAX_CLK);
	}

	MAX_PORT |= (1 << MAX_CS);
}

// Inicializar el MAX7219
void matriz_inicializar(void)
{
	MAX_DDR |= (1 << MAX_DIN) | (1 << MAX_CLK) | (1 << MAX_CS);

	MAX_PORT |= (1 << MAX_CS);
	MAX_PORT &= ~(1 << MAX_CLK);

	max_enviar(0x0F, 0x00); // Modo de prueba desactivado
	max_enviar(0x0C, 0x01); // Salir de apagado
	max_enviar(0x0B, 0x07); // Usar 8 filas
	max_enviar(0x09, 0x00); // Sin decodificación BCD
	max_enviar(0x0A, 0x03); // Brillo bajo

	for (uint8_t i = 0; i < 8; i++)
	max_enviar(i + 1, 0x00);

	matriz_limpiar();
}

// Apagar el buffer
void matriz_limpiar(void)
{
	for (uint8_t i = 0; i < 8; i++)
	buffer[i] = 0;
}

// Dibujar un píxel
void matriz_pixel(uint8_t fila, uint8_t columna, uint8_t color)
{
	if (fila > 7 || columna > 7)
	return;

	if (color == COLOR_APAGADO)
	buffer[fila] &= ~(1 << columna);
	else
	buffer[fila] |= (1 << columna);
}

// Actualizar las 8 filas
void matriz_actualizar(void)
{
	for (uint8_t fila = 0; fila < 8; fila++)
	max_enviar(fila + 1, buffer[fila]);
}

// Animación de éxito
void matriz_animacion_exito(void)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		buffer[i] = 0xFF;
		matriz_actualizar();
		_delay_ms(70);
	}

	matriz_limpiar();
	matriz_actualizar();
}

// Animación de error
void matriz_animacion_error(void)
{
	for (uint8_t i = 0; i < 3; i++)
	{
		for (uint8_t fila = 0; fila < 8; fila++)
		buffer[fila] = 0xFF;

		matriz_actualizar();
		_delay_ms(150);

		matriz_limpiar();
		matriz_actualizar();
		_delay_ms(150);
	}
}

#endif
