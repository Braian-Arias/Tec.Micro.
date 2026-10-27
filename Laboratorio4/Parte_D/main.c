
#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdlib.h>

#include "config.h"
#include "Buzzer.h"
#include "Joystick.h"
#include "Matriz.h"

// Longitud máxima de la secuencia
#define MAX_SECUENCIA 32

// Direcciones de EEPROM
#define EEPROM_MAGIC       0
#define EEPROM_RECORD      1
#define EEPROM_ULTIMO_NIVEL 2

#define MAGIC_VALOR 0xA5

// Datos de cada paso de la secuencia
typedef struct
{
	uint8_t fila;
	uint8_t columna;
	uint8_t color;
} Paso;

static Paso secuencia[MAX_SECUENCIA];

static uint8_t nivel = 1;
static uint8_t record = 0;
static uint8_t ultimo_nivel = 1;

static uint8_t cursor_fila = 3;
static uint8_t cursor_columna = 3;
static uint8_t color_cursor = COLOR_ROJO;

// Frecuencias asociadas a los colores
static const uint16_t tonos[8] =
{
	0,
	NOTA_DO,
	NOTA_MI,
	NOTA_SOL,
	NOTA_RE,
	NOTA_LA,
	NOTA_SI,
	NOTA_DO2
};

// Inicializar UART a 9600 baudios
static void uart_inicializar(void)
{
	UBRR0H = 0;
	UBRR0L = 103;

	UCSR0A = 0;
	UCSR0B = (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Enviar un carácter por UART
static void uart_caracter(char c)
{
	while (!(UCSR0A & (1 << UDRE0)))
	{
	}

	UDR0 = c;
}

// Enviar texto por UART
static void uart_texto(const char *texto)
{
	while (*texto)
	{
		uart_caracter(*texto++);
	}
}

// Mostrar un número decimal por UART
static void uart_numero(uint8_t numero)
{
	char digitos[4];
	uint8_t i = 0;

	if (numero == 0)
	{
		uart_caracter('0');
		return;
	}

	while (numero > 0)
	{
		digitos[i++] = '0' + (numero % 10);
		numero /= 10;
	}

	while (i > 0)
	{
		uart_caracter(digitos[--i]);
	}
}

// Mostrar información del juego
static void mostrar_estado(void)
{
	uart_texto("\r\nNivel actual: ");
	uart_numero(nivel);

	uart_texto("\r\nRecord historico: ");
	uart_numero(record);

	uart_texto("\r\nUltimo nivel guardado: ");
	uart_numero(ultimo_nivel);

	uart_texto("\r\n");
}

// Cargar los datos persistentes
static void eeprom_cargar_datos(void)
{
	if (eeprom_read_byte((const uint8_t *)EEPROM_MAGIC) != MAGIC_VALOR)
	{
		// Primera ejecución: inicializar los datos
		eeprom_update_byte((uint8_t *)EEPROM_RECORD, 0);
		eeprom_update_byte((uint8_t *)EEPROM_ULTIMO_NIVEL, 1);
		eeprom_update_byte((uint8_t *)EEPROM_MAGIC, MAGIC_VALOR);
	}

	record = eeprom_read_byte((const uint8_t *)EEPROM_RECORD);
	ultimo_nivel = eeprom_read_byte((const uint8_t *)EEPROM_ULTIMO_NIVEL);

	// Validar datos guardados
	if (record > MAX_SECUENCIA)
	{
		record = 0;
		eeprom_update_byte((uint8_t *)EEPROM_RECORD, record);
	}

	if (ultimo_nivel < 1 || ultimo_nivel > MAX_SECUENCIA)
	{
		ultimo_nivel = 1;
		eeprom_update_byte((uint8_t *)EEPROM_ULTIMO_NIVEL, ultimo_nivel);
	}
}

// Inicializar el generador pseudoaleatorio
static void aleatorio_inicializar(void)
{
	uint16_t semilla = 0;

	// Tomar pequeñas variaciones de las entradas analógicas
	for (uint8_t i = 0; i < 16; i++)
	{
		semilla = (uint16_t)(semilla * 33U + joystick_leer_x());
		_delay_ms(2);
	}

	if (semilla == 0)
	{
		semilla = 1234;
	}

	srand(semilla);
}

// Generar un paso aleatorio
static Paso generar_paso(void)
{
	Paso paso;

	paso.fila = rand() % 8;
	paso.columna = rand() % 8;

	// Colores del 1 al 7
	paso.color = 1 + (rand() % 7);

	return paso;
}

// Generar la secuencia correspondiente al nivel
static void generar_secuencia(void)
{
	for (uint8_t i = 0; i < nivel; i++)
	{
		secuencia[i] = generar_paso();
	}
}

// Dibujar solamente el cursor
static void dibujar_cursor(void)
{
	matriz_limpiar();

	matriz_pixel(cursor_fila, cursor_columna, color_cursor);

	matriz_actualizar();
}

// Reproducir un tono
// Reproducir un tono durante la cantidad de milisegundos indicada
static void sonido(uint16_t frecuencia, uint16_t duracion)
{
	reproducir_nota(frecuencia);

	for (uint16_t i = 0; i < duracion; i++)
	{
		_delay_ms(1);
	}

	buzzer_detener();
}

// Reproducir la secuencia del sistema
static void reproducir_secuencia(void)
{
	matriz_limpiar();
	matriz_actualizar();
	_delay_ms(400);

	for (uint8_t i = 0; i < nivel; i++)
	{
		matriz_limpiar();

		matriz_pixel(
		secuencia[i].fila,
		secuencia[i].columna,
		secuencia[i].color
		);

		matriz_actualizar();

		sonido(tonos[secuencia[i].color], 500);

		matriz_limpiar();
		matriz_actualizar();

		_delay_ms(200);
	}
}

// Esperar hasta que el botón se suelte
static void esperar_liberacion_boton(void)
{
	while (joystick_boton_presionado())
	{
		_delay_ms(10);
	}

	_delay_ms(30);
}

// Modo libre: mover cursor y cambiar su color
static void modo_libre(void)
{
	uart_texto("\r\nModo libre: mueve el joystick o pulsa SW para cambiar color.\r\n");

	// Ventana de preparación para el siguiente nivel
	for (uint8_t tiempo = 0; tiempo < 20; tiempo++)
	{
		joystick_actualizar_cursor(&cursor_fila, &cursor_columna);
		dibujar_cursor();

		if (joystick_boton_presionado())
		{
			_delay_ms(30);

			if (joystick_boton_presionado())
			{
				color_cursor = 1 + (rand() % 7);
				sonido(tonos[color_cursor], 100);
				esperar_liberacion_boton();
			}
		}

		_delay_ms(50);
	}
}

// Leer la respuesta del jugador
static uint8_t jugar_secuencia(void)
{
	uint8_t paso_actual = 0;

	uart_texto("\r\nTu turno. Repite las posiciones.\r\n");

	while (paso_actual < nivel)
	{
		joystick_actualizar_cursor(&cursor_fila, &cursor_columna);
		dibujar_cursor();

		if (joystick_boton_presionado())
		{
			_delay_ms(30);

			if (joystick_boton_presionado())
			{
				// Comparar la posición del cursor con el paso esperado
				if (cursor_fila == secuencia[paso_actual].fila &&
				cursor_columna == secuencia[paso_actual].columna)
				{
					sonido(tonos[secuencia[paso_actual].color], 100);

					paso_actual++;

					uart_texto("Acierto: ");
					uart_numero(paso_actual);
					uart_texto("\r\n");
				}
				else
				{
					uart_texto("Posicion incorrecta.\r\n");
					esperar_liberacion_boton();
					return 0;
				}

				esperar_liberacion_boton();
			}
		}

		_delay_ms(80);
	}

	return 1;
}

// Guardar el récord y el nivel alcanzado
static void guardar_progreso(void)
{
	ultimo_nivel = nivel;

	eeprom_update_byte(
	(uint8_t *)EEPROM_ULTIMO_NIVEL,
	ultimo_nivel
	);

	if (nivel > record)
	{
		record = nivel;

		eeprom_update_byte(
		(uint8_t *)EEPROM_RECORD,
		record
		);

		uart_texto("Nuevo record historico.\r\n");
	}
}

// Animación y sonido de error
static void error_juego(void)
{
	matriz_animacion_error();

	// Tono grave de error
	sonido(120, 500);

	uart_texto("Juego reiniciado. Nivel 1.\r\n");

	nivel = 1;

	eeprom_update_byte(
	(uint8_t *)EEPROM_ULTIMO_NIVEL,
	nivel
	);

	ultimo_nivel = nivel;

	_delay_ms(500);
}

// Animación de éxito
static void exito_juego(void)
{
	uart_texto("Nivel superado.\r\n");

	matriz_animacion_exito();
	sonido(NOTA_DO2, 250);

	guardar_progreso();

	if (nivel < MAX_SECUENCIA)
	{
		nivel++;
	}
	else
	{
		uart_texto("Longitud maxima alcanzada.\r\n");
	}

	mostrar_estado();
}

// Programa principal
int main(void)
{
	joystick_inicializar();
	buzzer_inicializar();
	matriz_inicializar();
	uart_inicializar();

	eeprom_cargar_datos();
	aleatorio_inicializar();

	uart_texto("\r\n=== SIMON DICE ===\r\n");
	mostrar_estado();

	// La partida nueva comienza en nivel 1.
	// El récord y el último nivel alcanzado se conservan.
	nivel = 1;

	while (1)
	{
		modo_libre();

		generar_secuencia();
		reproducir_secuencia();

		if (jugar_secuencia())
		{
			exito_juego();
		}
		else
		{
			error_juego();
		}

		_delay_ms(300);
	}
}

