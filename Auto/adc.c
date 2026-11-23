#include "adc.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

// Factor de escala en punto fijo para divisor de tensión R1=10k, R2=4.7k, Vref=5.0V
#define K_DIVISOR_DV 1527UL

static volatile uint16_t adc_valor_raw = 0;
static volatile bool adc_listo = false;

ISR(ADC_vect) {
    adc_valor_raw = ADC;
    adc_listo = true;
}

void adc_init(void) {
    // Configurar PC0 (ADC0) como entrada analógica sin pull-up
    DDRC &= ~(1 << DDC0);
    PORTC &= ~(1 << PORTC0);

    // REFS0 = 1 (Vref = AVcc), Canal ADC0 (MUX3..0 = 0000)
    ADMUX = (1 << REFS0);

    // ADEN = 1 (Habilitar ADC), ADIE = 1 (Habilitar interrupción), Prescaler = 128 (125 kHz a 16 MHz)
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADCSRB = 0;

    adc_valor_raw = 0;
    adc_listo = false;
}

void adc_iniciar_conversion(void) {
    if (!(ADCSRA & (1 << ADSC))) {
        ADCSRA |= (1 << ADSC);
    }
}

bool adc_obtener_resultado(uint16_t *resultado) {
    bool listo = false;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if (adc_listo) {
            *resultado = adc_valor_raw;
            adc_listo = false;
            listo = true;
        }
    }
    return listo;
}

uint8_t adc_convertir_bateria_dv(uint16_t raw) {
    // Cálculo en 32 bits: (raw * 1527) / 10000 -> Decivoltios
    return (uint8_t)(((uint32_t)raw * K_DIVISOR_DV) / 10000);
}
