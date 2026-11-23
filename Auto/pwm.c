#include "pwm.h"
#include <avr/io.h>

#define UMBRAL_ZONA_MUERTA 5

void pwm_init(void) {
    // Configurar pines PD5 (OC0B / D5) y PD6 (OC0A / D6) como salidas
    DDRD |= (1 << DDD5) | (1 << DDD6);

    // Timer0: Fast PWM modo 3, salidas no invertidas en OC0A y OC0B, Prescaler 64 (~976 Hz)
    TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
    TCCR0B = (1 << CS01) | (1 << CS00);

    OCR0A = 0;
    OCR0B = 0;
}

void motores_manejar(int16_t v, int16_t w) {
    // 1. Filtrado de zona muerta en el centro del mando táctil
    if (v > -UMBRAL_ZONA_MUERTA && v < UMBRAL_ZONA_MUERTA) v = 0;
    if (w > -UMBRAL_ZONA_MUERTA && w < UMBRAL_ZONA_MUERTA) w = 0;

    // 2. Mezcla diferencial
    int16_t izq = v + w;
    int16_t der = v - w;

    // 3. Saturación al rango [-100, +100]
    if (izq > 100) izq = 100;   if (izq < -100) izq = -100;
    if (der > 100) der = 100;   if (der < -100) der = -100;

    // 4. Mapeo a duty cycle de 8 bits (0 a 255)
    uint16_t pwm_izq = (izq < 0) ? -izq : izq;
    uint16_t pwm_der = (der < 0) ? -der : der;

    OCR0A = (uint8_t)((pwm_izq * 255UL) / 100);
    OCR0B = (uint8_t)((pwm_der * 255UL) / 100);
}

void motores_detener(void) {
    OCR0A = 0;
    OCR0B = 0;
}
