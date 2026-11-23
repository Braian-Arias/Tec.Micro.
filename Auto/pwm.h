#ifndef PWM_H_
#define PWM_H_

#include <stdint.h>

// Inicializa Timer0 (8 bits) en Fast PWM para motores de tracción (PD6 / OC0A, PD5 / OC0B)
void pwm_init(void);

// Procesa velocidad (v) y giro (w) en rango [-100, +100] con zona muerta y mezcla diferencial
void motores_manejar(int16_t v, int16_t w);

// Detiene por completo los motores de tracción
void motores_detener(void);

#endif /* PWM_H_ */
