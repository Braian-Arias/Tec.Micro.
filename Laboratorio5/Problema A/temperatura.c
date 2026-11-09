#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "temperatura.h"
#include "temporizadores.h"

// DHT11 conectado a A1 / PC1
#define DHT_PORT PORTC
#define DHT_DDR  DDRC
#define DHT_PIN  PINC
#define DHT_BIT  PC1

// Flancos esperados: 3 de respuesta + 2 por cada uno de los 40 bits
#define BORDES_TOTALES   83
// Timer1 cuenta a 250 kHz (4 us por cuenta). 13 cuentas = 52 us
#define UMBRAL_TICKS     13
// Timer1 se reinicia cada 25000 cuentas (OCR1A = 24999)
#define TICKS_PERIODO    25000UL

// Estados de la lectura (sin switch-case)
#define DHT_REPOSO       0
#define DHT_INICIO       1
#define DHT_ESPERA       2
#define DHT_DECODIFICAR  3

// Temperatura de la última lectura válida
static int16_t temperatura_valor = 0;

// Instante (TCNT1) de cada flanco capturado por la ISR
static volatile uint16_t tiempos_borde[BORDES_TOTALES];
static volatile uint8_t num_bordes = 0;

static uint8_t estado_dht = DHT_REPOSO;
static uint32_t marca_ms = 0;

// ISR de PCINT1 (grupo PC): guarda el instante de cada flanco de PC1
ISR(PCINT1_vect)
{
	// El primer flanco debe ser de bajada (respuesta del sensor).
	// Se descarta la subida del pull-up al liberar el bus.
	if (num_bordes == 0 && (DHT_PIN & (1 << DHT_BIT)))
	{
		return;
	}

	if (num_bordes < BORDES_TOTALES)
	{
		tiempos_borde[num_bordes] = TCNT1;
		num_bordes++;
	}
}

// Convierte los flancos capturados en 40 bits y valida el checksum
static void temp_decodificar(void)
{
	uint8_t datos[5] = {0, 0, 0, 0, 0};

	for (uint8_t i = 0; i < 40; i++)
	{
		// Bit i: subida en 3+2i, bajada en 4+2i
		uint16_t inicio = tiempos_borde[3 + 2 * i];
		uint16_t fin    = tiempos_borde[4 + 2 * i];

		uint16_t ticks = (fin >= inicio)
		? (uint16_t)(fin - inicio)
		: (uint16_t)(fin + TICKS_PERIODO - inicio);

		datos[i / 8] <<= 1;

		// Pulso alto largo (~70 us) = 1; corto (~27 us) = 0
		if (ticks > UMBRAL_TICKS)
		{
			datos[i / 8] |= 1;
		}
	}

	uint8_t suma = (uint8_t)(datos[0] + datos[1] + datos[2] + datos[3]);

	if (suma == datos[4])
	{
		// DHT11: el byte 2 contiene los grados enteros
		temperatura_valor = datos[2];
	}
	// Si falla el checksum se conserva la última lectura válida
}

void temp_inicializar(void)
{
	// PC1 como entrada con pull-up
	DHT_DDR &= ~(1 << DHT_BIT);
	DHT_PORT |= (1 << DHT_BIT);

	// Habilitar el grupo PCINT1; el pin se habilita solo al esperar datos
	PCICR |= (1 << PCIE1);
	PCMSK1 &= ~(1 << PCINT9);

	temperatura_valor = 0;
	estado_dht = DHT_REPOSO;
}

// Solo inicia la lectura: pone el pulso de inicio y retorna sin esperar
uint8_t temp_iniciar_lectura(void)
{
	if (estado_dht != DHT_REPOSO)
	{
		return 0;   // ya hay una lectura en curso
	}

	// Pulso de inicio: PC1 como salida en nivel bajo
	DHT_DDR |= (1 << DHT_BIT);
	DHT_PORT &= ~(1 << DHT_BIT);

	marca_ms = tmr_obtener_tiempo_ms();
	estado_dht = DHT_INICIO;

	return 1;
}

// Llamar en cada vuelta de main: avanza la lectura sin bloquear
void temp_actualizar(void)
{
	uint32_t ahora = tmr_obtener_tiempo_ms();
	uint32_t transcurrido = ahora - marca_ms;

	if (estado_dht == DHT_INICIO)
	{
		// Mantener el pulso bajo al menos 18 ms (aquí 20 ms)
		if (transcurrido >= 20)
		{
			// Liberar el bus y activar la captura de flancos
			DHT_DDR &= ~(1 << DHT_BIT);
			DHT_PORT |= (1 << DHT_BIT);

			num_bordes = 0;
			PCIFR = (1 << PCIF1);       // limpiar bandera pendiente
			PCMSK1 |= (1 << PCINT9);

			marca_ms = ahora;
			estado_dht = DHT_ESPERA;
		}
	}
	else if (estado_dht == DHT_ESPERA)
	{
		if (num_bordes >= BORDES_TOTALES)
		{
			PCMSK1 &= ~(1 << PCINT9);
			estado_dht = DHT_DECODIFICAR;
		}
		else if (transcurrido >= 10)
		{
			// Timeout: sensor ausente o trama incompleta
			PCMSK1 &= ~(1 << PCINT9);
			estado_dht = DHT_REPOSO;
		}
	}
	else if (estado_dht == DHT_DECODIFICAR)
	{
		temp_decodificar();
		estado_dht = DHT_REPOSO;
	}
}

int16_t temp_obtener(void)
{
	return temperatura_valor;
}