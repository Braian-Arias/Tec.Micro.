#ifndef PINZA_H_
#define PINZA_H_

#include <stdint.h>

// Inicializa Timer1 en Fast PWM modo 14 (50 Hz, ICR1=39999) en PB1 (D9)
void pinza_init(void);

// Solicita un nuevo ángulo objetivo (0° a 180°)
void pinza_pedir_angulo(uint8_t angulo);

// Tarea periódica no bloqueante llamada cada 20 ms para mover el servo suavemente (2°/20ms)
void pinza_tarea_20ms(void);

// Abrir y cerrar pinza
void pinza_abrir(void);
void pinza_cerrar(void);

#endif /* PINZA_H_ */
