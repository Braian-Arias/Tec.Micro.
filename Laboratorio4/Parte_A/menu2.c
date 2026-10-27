
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#include "SPI.h"
#include "RC522.h"
#include "LCD_I2C.h"
#include "UART.h"
#include "Teclado4x4.h"

// Estados del cajero
typedef enum
{
	ESPERA_TARJETA,
	INGRESAR_PIN,
	ACCESO_AUTORIZADO,
	ACCESO_DENEGADO
} EstadoCajero;

// Reemplazar por el UID real de la tarjeta
static const uint8_t uid_autorizado[4] =
{
	0xDE, 0xAD, 0xBE, 0xEF
};

// PIN de prueba
static const char pin_correcto[] = "1234";

// Variables
static uint8_t uid_leido[4];
static char pin_ingresado[5];
static uint8_t cantidad_digitos = 0;

// Compara el UID leído con el autorizado
static uint8_t verificar_tarjeta(void)
{
	for (uint8_t i = 0; i < 4; i++)
	{
		if (uid_leido[i] != uid_autorizado[i])
		{
			return 0;
		}
	}

	return 1;
}

// Pantalla inicial
static void mostrar_inicio(void)
{
	lcd_limpiar();
	lcd_posicion(0, 0);
	lcd_texto("CAJERO AUTOMAT.");
	lcd_posicion(1, 0);
	lcd_texto("Acerque tarjeta");
}

// Solicita el PIN
static void mostrar_ingreso_pin(void)
{
	lcd_limpiar();
	lcd_posicion(0, 0);
	lcd_texto("Ingrese su PIN:");
	lcd_posicion(1, 0);
}

// Limpia el PIN ingresado
static void limpiar_pin(void)
{
	cantidad_digitos = 0;

	for (uint8_t i = 0; i < 5; i++)
	{
		pin_ingresado[i] = '\0';
	}
}

// Muestra asteriscos en lugar del PIN
static void mostrar_asteriscos(void)
{
	lcd_posicion(1, 0);

	for (uint8_t i = 0; i < cantidad_digitos; i++)
	{
		lcd_texto("*");
	}
}

// Compara el PIN ingresado
static uint8_t verificar_pin(void)
{
	for (uint8_t i = 0; i < 4; i++)
	{
		if (pin_ingresado[i] != pin_correcto[i])
		{
			return 0;
		}
	}

	return 1;
}

// Espera a que retiren la tarjeta
static void esperar_retirar_tarjeta(void)
{
	uint8_t uid_temporal[4];

	lcd_limpiar();
	lcd_posicion(0, 0);
	lcd_texto("Retire tarjeta");

	while (mfrc522_standard(uid_temporal))
	{
		_delay_ms(100);
	}
}

// Muestra el resultado del acceso
static void mostrar_resultado(uint8_t autorizado)
{
	lcd_limpiar();

	if (autorizado)
	{
		lcd_posicion(0, 0);
		lcd_texto("PIN correcto");
		lcd_posicion(1, 0);
		lcd_texto("Acceso permitido");

		uart_print("PIN correcto. Acceso permitido.\r\n");
	}
	else
	{
		lcd_posicion(0, 0);
		lcd_texto("PIN incorrecto");
		lcd_posicion(1, 0);
		lcd_texto("Acceso denegado");

		uart_print("PIN incorrecto. Acceso denegado.\r\n");
	}

	_delay_ms(2000);
}

// Programa principal
int main(void)
{
	EstadoCajero estado = ESPERA_TARJETA;
	char tecla;

	// Iniciar
	uart_init(103);
	i2c_iniciar();
	lcd_iniciar();

	spi_init();
	mfrc522_resetPinInit();
	mfrc522_init();

	teclado4x4_inicio();

	uart_print("\r\n====================\r\n");
	uart_print(" CAJERO - AUTOMATICO    \r\n");
	uart_print("========================\r\n");
	uart_print("Sistema inicializado.\r\n");
	uart_print("Esperando tarjeta...\r\n");

	mostrar_inicio();

	while (1)
	{
		switch (estado)
		{
			case ESPERA_TARJETA:

			if (mfrc522_standard(uid_leido))
			{
				uart_print("\r\nTarjeta detectada.\r\n");
				uart_print("UID: ");
				uart_print_hex_array(uid_leido, 4);
				uart_print("\r\n");

				if (verificar_tarjeta())
				{
					uart_print("Tarjeta autorizada.\r\n");

					limpiar_pin();
					mostrar_ingreso_pin();
					estado = INGRESAR_PIN;
				}
				else
				{
					uart_print("Tarjeta no autorizada.\r\n");

					lcd_limpiar();
					lcd_posicion(0, 0);
					lcd_texto("Tarjeta invalida");
					lcd_posicion(1, 0);
					lcd_texto("Acceso denegado");

					_delay_ms(2000);
					esperar_retirar_tarjeta();

					estado = ESPERA_TARJETA;
					mostrar_inicio();
				}
			}

			break;

			case INGRESAR_PIN:

			tecla = teclado4x4_leerTecla();

			if (tecla >= '0' && tecla <= '9')
			{
				if (cantidad_digitos < 4)
				{
					pin_ingresado[cantidad_digitos] = tecla;
					cantidad_digitos++;

					pin_ingresado[cantidad_digitos] = '\0';

					mostrar_asteriscos();
				}

				if (cantidad_digitos == 4)
				{
					_delay_ms(300);

					if (verificar_pin())
					{
						estado = ACCESO_AUTORIZADO;
					}
					else
					{
						estado = ACCESO_DENEGADO;
					}
				}
			}
			else if (tecla == '*')
			{
				limpiar_pin();
				mostrar_ingreso_pin();
			}
			else if (tecla == '#')
			{
				limpiar_pin();
				mostrar_ingreso_pin();
			}

			break;

			case ACCESO_AUTORIZADO:

			mostrar_resultado(1);
			esperar_retirar_tarjeta();

			estado = ESPERA_TARJETA;
			mostrar_inicio();

			break;

			case ACCESO_DENEGADO:

			mostrar_resultado(0);
			esperar_retirar_tarjeta();

			estado = ESPERA_TARJETA;
			mostrar_inicio();

			break;

			default:

			estado = ESPERA_TARJETA;
			mostrar_inicio();

			break;
		}
	}

	return 0;
}
