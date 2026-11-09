#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <stdint.h>

#include "temporizadores.h"
#include "adc.h"
#include "entradas_salidas.h"
#include "maquina_estados.h"
#include "temperatura.h"
#include "LCD_I2C.h"
#include "Buzzer.h"


// ------------------------------------
// CONSTANTES DE TEMPORIZACIÓN
// ------------------------------------

#define BUTTON_DEBOUNCE_MS       50UL
#define BUZZER_DURATION_MS       150UL
#define LCD_INTERVAL_MS          500UL
#define TEMPERATURE_INTERVAL_MS  2000UL


// ------------------------------------
// VARIABLES DEL SISTEMA
// ------------------------------------

// Última actualización de la LCD
static uint32_t lcd_last_time_ms = 0;

// Última solicitud de lectura del DHT11
static uint32_t temperature_last_time_ms = 0;

// Última pulsación válida
static uint32_t button_last_time_ms = 0;

// Inicio de reproducción del buzzer
static uint32_t buzzer_start_time_ms = 0;

// Valor ADC mostrado en pantalla
static uint16_t adc_display_value = 0;

// Estado del buzzer
static uint8_t buzzer_active = 0;

// Control de la primera pulsación
static uint8_t button_initialized = 0;


// ------------------------------------
// ACTUALIZACIÓN DE LA LCD
// ------------------------------------

static void actualizar_pantalla(void)
{
	SystemState current_state = fsm_obtener_estado();

	// --------------------------------
	// ESTADO ACTIVO
	// --------------------------------

	if (current_state == SYSTEM_ACTIVO)
	{
		lcd_limpiar_linea(0);
		lcd_limpiar_linea(1);

		// Primera línea
		lcd_posicion(0, 0);
		lcd_texto("ACTIVE ADC:");
		lcd_numero(adc_display_value);

		// Segunda línea
		lcd_posicion(1, 0);
		lcd_texto("TEMP:");
		lcd_numero((uint16_t)temp_obtener());
		lcd_texto(" C");
	}


	// --------------------------------
	// ESTADO STANDBY
	// --------------------------------

	else if (current_state == SYSTEM_STANDBY)
	{
		lcd_limpiar_linea(0);
		lcd_limpiar_linea(1);

		lcd_posicion(0, 0);
		lcd_texto("STANDBY");

		lcd_posicion(1, 0);
		lcd_texto("Esperando...");
	}


	// --------------------------------
	// ESTADO APAGADO
	// --------------------------------

	else if (current_state == SYSTEM_APAGADO)
	{
		lcd_limpiar_linea(0);
		lcd_limpiar_linea(1);

		lcd_posicion(0, 0);
		lcd_texto("SYSTEM OFF");

		lcd_posicion(1, 0);
		lcd_texto("PRESS TO WAKE");
	}
}


// ------------------------------------
// CONTROL DEL BUZZER
// ------------------------------------

static void activar_buzzer(uint32_t current_time)
{
	// Reproducir nota DO de la segunda octava
	reproducir_nota(NOTA_DO2);

	buzzer_active = 1;

	buzzer_start_time_ms = current_time;
}


static void actualizar_buzzer(uint32_t current_time)
{
	if (buzzer_active)
	{
		// Detener el sonido después de 150 ms
		if ((uint32_t)(current_time - buzzer_start_time_ms)
		>= BUZZER_DURATION_MS)
		{
			buzzer_detener();

			buzzer_active = 0;
		}
	}
}


// ------------------------------------
// PROGRAMA PRINCIPAL
// ------------------------------------

int main(void)
{
	uint32_t current_time;

	uint8_t user_activity = 0;

	uint16_t adc_result;


	// --------------------------------
	// INICIALIZACIÓN DE LOS MÓDULOS
	// --------------------------------

	es_inicializar();

	buzzer_inicializar();

	adc_inicializar();

	temp_inicializar();

	// Inicialización de la comunicación I2C
	i2c_iniciar();

	// Inicialización de la pantalla LCD
	lcd_iniciar();

	// Inicialización de los temporizadores
	tmr_inicializar();

	// Inicialización de la FSM
	current_time = tmr_obtener_tiempo_ms();

	fsm_inicializar(current_time);

	// Inicializar tiempos de las tareas
	lcd_last_time_ms = current_time;

	temperature_last_time_ms = current_time;

	// Mostrar estado inicial
	actualizar_pantalla();


	// --------------------------------
	// BUCLE PRINCIPAL
	// --------------------------------

	while (1)
	{
		// Avanza la lectura del DHT11 sin bloquear
		temp_actualizar();

		// Obtener tiempo actual
		current_time = tmr_obtener_tiempo_ms();

		// Reiniciar indicador de actividad
		user_activity = 0;


		// --------------------------------
		// 1. PROCESAMIENTO DEL PULSADOR
		// --------------------------------

		if (tmr_obtener_evento_boton())
		{
			// La primera pulsación se acepta directamente
			// Las siguientes deben respetar el antirrebote
			if (!button_initialized ||
			(uint32_t)(current_time - button_last_time_ms)
			>= BUTTON_DEBOUNCE_MS)
			{
				button_initialized = 1;

				button_last_time_ms = current_time;

				// Registrar actividad del usuario
				user_activity = 1;

				// Activar el buzzer
				activar_buzzer(current_time);
			}
		}


		// --------------------------------
		// 2. SOLICITUD DE CONVERSIÓN ADC
		// --------------------------------

		if (tmr_obtener_solicitud_adc())
		{
			adc_iniciar_conversion();
		}


		// --------------------------------
		// 3. PROCESAMIENTO DEL ADC
		// --------------------------------

		if (adc_obtener_resultado(&adc_result))
		{
			adc_display_value = adc_result;
		}


		// --------------------------------
		// 4. ACTUALIZACIÓN DE LA FSM
		// --------------------------------

		fsm_actualizar(
		current_time,
		user_activity
		);


		// --------------------------------
		// 5. ACTUALIZACIÓN DEL LED
		// --------------------------------

		es_actualizar_led(
		current_time,
		fsm_obtener_estado()
		);


		// --------------------------------
		// 6. CONTROL DEL BUZZER
		// --------------------------------

		actualizar_buzzer(current_time);


		// --------------------------------
		// 7. SOLICITUD DE LECTURA DE TEMPERATURA
		// --------------------------------

		if ((uint32_t)(current_time - temperature_last_time_ms)
		>= TEMPERATURE_INTERVAL_MS)
		{
			temperature_last_time_ms = current_time;

			// Solo inicia la lectura; temp_actualizar() la completa
			temp_iniciar_lectura();
		}


		// --------------------------------
		// 8. ACTUALIZACIÓN DE LA LCD
		// --------------------------------

		if ((uint32_t)(current_time - lcd_last_time_ms)
		>= LCD_INTERVAL_MS)
		{
			lcd_last_time_ms = current_time;

			actualizar_pantalla();
		}
	}
}