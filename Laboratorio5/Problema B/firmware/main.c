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
#include "energia.h"
#include "uart.h"

// ============================================
// LABORATORIO 5 - MONITOREO MATLAB
// ATmega328P - 16 MHz
// UART 9600 BAUDIOS
// ============================================

// CONSTANTES
#define BUTTON_DEBOUNCE_MS       50UL
#define BUZZER_DURATION_MS       150UL
#define LCD_INTERVAL_MS          500UL
#define TEMPERATURE_INTERVAL_MS  2000UL
#define UART_INTERVAL_MS         500UL

// ============================================
// VARIABLES DEL SISTEMA
// ============================================

static uint32_t lcd_last_time_ms = 0;
static uint32_t temperature_last_time_ms = 0;
static uint32_t button_last_time_ms = 0;
static uint32_t buzzer_start_time_ms = 0;
static uint32_t uart_last_time_ms = 0;

static uint16_t adc_display_value = 0;

static uint8_t buzzer_active = 0;
static uint8_t button_initialized = 0;

// ============================================
// CORRIENTE DE REFERENCIA
// ============================================

// Son valores medidos anteriormente.
// NO son mediciones instantaneas.

static uint16_t obtener_corriente_referencia(
SystemState estado)
{
	if (estado == SYSTEM_ACTIVO)
	{
		return 2130;  // 21.30 mA
	}
	else if (estado == SYSTEM_STANDBY)
	{
		return 2070;  // 20.70 mA
	}
	else
	{
		return 778;   // 7.78 mA
	}
}

// ============================================
// ENVIO DE DATOS A MATLAB
// ============================================

static void enviar_datos_matlab(
uint32_t tiempo,
SystemState estado)
{
	uint16_t corriente;

	corriente = obtener_corriente_referencia(
	estado
	);

	// Formato:
	// tiempo_ms,ESTADO,corriente_mA

	uart_enviar_matlab(
	tiempo,
	(uint8_t)estado,
	corriente
	);
}

// ============================================
// ACTUALIZACION LCD
// ============================================

static void actualizar_pantalla(void)
{
	SystemState estado = fsm_obtener_estado();

	// ESTADO ACTIVO
	if (estado == SYSTEM_ACTIVO)
	{
		lcd_limpiar_linea(0);
		lcd_limpiar_linea(1);

		lcd_posicion(0, 0);
		lcd_texto("ACTIVE ADC:");
		lcd_numero(adc_display_value);

		lcd_posicion(1, 0);
		lcd_texto("TEMP:");
		lcd_numero((uint16_t)temp_obtener());
		lcd_texto(" C");
	}

	// ESTADO STANDBY
	else if (estado == SYSTEM_STANDBY)
	{
		lcd_limpiar_linea(0);
		lcd_limpiar_linea(1);

		lcd_posicion(0, 0);
		lcd_texto("STANDBY");

		lcd_posicion(1, 0);
		lcd_texto("Esperando...");
	}

	// ESTADO APAGADO
	else if (estado == SYSTEM_APAGADO)
	{
		lcd_limpiar_linea(0);
		lcd_limpiar_linea(1);

		lcd_posicion(0, 0);
		lcd_texto("SYSTEM OFF");

		lcd_posicion(1, 0);
		lcd_texto("PRESS TO WAKE");
	}
}

// ============================================
// CONTROL DEL BUZZER
// ============================================

static void activar_buzzer(uint32_t tiempo)
{
	reproducir_nota(NOTA_DO2);

	buzzer_active = 1;
	buzzer_start_time_ms = tiempo;
}

static void actualizar_buzzer(uint32_t tiempo)
{
	if (buzzer_active)
	{
		if ((uint32_t)(
		tiempo - buzzer_start_time_ms
		) >= BUZZER_DURATION_MS)
		{
			buzzer_detener();
			buzzer_active = 0;
		}
	}
}

// ============================================
// PROGRAMA PRINCIPAL
// ============================================

int main(void)
{
	uint32_t current_time;

	uint8_t user_activity;

	uint16_t adc_result;

	SystemState estado_antes;
	SystemState estado_actual;

	// ========================================
	// INICIALIZACION DE MODULOS
	// ========================================

	es_inicializar();

	buzzer_inicializar();

	adc_inicializar();

	temp_inicializar();

	// I2C y LCD
	i2c_iniciar();
	lcd_iniciar();

	// Temporizadores
	tmr_inicializar();

	// UART para MATLAB
	uart_inicializar();

	// Inicializar FSM
	current_time = tmr_obtener_tiempo_ms();

	fsm_inicializar(current_time);

	// Tiempos iniciales
	lcd_last_time_ms = current_time;

	temperature_last_time_ms = current_time;

	uart_last_time_ms = current_time;

	// Mostrar LCD inicial
	actualizar_pantalla();

	// Enviar primer estado a MATLAB
	enviar_datos_matlab(
	current_time,
	fsm_obtener_estado()
	);

	// ========================================
	// BUCLE PRINCIPAL
	// ========================================

	while (1)
	{
		estado_antes = fsm_obtener_estado();

		// ====================================
		// 1. WATCHDOG EN APAGADO
		// ====================================

		if (energia_obtener_evento_wdt() &&
		estado_antes == SYSTEM_APAGADO)
		{
			// Habilitar ADC temporalmente
			PRR &= ~(1 << PRADC);

			ADCSRA |= (1 << ADEN);

			adc_iniciar_conversion();

			while (ADCSRA & (1 << ADSC))
			{
			}

			adc_display_value = ADC;

			// Deshabilitar nuevamente ADC
			ADCSRA &= ~(1 << ADEN);

			PRR |= (1 << PRADC);
		}

		// ====================================
		// 2. TEMPERATURA DHT11
		// ====================================

		if (estado_antes != SYSTEM_APAGADO)
		{
			temp_actualizar();
		}

		current_time = tmr_obtener_tiempo_ms();

		user_activity = 0;

		// ====================================
		// 3. PULSADOR
		// ====================================

		if (tmr_obtener_evento_boton())
		{
			if (!button_initialized ||
			(uint32_t)(
			current_time - button_last_time_ms
			) >= BUTTON_DEBOUNCE_MS)
			{
				button_initialized = 1;

				button_last_time_ms = current_time;

				user_activity = 1;

				activar_buzzer(current_time);
				buzzer_Activar();
			}
		}

		// ====================================
		// 4. SOLICITUD ADC
		// ====================================

		if (tmr_obtener_solicitud_adc())
		{
			if (estado_antes != SYSTEM_APAGADO)
			{
				adc_iniciar_conversion();
			}
		}

		// ====================================
		// 5. PROCESAR ADC
		// ====================================

		if (adc_obtener_resultado(&adc_result))
		{
			adc_display_value = adc_result;
		}

		// ====================================
		// 6. ACTUALIZAR FSM
		// ====================================

		fsm_actualizar(
		current_time,
		user_activity
		);

		estado_actual = fsm_obtener_estado();

		// ====================================
		// 7. CAMBIO DE ESTADO
		// ====================================

		if (estado_antes != estado_actual)
		{
			// Restaurar perifericos al despertar
			if (estado_antes == SYSTEM_APAGADO)
			{
				energia_despertar_perifericos();

				// Restaurar UART
				uart_inicializar();
			}

			// Actualizar pantalla
			actualizar_pantalla();

			// Enviar cambio a MATLAB ANTES
			// de deshabilitar perifericos
			enviar_datos_matlab(
			current_time,
			estado_actual
			);

			uart_last_time_ms = current_time;

			// Aplicar configuracion energetica
			energia_aplicar_estado(estado_actual);
		}

		// ====================================
		// 8. LED
		// ====================================

		es_actualizar_led(
		current_time,
		fsm_obtener_estado()
		);

		if (fsm_obtener_estado() == SYSTEM_APAGADO)
		{
			es_apagar_led();
		}

		// ====================================
		// 9. BUZZER
		// ====================================

		actualizar_buzzer(current_time);

		// ====================================
		// 10. LECTURA DE TEMPERATURA
		// ====================================

		if (fsm_obtener_estado() == SYSTEM_ACTIVO &&
		(uint32_t)(
		current_time - temperature_last_time_ms
		) >= TEMPERATURE_INTERVAL_MS)
		{
			temperature_last_time_ms = current_time;

			temp_iniciar_lectura();
		}

		// ====================================
		// 11. ACTUALIZACION LCD
		// ====================================

		if (fsm_obtener_estado() != SYSTEM_APAGADO &&
		(uint32_t)(
		current_time - lcd_last_time_ms
		) >= LCD_INTERVAL_MS)
		{
			lcd_last_time_ms = current_time;

			actualizar_pantalla();
		}

		// ====================================
		// 12. ENVIO PERIODICO A MATLAB
		// ====================================

		estado_actual = fsm_obtener_estado();

		if (estado_actual != SYSTEM_APAGADO &&
		(uint32_t)(
		current_time - uart_last_time_ms
		) >= UART_INTERVAL_MS)
		{
			uart_last_time_ms = current_time;

			enviar_datos_matlab(
			current_time,
			estado_actual
			);
		}

		// ====================================
		// 13. MODOS SLEEP
		// ====================================

		energia_dormir(
		fsm_obtener_estado()
		);
	}
}
