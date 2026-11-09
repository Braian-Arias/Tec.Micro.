#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdlib.h>

#include "LCD_I2C.h"

#define DIRECCION_LCD  0x27
#define BIT_LUZ_LCD    0x08
#define BIT_ENABLE_LCD 0x04
#define BIT_RS_LCD     0x01

static uint8_t lcd_disponible = 0;

static uint8_t i2c_esperar(void)
{
	uint16_t intentos = 60000;

	while (!(TWCR & (1 << TWINT)) && intentos > 0)
	{
		intentos--;
	}

	return intentos > 0;
}

void i2c_iniciar(void)
{
	// Prescaler = 1; velocidad aproximada de 100 kHz
	TWSR = 0;
	TWBR = 72;
	TWCR = (1 << TWEN);
}

static uint8_t i2c_enviar_byte(uint8_t direccion, uint8_t dato)
{
	uint8_t estado;

	// START
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

	if (!i2c_esperar())
	goto error;

	estado = TWSR & 0xF8;

	if (estado != 0x08 && estado != 0x10)
	goto error;

	// Dirección del dispositivo en modo escritura
	TWDR = (uint8_t)(direccion << 1);
	TWCR = (1 << TWINT) | (1 << TWEN);

	if (!i2c_esperar())
	goto error;

	if ((TWSR & 0xF8) != 0x18)
	goto error;

	// Enviar dato al expansor
	TWDR = dato;
	TWCR = (1 << TWINT) | (1 << TWEN);

	if (!i2c_esperar())
	goto error;

	if ((TWSR & 0xF8) != 0x28)
	goto error;

	// STOP
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);

	return 1;

	error:
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	return 0;
}

static uint8_t lcd_expansor(uint8_t dato)
{
	return i2c_enviar_byte(
	DIRECCION_LCD,
	(uint8_t)(dato | BIT_LUZ_LCD)
	);
}

static void lcd_pulso_enable(uint8_t dato)
{
	lcd_expansor((uint8_t)(dato | BIT_ENABLE_LCD));

	_delay_us(1);

	lcd_expansor((uint8_t)(dato & ~BIT_ENABLE_LCD));

	_delay_us(50);
}

static void lcd_enviar_nibble(uint8_t nibble, uint8_t modo_dato)
{
	uint8_t salida;

	salida = (uint8_t)((nibble & 0x0F) << 4);

	if (modo_dato)
	salida |= BIT_RS_LCD;

	lcd_expansor(salida);
	lcd_pulso_enable(salida);
}

static void lcd_enviar_byte(uint8_t dato, uint8_t modo_dato)
{
	if (!lcd_disponible)
	return;

	lcd_enviar_nibble((uint8_t)(dato >> 4), modo_dato);
	lcd_enviar_nibble((uint8_t)(dato & 0x0F), modo_dato);
}

static void lcd_comando(uint8_t comando)
{
	lcd_enviar_byte(comando, 0);
}

static void lcd_caracter(char caracter)
{
	lcd_enviar_byte((uint8_t)caracter, 1);
}

void lcd_iniciar(void)
{
	_delay_ms(50);

	lcd_disponible = lcd_expansor(0);

	if (!lcd_disponible)
	return;

	lcd_enviar_nibble(0x03, 0);
	_delay_ms(5);

	lcd_enviar_nibble(0x03, 0);
	_delay_us(150);

	lcd_enviar_nibble(0x03, 0);
	_delay_ms(2);

	lcd_enviar_nibble(0x02, 0);
	_delay_ms(2);

	// LCD de 4 bits y 2 líneas
	lcd_comando(0x28);
	_delay_ms(2);

	// Pantalla encendida, cursor apagado
	lcd_comando(0x0C);
	_delay_ms(2);

	// Incrementar posición automáticamente
	lcd_comando(0x06);
	_delay_ms(2);

	lcd_comando(0x01);
	_delay_ms(2);
}

void lcd_posicion(uint8_t fila, uint8_t columna)
{
	uint8_t posicion;

	if (fila == 0)
	posicion = columna;
	else
	posicion = (uint8_t)(0x40 + columna);

	lcd_comando((uint8_t)(0x80 | posicion));
}

void lcd_texto(const char *texto)
{
	while (*texto != '\0')
	{
		lcd_caracter(*texto);
		texto++;
	}
}

void lcd_numero(uint16_t numero)
{
	char texto_numero[6];

	utoa(numero, texto_numero, 10);
	lcd_texto(texto_numero);
}

void lcd_limpiar(void)
{
	lcd_comando(0x01);
	_delay_ms(2);
}

void lcd_limpiar_linea(uint8_t fila)
{
	lcd_posicion(fila, 0);

	for (uint8_t columna = 0; columna < 16; columna++)
	lcd_caracter(' ');
}