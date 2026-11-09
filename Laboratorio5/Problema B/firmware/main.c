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
#include "bluetooth.h"
#include "energia.h"


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
static uint8_t emergencia = 0, alarmas = 0;
static int8_t led_override = -1;


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
	uint8_t cmd, dato;
	SystemState estado_antes;


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
	bt_inicializar();

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
        estado_antes = fsm_obtener_estado();
        if (energia_obtener_evento_wdt() && estado_antes == SYSTEM_APAGADO) {
            // ADC0 se lee después del despertar y se vuelve a deshabilitar.
            PRR &= ~(1 << PRADC);
            ADCSRA |= (1 << ADEN);
            adc_iniciar_conversion();
            while (ADCSRA & (1 << ADSC)) { }
            adc_display_value = ADC;
            ADCSRA &= ~(1 << ADEN);
            PRR |= (1 << PRADC);
        }
		// Avanza la lectura del DHT11 sin bloquear
		if (estado_antes != SYSTEM_APAGADO) temp_actualizar();

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
				emergencia = 0; // recuperación por pulsador físico

				// Activar el buzzer
				activar_buzzer(current_time);
			}
		}


		// --------------------------------
		// 2. SOLICITUD DE CONVERSIÓN ADC
		// --------------------------------

		if (tmr_obtener_solicitud_adc())
		{
			if (estado_antes != SYSTEM_APAGADO) adc_iniciar_conversion();
		}


		// --------------------------------
		// 3. PROCESAMIENTO DEL ADC
		// --------------------------------

		if (adc_obtener_resultado(&adc_result))
		{
			adc_display_value = adc_result;
		}


		// Procesar comandos Bluetooth fuera de la ISR.
        if (estado_antes != SYSTEM_APAGADO) {
            bt_procesar(current_time);
            if (bt_obtener_comando(&cmd, &dato)) {
                if (cmd == BT_ESTADO && dato == 0)
                    bt_enviar(0x81, (uint8_t)fsm_obtener_estado());
                else if (cmd == BT_SENSORES && dato == 0) {
                    bt_enviar(0x82, (uint8_t)(adc_display_value >> 2));
                    bt_enviar(0xC2, (uint8_t)temp_obtener());
                } else if (cmd == BT_ALARMAS && dato == 0)
                    bt_enviar(0x86, alarmas);
                else if (cmd == BT_EMERGENCIA && dato == 1) {
                    emergencia = 1;
                    alarmas++;
                    buzzer_detener();
                    bt_enviar(0x85, 1);
                    fsm_fijar_estado(SYSTEM_APAGADO, current_time);
                } else if (emergencia) bt_enviar(BT_ERROR, 5);
                else if (cmd == BT_CAMBIAR && dato <= 2) {
                    bt_enviar(0x83, dato);
                    fsm_fijar_estado((SystemState)dato, current_time);
                    if (dato == 0) user_activity = 1;
                } else if (cmd == BT_ACTUADOR && dato <= 1) {
                    led_override = (int8_t)dato;
                    if (dato) es_encender_led(); else es_apagar_led();
                    user_activity = 1;
                    bt_enviar(0x84, dato);
                } else bt_enviar(BT_ERROR, 4);
            }
        }

        // --------------------------------
		// 4. ACTUALIZACIÓN DE LA FSM
		// --------------------------------

		fsm_actualizar(current_time, user_activity);
        if (estado_antes != fsm_obtener_estado()) {
            if (estado_antes == SYSTEM_APAGADO) energia_despertar_perifericos();
            actualizar_pantalla();
            energia_aplicar_estado(fsm_obtener_estado());
        }


		// --------------------------------
		// 5. ACTUALIZACIÓN DEL LED
		// --------------------------------

		if (led_override < 0) es_actualizar_led(
		current_time,
		fsm_obtener_estado()
		);
		if (fsm_obtener_estado() == SYSTEM_APAGADO) es_apagar_led();


		// --------------------------------
		// 6. CONTROL DEL BUZZER
		// --------------------------------

		actualizar_buzzer(current_time);


		// --------------------------------
		// 7. SOLICITUD DE LECTURA DE TEMPERATURA
		// --------------------------------

		if (fsm_obtener_estado() == SYSTEM_ACTIVO &&
		(uint32_t)(current_time - temperature_last_time_ms)
		>= TEMPERATURE_INTERVAL_MS)
		{
			temperature_last_time_ms = current_time;

			// Solo inicia la lectura; temp_actualizar() la completa
			temp_iniciar_lectura();
		}


		// --------------------------------
		// 8. ACTUALIZACIÓN DE LA LCD
		// --------------------------------

		if (fsm_obtener_estado() != SYSTEM_APAGADO &&
		(uint32_t)(current_time - lcd_last_time_ms)
		>= LCD_INTERVAL_MS)
		{
			lcd_last_time_ms = current_time;

			actualizar_pantalla();
		}
        energia_dormir(fsm_obtener_estado());
	}
}
