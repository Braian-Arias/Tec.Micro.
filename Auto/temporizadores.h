#ifndef TEMPORIZADORES_H_
#define TEMPORIZADORES_H_

#include <stdint.h>

// Inicializa Timer2 en modo CTC para una base de tiempo de 1 ms
void temporizadores_init(void);

// Devuelve los milisegundos transcurridos desde el inicio (lectura atómica)
uint32_t millis(void);

// Retardo no bloqueante voluntario
void delay_ms_tick(uint16_t ms);

#endif /* TEMPORIZADORES_H_ */
