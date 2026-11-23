#include "temporizadores.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

static volatile uint32_t ms_contador = 0;

ISR(TIMER2_COMPA_vect) {
    ms_contador++;
}

void temporizadores_init(void) {
    // Configurar Timer2 en modo CTC (Clear Timer on Compare Match)
    // Prescaler 64, TOP = OCR2A = 249
    // F_CPU = 16 MHz -> f = 16MHz / (64 * 250) = 1000 Hz (1 ms)
    TCCR2A = (1 << WGM21);
    TCCR2B = (1 << CS22); // Prescaler 64
    OCR2A = 249;
    TIMSK2 = (1 << OCIE2A); // Habilitar interrupción por comparación
}

uint32_t millis(void) {
    uint32_t ms;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        ms = ms_contador;
    }
    return ms;
}

void delay_ms_tick(uint16_t ms) {
    uint32_t inicio = millis();
    while ((millis() - inicio) < ms) {
        // Espera activa limpia
    }
}
