
#include "config.h"

#if TIPO_MATRIZ == 1

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include "Matriz.h"

// Pin de datos WS2812B: PB0 (Arduino D8)
#define WS_PIN PB0

static uint8_t leds[64][3];

static const uint8_t colores[8][3] = {
	{0,   0,   0},   // Apagado
	{255, 0,   0},   // Rojo
	{0,   255, 0},   // Verde
	{0,   0,   255}, // Azul
	{255, 180, 0},   // Amarillo
	{0,   180, 255}, // Celeste
	{255, 0,   180}, // Magenta
	{255, 255, 255}  // Blanco
};

// Índice de LED en una matriz conectada en filas serpenteantes
static uint8_t indice_led(uint8_t fila, uint8_t columna)
{
	if (fila & 1)
	columna = 7 - columna;

	return fila * 8 + columna;
}

// Enviar un bit con temporización por software
static inline void enviar_bit(uint8_t bit)
{
	if (bit)
	{
		PORTB |= (1 << WS_PIN);
		asm volatile("nop\n\tnop\n\tnop\n\tnop\n\t");
		PORTB &= ~(1 << WS_PIN);
		asm volatile("nop\n\tnop\n\t");
	}
	else
	{
		PORTB |= (1 << WS_PIN);
		asm volatile("nop\n\t");
		PORTB &= ~(1 << WS_PIN);
		asm volatile("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\t");
	}
}

// Enviar un byte en orden MSB primero
static void enviar_byte(uint8_t dato)
{
	for (uint8_t mascara = 0x80; mascara; mascara >>= 1)
	enviar_bit(dato & mascara);
}

// Enviar el contenido completo de la matriz
static void enviar_matriz(void)
{
	uint8_t sreg = SREG;
	cli();

	for (uint8_t i = 0; i < 64; i++)
	{
		// WS2812B utiliza orden GRB
		enviar_byte(leds[i][1]);
		enviar_byte(leds[i][0]);
		enviar_byte(leds[i][2]);
	}

	SREG = sreg;

	// Tiempo de reset/latch
	_delay_us(80);
}

// Inicializar matriz
void matriz_inicializar(void)
{
	DDRB |= (1 << WS_PIN);
	PORTB &= ~(1 << WS_PIN);

	for (uint8_t i = 0; i < 64; i++)
	{
		leds[i][0] = 0;
		leds[i][1] = 0;
		leds[i][2] = 0;
	}

	enviar_matriz();
}

// Limpiar matriz
void matriz_limpiar(void)
{
	for (uint8_t i = 0; i < 64; i++)
	{
		leds[i][0] = 0;
		leds[i][1] = 0;
		leds[i][2] = 0;
	}
}

// Dibujar píxel
void matriz_pixel(uint8_t fila, uint8_t columna, uint8_t color)
{
	if (fila > 7 || columna > 7 || color > COLOR_BLANCO)
	return;

	uint8_t indice = indice_led(fila, columna);

	leds[indice][0] = colores[color][0];
	leds[indice][1] = colores[color][1];
	leds[indice][2] = colores[color][2];
}

// Actualizar matriz
void matriz_actualizar(void)
{
	enviar_matriz();
}

// Animación de éxito
void matriz_animacion_exito(void)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		for (uint8_t j = 0; j < 8; j++)
		matriz_pixel(i, j, COLOR_VERDE);

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
		matriz_limpiar();

		for (uint8_t fila = 0; fila < 8; fila++)
		for (uint8_t columna = 0; columna < 8; columna++)
		matriz_pixel(fila, columna, COLOR_ROJO);

		matriz_actualizar();
		_delay_ms(150);

		matriz_limpiar();
		matriz_actualizar();
		_delay_ms(150);
	}
}

#endif
