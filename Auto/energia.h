#ifndef ENERGIA_H_
#define ENERGIA_H_

#include <stdint.h>

// Inicializa la gestión de energía, apaga periféricos no usados (SPI, TWI) y configura WDT (8s)
void energia_init(void);

// Reinicia el Watchdog Timer para prevenir el reset del sistema
void energia_wdt_reset(void);

// Coloca el microcontrolador en modo Sleep IDLE hasta la próxima interrupción
void energia_dormir_idle(void);

#endif /* ENERGIA_H_ */
