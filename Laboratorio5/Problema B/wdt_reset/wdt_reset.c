/* Prueba independiente: WDT como mecanismo de reset, ATmega328P / 16 MHz.
 * D2/PD2: pulsador a GND; D13/PB5: actividad/bloqueo;
 * D4/PD4: causa WDRF y estado seguro; D5/PD5: arranque normal.
 */
#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdint.h>

int main(void)
{
    /* El flag WDRF debe leerse antes de limpiarlo y antes de desarmar el WDT. */
    uint8_t causa_reset = MCUSR;
    MCUSR = 0;
    wdt_disable();

    DDRB |= (1 << DDB5);
    DDRD |= (1 << DDD4) | (1 << DDD5);
    PORTB &= ~(1 << PORTB5);
    PORTD &= ~((1 << PORTD4) | (1 << PORTD5));
    DDRD &= ~(1 << DDD2);
    PORTD |= (1 << PORTD2); /* pull-up interno: pulsado = 0 */

    if (causa_reset & (1 << WDRF)) {
        /* Recuperación verificable: actuador apagado y alarma de reset encendida.
         * WDT desarmado para que la indicación no provoque resets repetidos. */
        PORTB &= ~(1 << PORTB5);
        PORTD |= (1 << PORTD4);
        for (;;) { }
    }

    PORTD |= (1 << PORTD5); /* arranque normal */
    wdt_enable(WDTO_2S);   /* WDE=1, WDIE=0: modo reset, nominal 2 s */
    for (;;) {
        wdt_reset();
        if (!(PIND & (1 << PIND2))) {
            PORTB |= (1 << PORTB5); /* indicador de bloqueo */
            for (;;) { }            /* bloqueo intencional: no ejecuta WDR */
        }
        PORTB ^= (1 << PORTB5);
        _delay_ms(200);
    }
}
