#include "pinza.h"
#include <avr/io.h>

#define PINZA_ANGULO_ABIERTA 0
#define PINZA_ANGULO_CERRADA 180
#define PASO_ANGULO 2 // Avance progresivo de 2° cada 20 ms (PDF 19)

static uint8_t pos_actual = 0;
static uint8_t pos_objetivo = 0;

static inline void servo_set_us(uint16_t us) {
    if (us < 500) us = 500;
    if (us > 2500) us = 2500;
    // ICR1 = 39999 (50 Hz con N=8). 0.5 us por cuenta: OCR1A = 2 * us - 1
    OCR1A = 2 * us - 1;
}

void pinza_init(void) {
    // Pin PB1 (D9 / OC1A) como salida
    DDRB |= (1 << DDB1);

    // Timer1: Fast PWM modo 14 (TOP = ICR1), Prescaler 8 -> ICR1 = 39999 para 50 Hz
    TCCR1A = (1 << COM1A1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);
    ICR1 = 39999;

    pos_actual = PINZA_ANGULO_ABIERTA;
    pos_objetivo = PINZA_ANGULO_ABIERTA;
    servo_set_us(1000); // 1 ms inicial (0°)
}

void pinza_pedir_angulo(uint8_t angulo) {
    if (angulo > 180) angulo = 180;
    pos_objetivo = angulo;
}

void pinza_tarea_20ms(void) {
    if (pos_actual + PASO_ANGULO <= pos_objetivo) {
        pos_actual += PASO_ANGULO;
    } else if (pos_actual >= pos_objetivo + PASO_ANGULO) {
        pos_actual -= PASO_ANGULO;
    } else {
        pos_actual = pos_objetivo;
    }

    uint16_t us = 1000 + ((uint32_t)pos_actual * 1000) / 180;
    servo_set_us(us);
}

void pinza_abrir(void) {
    pinza_pedir_angulo(PINZA_ANGULO_ABIERTA);
}

void pinza_cerrar(void) {
    pinza_pedir_angulo(PINZA_ANGULO_CERRADA);
}
