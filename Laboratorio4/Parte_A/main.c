// Cajero Automatico
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#include "SPI.h"
#include "RC522.h"
#include "LCD_I2C.h"
//#include "UART.h" hacer un uart no entiendo mejor el mio modificar despues el uart y modificar esto. no tocar por ahora, es para probar si anda la tarjeta esto

#define VELOCIDAD_UART 9600UL
#define VALOR_UBRR ((F_CPU / (16UL * VELOCIDAD_UART)) - 1UL)

static uint8_t uid_leido[4];

static void uart_iniciar(void)
{
	UBRR0H = (uint8_t)(VALOR_UBRR >> 8);
	UBRR0L = (uint8_t)VALOR_UBRR;

	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

static void uart_enviar_caracter(char caracter)
{
	while (!(UCSR0A & (1 << UDRE0)))
	{
	}

	UDR0 = caracter;
}

static void uart_enviar_texto(const char *texto)
{
	while (*texto != '\0')
	{
		uart_enviar_caracter(*texto);
		texto++;
	}
}

static char hex_digito(uint8_t valor)
{
	valor &= 0x0F;

	if (valor < 10)
	return (char)('0' + valor);

	return (char)('A' + valor - 10);
}

static void uart_enviar_hex(uint8_t valor)
{
	uart_enviar_texto("0x");
	uart_enviar_caracter(hex_digito(valor >> 4));
	uart_enviar_caracter(hex_digito(valor));
}

static void mostrar_uid_lcd(const uint8_t *uid)
{
	lcd_limpiar();

	lcd_posicion(0, 0);
	lcd_texto("Tarjeta detect.");

	lcd_posicion(1, 0);
	lcd_texto("UID: ");

	for (uint8_t i = 0; i < 4; i++)
	{
		lcd_caracter:
		{
			char digito_alto = hex_digito(uid[i] >> 4);
			char digito_bajo = hex_digito(uid[i]);
			
			char texto[3] = {
				digito_alto,
				digito_bajo,
				'\0'
			};

			lcd_texto(texto);
		}

		if (i < 3)
		lcd_texto(" ");
	}
}

int main(void)
{
	uart_iniciar();

	i2c_iniciar();
	lcd_iniciar();

	spi_init();
	mfrc522_resetPinInit();
	mfrc522_init();

	lcd_limpiar();
	lcd_posicion(0, 0);
	lcd_texto("CAJERO AUTOMATICO");

	lcd_posicion(1, 0);
	lcd_texto("Acerque tarjeta");

	uart_enviar_texto("\r\n========================\r\n");
	uart_enviar_texto(" CAJERO - ACTUALIZACION 1\r\n");
	uart_enviar_texto("========================\r\n");
	uart_enviar_texto("SPI inicializado.\r\n");
	uart_enviar_texto("LCD I2C inicializado.\r\n");
	uart_enviar_texto("RC522 inicializado.\r\n");
	uart_enviar_texto("Esperando tarjeta...\r\n");

	while (1)
	{
		if (mfrc522_standard(uid_leido))
		{
			uart_enviar_texto("\r\nTarjeta detectada\r\nUID: ");

			for (uint8_t i = 0; i < 4; i++)
			{
				uart_enviar_hex(uid_leido[i]);

				if (i < 3)
				uart_enviar_texto(" ");
			}

			uart_enviar_texto("\r\n");

			mostrar_uid_lcd(uid_leido);

			_delay_ms(1500);

			lcd_limpiar();
			lcd_posicion(0, 0);
			lcd_texto("CAJERO AUTOMATICO");

			lcd_posicion(1, 0);
			lcd_texto("Acerque tarjeta");
		}
	}
}

