#include <avr/io.h>
#include <stdint.h>

#include "entradas_salidas.h"
#include "maquina_estados.h"

// Estado interno del LED
static uint8_t led_encendido = 0;

// Último instante en que cambió el LED
static uint32_t led_ultimo_cambio_ms = 0;

// Estado anterior de la FSM
static SystemState estado_anterior = SYSTEM_ACTIVO;

void es_inicializar(void)
{
	// LED en D13 / PB5
	DDRB |= (1 << DDB5);
	PORTB &= ~(1 << PORTB5);
	led_encendido = 0;
}

void es_encender_led(void)
{
	PORTB |= (1 << PORTB5);
	led_encendido = 1;
}

void es_apagar_led(void)
{
	PORTB &= ~(1 << PORTB5);
	led_encendido = 0;
}

void es_actualizar_led(uint32_t tiempo_actual, SystemState estado)
{
	// Al cambiar de estado, apagar el LED y reiniciar la temporización
	if (estado != estado_anterior)
	{
		estado_anterior = estado;
		led_ultimo_cambio_ms = tiempo_actual;
		es_apagar_led();
		return;
	}

	// En APAGADO el LED permanece apagado
	if (estado == SYSTEM_APAGADO)
	{
		es_apagar_led();
		return;
	}

	// En ACTIVO y STANDBY cambia cada 200 ms
	if ((uint32_t)(tiempo_actual - led_ultimo_cambio_ms) >= 200UL)
	{
		led_ultimo_cambio_ms = tiempo_actual;

		if (led_encendido)
		{
			es_apagar_led();
		}
		else
		{
			es_encender_led();
		}
	}
}