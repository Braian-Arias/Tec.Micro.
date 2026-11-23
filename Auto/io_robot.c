#include "io_robot.h"
#include <avr/io.h>

static uint32_t t_ultimo_led = 0;

void io_robot_init(void) {
    // Configurar PC1 (A1) como entrada con resistencia pull-up interna (Final de Carrera)
    DDRC &= ~(1 << DDC1);
    PORTC |= (1 << PORTC1);

    // Configurar PB5 (D13 / LED de estado) como salida
    DDRB |= (1 << DDB5);
    PORTB &= ~(1 << PORTB5);
}

bool io_pinza_detectada(void) {
    // Microinterruptor normalmente abierto conectado a GND con pull-up interna
    // Presionado = Nivel LÓGICO BAJO (0)
    static uint8_t lecturas = 0xFF;
    lecturas = (lecturas << 1) | ((PINC & (1 << PINC1)) ? 1 : 0);
    
    // Filtrado por muestreo: si las últimas lecturas son 0, está presionado
    return (lecturas == 0x00);
}

void io_led_actualizar(uint8_t estado_robot, uint32_t ms_actual) {
    uint16_t intervalo_ms = 500; // Por defecto parpadeo normal (500 ms)

    switch (estado_robot) {
        case 1: // ESPERA_ENLACE / STANDBY: Parpadeo lento (1000 ms)
        case 4:
            intervalo_ms = 1000;
            break;
        case 2: // MANUAL: LED Encendido fijo
            PORTB |= (1 << PORTB5);
            return;
        case 3: // FAILSAFE: Parpadeo muy rápido (100 ms)
            intervalo_ms = 100;
            break;
        case 5: // BATERIA_BAJA: Parpadeo medio (200 ms)
            intervalo_ms = 200;
            break;
        default:
            intervalo_ms = 500;
            break;
    }

    if (ms_actual - t_ultimo_led >= intervalo_ms) {
        t_ultimo_led = ms_actual;
        PORTB ^= (1 << PORTB5); // Conmutar estado del LED
    }
}
