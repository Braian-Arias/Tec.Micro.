#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/atomic.h>
#include "energia.h"
#include "LCD_I2C.h"
#include "bluetooth.h"

static SystemState estado_anterior = SYSTEM_ACTIVO;
static volatile uint8_t evento_wdt;
ISR(WDT_vect) { evento_wdt = 1; }

void energia_wdt_interrupcion_8s(void)
{
    uint8_t sreg = SREG;
    cli();
    wdt_reset();
    MCUSR &= ~(1 << WDRF);
    WDTCSR = (1 << WDCE) | (1 << WDE);
    WDTCSR = (1 << WDIE) | (1 << WDP3) | (1 << WDP0);
    SREG = sreg;
}

uint8_t energia_obtener_evento_wdt(void)
{
    uint8_t evento;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { evento = evento_wdt; evento_wdt = 0; }
    return evento;
}

void energia_despertar_perifericos(void)
{
    PRR &= ~((1 << PRADC) | (1 << PRUSART0) | (1 << PRTIM0) |
             (1 << PRTIM1) | (1 << PRTWI));
    ADCSRA |= (1 << ADEN);
    bt_habilitar(1);
    lcd_retroiluminacion(1);
}

void energia_aplicar_estado(SystemState estado)
{
    if (estado == estado_anterior) return;
    if (estado == SYSTEM_ACTIVO) {
        wdt_disable();
        energia_despertar_perifericos();
    }
    else if (estado == SYSTEM_STANDBY) {
        // Timer0, Timer1, UART y TWI siguen operativos en Idle.
        PRR &= ~((1 << PRTIM0) | (1 << PRTIM1) | (1 << PRUSART0) | (1 << PRTWI));
    } else if (estado == SYSTEM_APAGADO) {
        bt_esperar_envio();
        lcd_retroiluminacion(0);
        ADCSRA &= ~(1 << ADEN);
        bt_habilitar(0);
        PRR |= (1 << PRADC) | (1 << PRTIM0) | (1 << PRTIM1) |
               (1 << PRSPI) | (1 << PRTIM2) | (1 << PRTWI);
        energia_wdt_interrupcion_8s();
    }
    estado_anterior = estado;
}

void energia_dormir(SystemState estado)
{
    if (estado == SYSTEM_ACTIVO) return;
    set_sleep_mode(estado == SYSTEM_STANDBY ? SLEEP_MODE_IDLE : SLEEP_MODE_PWR_DOWN);
    cli();
    sleep_enable();
    sei();
    sleep_cpu();
    sleep_disable();
}
