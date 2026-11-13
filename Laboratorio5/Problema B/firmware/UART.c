#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <stdint.h>
#include "uart.h"

// ====================================
// INICIALIZACION UART
// ====================================

void uart_inicializar(void)
{
	// Activar USART0
	PRR &= ~(1 << PRUSART0);

	// Modo normal
	UCSR0A = 0;

	// 9600 baudios con 16 MHz
	UBRR0H = 0;
	UBRR0L = 103;

	// Activar transmisor
	UCSR0B = (1 << TXEN0);

	// 8 bits, sin paridad, 1 stop
	UCSR0C = (1 << UCSZ01) |
	(1 << UCSZ00);
}

// ====================================
// ENVIAR CARACTER
// ====================================

void uart_enviar_caracter(char dato)
{
	while (!(UCSR0A & (1 << UDRE0)))
	{
		// Esperar buffer libre
	}

	UDR0 = (uint8_t)dato;
}

// ====================================
// ENVIAR TEXTO
// ====================================

void uart_enviar_texto(const char *texto)
{
	while (*texto != '\0')
	{
		uart_enviar_caracter(*texto);
		texto++;
	}
}

// ====================================
// ENVIAR NUMERO ENTERO
// ====================================

void uart_enviar_numero(uint32_t numero)
{
	char buffer[10];
	uint8_t i = 0;

	if (numero == 0)
	{
		uart_enviar_caracter('0');
		return;
	}

	while (numero > 0)
	{
		buffer[i++] = '0' + (numero % 10);
		numero /= 10;
	}

	while (i > 0)
	{
		uart_enviar_caracter(buffer[--i]);
	}
}

// ====================================
// ESPERAR FIN DE TRANSMISION
// ====================================

void uart_esperar_fin(void)
{
	while (!(UCSR0A & (1 << TXC0)))
	{
	}

	// Limpiar bandera escribiendo 1
	UCSR0A |= (1 << TXC0);
}

// ====================================
// TRANSMITIR DATOS A MATLAB
// ====================================

void uart_enviar_matlab(
uint32_t tiempo_ms,
uint8_t estado,
uint16_t corriente_centesimas)
{
	// Reactivar USART si PRR la deshabilito
	PRR &= ~(1 << PRUSART0);

	// Limpiar bandera de transmision completa
	UCSR0A |= (1 << TXC0);

	// Enviar tiempo
	uart_enviar_numero(tiempo_ms);
	uart_enviar_caracter(',');

	// Enviar estado
	if (estado == 0)
	{
		uart_enviar_texto("ACTIVO");
	}
	else if (estado == 1)
	{
		uart_enviar_texto("STANDBY");
	}
	else if (estado == 2)
	{
		uart_enviar_texto("APAGADO");
	}
	else
	{
		uart_enviar_texto("DESCONOCIDO");
	}

	uart_enviar_caracter(',');

	// Enviar corriente con dos decimales
	uart_enviar_numero(
	corriente_centesimas / 100);

	uart_enviar_caracter('.');

	uint8_t decimales =
	corriente_centesimas % 100;

	if (decimales < 10)
	{
		uart_enviar_caracter('0');
	}

	uart_enviar_numero(decimales);

	uart_enviar_texto("\r\n");

	// Asegurar transmision completa
	uart_esperar_fin();
}