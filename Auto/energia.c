#include "energia.h"
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

void energia_init(void) {
    // 1. Reducción de potencia: deshabilitar SPI y TWI (I2C) en PRR
    PRR |= (1 << PRSPI) | (1 << PRTWI);

    // 2. Configurar Watchdog Timer a 8 segundos
    MCUSR &= ~(1 << WDRF);
    WDTCSR |= (1 << WDCE) | (1 << WDE);
    WDTCSR = (1 << WDE) | (1 << WDP3) | (1 << WDP0); // ~8 segundos
}

void energia_wdt_reset(void) {
    wdt_reset();
}

void energia_dormir_idle(void) {
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();
    sleep_cpu();
    sleep_disable();
}
