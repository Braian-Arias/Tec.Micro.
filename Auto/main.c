#include <avr/io.h>
#include <avr/interrupt.h>

#include "temporizadores.h"
#include "energia.h"
#include "io_robot.h"
#include "adc.h"
#include "pwm.h"
#include "pinza.h"
#include "bluetooth.h"
#include "fsm.h"

int main(void) {
    // 1. Inicialización de periféricos y hardware
    temporizadores_init();
    energia_init();
    io_robot_init();
    adc_init();
    pwm_init();
    pinza_init();
    bluetooth_init();
    fsm_init();

    // 2. Habilitar interrupciones globales
    sei();

    trama_cmd_t trama_actual;
    uint16_t adc_raw = 0;
    uint8_t vbat_dv = 84; // ~8.4 V inicial

    uint32_t t_ultimo_servo = 0;
    uint32_t t_ultimo_adc   = 0;
    uint32_t t_ultima_telemetria = 0;

    while (1) {
        // Alimentar Watchdog Timer
        energia_wdt_reset();

        uint32_t ms_actual = millis();

        // 3. Recepción y decodificación de tramas Bluetooth
        bool nueva_trama = bluetooth_obtener_trama(&trama_actual, ms_actual);

        // 4. Lectura del sensor final de carrera
        bool objeto_sujeto = io_pinza_detectada();

        // 5. Tarea periódica ADC (cada 100 ms)
        if (ms_actual - t_ultimo_adc >= 100) {
            t_ultimo_adc = ms_actual;
            adc_iniciar_conversion();
        }
        if (adc_obtener_resultado(&adc_raw)) {
            vbat_dv = adc_convertir_bateria_dv(adc_raw);
        }

        // 6. Tarea periódica de la pinza (cada 20 ms)
        if (ms_actual - t_ultimo_servo >= 20) {
            t_ultimo_servo = ms_actual;
            pinza_tarea_20ms();
        }

        // 7. Actualización de la Máquina de Estados (FSM)
        fsm_update(&trama_actual, nueva_trama, vbat_dv, ms_actual);

        // 8. Actualización del LED indicador de estado
        estado_robot_t st = fsm_obtener_estado();
        io_led_actualizar((uint8_t)st, ms_actual);

        // 9. Telemetría a la App móvil (cada 500 ms)
        if (ms_actual - t_ultima_telemetria >= 500) {
            t_ultima_telemetria = ms_actual;
            uint8_t flags = objeto_sujeto ? 0x01 : 0x00;
            bluetooth_enviar_telemetria(vbat_dv, (uint8_t)st, flags);
        }

        // 10. Ahorro de energía en espera IDLE
        energia_dormir_idle();
    }

    return 0;
}
