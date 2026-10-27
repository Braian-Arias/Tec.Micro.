#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "SPI.h"
#include "RC522.h"
#include "LCD_I2C.h"
#include "UART.h"
#include "Teclado4x4.h"

#define CANT_CUENTAS 3
#define MAX_INTENTOS 3
#define MAX_DIGITOS_IMPORTE 8

// Estructura de cada cuenta bancaria
typedef struct
{
	uint8_t uid[4];
	char pin[5];
	uint32_t saldo;
} Cuenta;

// Cuentas almacenadas en la EEPROM
static Cuenta cuentas_eeprom[CANT_CUENTAS] EEMEM;

// Marca para comprobar si la EEPROM fue inicializada
static uint16_t marca_eeprom EEMEM = 0xFFFF;

// UID, PIN y saldo iniciales de ejemplo
static const Cuenta cuentas_iniciales[CANT_CUENTAS] =
{
	{{0xC3, 0x6A, 0x58, 0x28}, "1234", 15000},
	{{0x11, 0x22, 0x33, 0x44}, "2345", 25000},
	{{0xC3, 0xFA, 0x58, 0x29}, "3456", 10000},
};

// Copia de las cuentas en la memoria RAM
static Cuenta cuentas[CANT_CUENTAS];

// UID de la tarjeta detectada
static uint8_t uid_leido[4];

// Inicializa las cuentas si la EEPROM está vacía
static void inicializar_cuentas(void)
{
	uint16_t marca;

	marca = eeprom_read_word(&marca_eeprom);

	if (marca != 0xA55A)
	{
		eeprom_update_block(
		cuentas_iniciales,
		cuentas_eeprom,
		sizeof(cuentas_iniciales)
		);

		eeprom_update_word(&marca_eeprom, 0xA55A);

		uart_print("Cuentas inicializadas en EEPROM.\r\n");
	}

	eeprom_read_block(
	cuentas,
	cuentas_eeprom,
	sizeof(cuentas)
	);

	// Actualiza solamente los UID, conservando PIN y saldos
	for (uint8_t i = 0; i < CANT_CUENTAS; i++)
	{
		eeprom_update_block(
		cuentas_iniciales[i].uid,
		cuentas_eeprom[i].uid,
		sizeof(cuentas[i].uid)
		);

		memcpy(
		cuentas[i].uid,
		cuentas_iniciales[i].uid,
		sizeof(cuentas[i].uid)
		);
	}
}

// Guarda las cuentas actualizadas en la EEPROM
static void guardar_cuentas(void)
{
	eeprom_update_block(
	cuentas,
	cuentas_eeprom,
	sizeof(cuentas)
	);
}

// Busca la cuenta asociada al UID
static int8_t buscar_cuenta(const uint8_t *uid)
{
	for (uint8_t i = 0; i < CANT_CUENTAS; i++)
	{
		uint8_t coincide = 1;

		for (uint8_t j = 0; j < 4; j++)
		{
			if (cuentas[i].uid[j] != uid[j])
			{
				coincide = 0;
				break;
			}
		}

		if (coincide)
		{
			return (int8_t)i;
		}
	}

	return -1;
}

// Muestra la pantalla inicial
static void mostrar_inicio(void)
{
	lcd_limpiar();
	lcd_posicion(0, 0);
	lcd_texto("CAJERO AUTOMAT.");
	lcd_posicion(1, 0);
	lcd_texto("Acerque tarjeta");
}

// Espera a que se retire la tarjeta
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

// Solicita y verifica el PIN de la cuenta
static uint8_t autenticar_cuenta(uint8_t indice)
{
	char pin_ingresado[5];
	char tecla;
	uint8_t cantidad;
	uint8_t intentos = 0;

	while (intentos < MAX_INTENTOS)
	{
		cantidad = 0;

		for (uint8_t i = 0; i < 5; i++)
		{
			pin_ingresado[i] = '\0';
		}

		lcd_limpiar();
		lcd_posicion(0, 0);
		lcd_texto("Ingrese su PIN:");
		lcd_posicion(1, 0);

		while (cantidad < 4)
		{
			tecla = teclado4x4_leerTecla();

			if (tecla >= '0' && tecla <= '9')
			{
				pin_ingresado[cantidad] = tecla;
				cantidad++;

				lcd_posicion(1, 0);

				for (uint8_t i = 0; i < cantidad; i++)
				{
					lcd_texto("*");
				}
			}
			else if (tecla == '*')
			{
				cantidad = 0;

				for (uint8_t i = 0; i < 5; i++)
				{
					pin_ingresado[i] = '\0';
				}

				lcd_posicion(1, 0);
				lcd_texto("                ");
				lcd_posicion(1, 0);
			}
		}

		pin_ingresado[4] = '\0';

		if (strcmp(pin_ingresado, cuentas[indice].pin) == 0)
		{
			lcd_limpiar();
			lcd_posicion(0, 0);
			lcd_texto("PIN correcto");
			lcd_posicion(1, 0);
			lcd_texto("Bienvenido");

			uart_print("Autenticacion correcta.\r\n");

			_delay_ms(1500);

			return 1;
		}

		intentos++;

		lcd_limpiar();
		lcd_posicion(0, 0);
		lcd_texto("PIN incorrecto");
		lcd_posicion(1, 0);

		if (intentos < MAX_INTENTOS)
		{
			lcd_texto("Intentelo otra vez");
		}
		else
		{
			lcd_texto("Cuenta bloqueada");
		}

		uart_print("PIN incorrecto.\r\n");

		_delay_ms(1500);
	}

	return 0;
}

// Muestra el saldo disponible
static void consultar_saldo(uint8_t indice)
{
	char texto[17];

	lcd_limpiar();
	lcd_posicion(0, 0);
	lcd_texto("Saldo disponible");

	snprintf(
	texto,
	sizeof(texto),
	"$%lu",
	(unsigned long)cuentas[indice].saldo
	);

	lcd_posicion(1, 0);
	lcd_texto(texto);

	uart_print("Saldo consultado: ");
	uart_print(texto);
	uart_print("\r\n");

	_delay_ms(2500);
}

// Solicita un importe utilizando el teclado
// Devuelve 1 si se confirma y 0 si se cancela
static uint8_t ingresar_importe(uint32_t *importe)
{
	char texto[MAX_DIGITOS_IMPORTE + 1];
	char tecla;
	uint8_t cantidad = 0;
	uint32_t valor = 0;

	for (uint8_t i = 0; i < sizeof(texto); i++)
	{
		texto[i] = '\0';
	}

	lcd_limpiar();
	lcd_posicion(0, 0);
	lcd_texto("Monto #OK *Canc");
	lcd_posicion(1, 0);

	while (1)
	{
		tecla = teclado4x4_leerTecla();

		if (tecla >= '0' && tecla <= '9')
		{
			if (cantidad < MAX_DIGITOS_IMPORTE)
			{
				texto[cantidad] = tecla;
				cantidad++;
				texto[cantidad] = '\0';

				lcd_posicion(1, 0);
				lcd_texto("                ");
				lcd_posicion(1, 0);
				lcd_texto(texto);
			}
		}
		else if (tecla == '*')
		{
			return 0;
		}
		else if (tecla == '#')
		{
			if (cantidad == 0)
			{
				lcd_limpiar();
				lcd_posicion(0, 0);
				lcd_texto("Importe invalido");
				_delay_ms(1200);
				return 0;
			}

			for (uint8_t i = 0; i < cantidad; i++)
			{
				valor = valor * 10 + (uint32_t)(texto[i] - '0');
			}

			*importe = valor;

			return 1;
		}
	}
}

// Retira dinero si existe saldo suficiente
static void retirar_dinero(uint8_t indice)
{
	uint32_t importe;

	if (!ingresar_importe(&importe))
	{
		lcd_limpiar();
		lcd_posicion(0, 0);
		lcd_texto("Retiro cancelado");
		_delay_ms(1200);
		return;
	}

	if (importe == 0)
	{
		lcd_limpiar();
		lcd_posicion(0, 0);
		lcd_texto("Importe invalido");
		_delay_ms(1500);
		return;
	}

	if (importe > cuentas[indice].saldo)
	{
		lcd_limpiar();
		lcd_posicion(0, 0);
		lcd_texto("Saldo insufic.");
		lcd_posicion(1, 0);
		lcd_texto("Retiro cancelado");

		uart_print("Saldo insuficiente.\r\n");

		_delay_ms(2000);
		return;
	}

	cuentas[indice].saldo -= importe;

	guardar_cuentas();

	lcd_limpiar();
	lcd_posicion(0, 0);
	lcd_texto("Retiro realizado");
	lcd_posicion(1, 0);
	lcd_texto("Saldo actualizado");

	uart_print("Retiro realizado.\r\n");

	_delay_ms(2000);
}

// Deposita dinero en la cuenta
static void depositar_dinero(uint8_t indice)
{
	uint32_t importe;

	if (!ingresar_importe(&importe))
	{
		lcd_limpiar();
		lcd_posicion(0, 0);
		lcd_texto("Deposito cancel.");
		_delay_ms(1200);
		return;
	}

	if (importe == 0)
	{
		lcd_limpiar();
		lcd_posicion(0, 0);
		lcd_texto("Importe invalido");
		_delay_ms(1500);
		return;
	}

	if (importe > UINT32_MAX - cuentas[indice].saldo)
	{
		lcd_limpiar();
		lcd_posicion(0, 0);
		lcd_texto("Importe excesivo");

		_delay_ms(1500);
		return;
	}

	cuentas[indice].saldo += importe;

	guardar_cuentas();

	lcd_limpiar();
	lcd_posicion(0, 0);
	lcd_texto("Deposito exitoso");
	lcd_posicion(1, 0);
	lcd_texto("Saldo actualizado");

	uart_print("Deposito realizado.\r\n");

	_delay_ms(2000);
}

// Muestra el menu principal
static void mostrar_menu(void)
{
	lcd_limpiar();

	lcd_posicion(0, 0);
	lcd_texto("1:Saldo 2:Retiro");

	lcd_posicion(1, 0);
	lcd_texto("3:Deposito 4:Fin");
}

// Ejecuta las operaciones de la cuenta autenticada
static void menu_cuenta(uint8_t indice)
{
	char tecla;
	uint8_t salir = 0;

	while (!salir)
	{
		mostrar_menu();

		tecla = '\0';

		while (tecla == '\0')
		{
			tecla = teclado4x4_leerTecla();
		}

		switch (tecla)
		{
			case '1':

			consultar_saldo(indice);

			break;

			case '2':

			retirar_dinero(indice);

			break;

			case '3':

			depositar_dinero(indice);

			break;

			case '4':

			lcd_limpiar();
			lcd_posicion(0, 0);
			lcd_texto("Sesion finalizada");
			lcd_posicion(1, 0);
			lcd_texto("Hasta pronto");

			uart_print("Sesion finalizada.\r\n");

			_delay_ms(1500);

			salir = 1;

			break;

			default:

			break;
		}
	}
}

// Programa principal
int main(void)
{
	int8_t indice;
	uint8_t tarjeta_detectada;

	// Inicializa los perifericos
	uart_init(103);

	i2c_iniciar();
	lcd_iniciar();

	spi_init();
	mfrc522_resetPinInit();
	mfrc522_init();

	teclado4x4_inicio();

	uart_print("\r\n====================\r\n");
	uart_print(" CAJERO AUTOMATICO      \r\n");
	uart_print("========================\r\n");

	// Carga las cuentas desde la EEPROM
	inicializar_cuentas();

	while (1)
	{
		mostrar_inicio();

		tarjeta_detectada = 0;

		// Espera una tarjeta RFID
		while (!tarjeta_detectada)
		{
			if (mfrc522_standard(uid_leido))
			{
				tarjeta_detectada = 1;
			}
		}

		uart_print("\r\nTarjeta detectada. UID: ");
		uart_print_hex_array(uid_leido, 4);
		uart_print("\r\n");

		// Busca la cuenta asociada a la tarjeta
		indice = buscar_cuenta(uid_leido);

		if (indice < 0)
		{
			lcd_limpiar();
			lcd_posicion(0, 0);
			lcd_texto("Tarjeta invalida");
			lcd_posicion(1, 0);
			lcd_texto("Acceso denegado");

			uart_print("Tarjeta no registrada.\r\n");

			_delay_ms(2000);

			esperar_retirar_tarjeta();

			continue;
		}

		uart_print("Cuenta identificada.\r\n");

		// Verifica el PIN de la cuenta
		if (!autenticar_cuenta((uint8_t)indice))
		{
			lcd_limpiar();
			lcd_posicion(0, 0);
			lcd_texto("Acceso denegado");
			lcd_posicion(1, 0);
			lcd_texto("Retire tarjeta");

			esperar_retirar_tarjeta();

			continue;
		}

		// Accede al menu de operaciones bancarias
		menu_cuenta((uint8_t)indice);

		// Finaliza la sesion antes de aceptar otra tarjeta
		esperar_retirar_tarjeta();
	}

	return 0;
}
