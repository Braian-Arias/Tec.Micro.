#ifndef TEMPORIZADORES_H
#define TEMPORIZADORES_H

#include <stdint.h>

void tmr_inicializar(void);
uint32_t tmr_obtener_tiempo_ms(void);
uint8_t tmr_obtener_solicitud_adc(void);
uint8_t tmr_obtener_evento_boton(void);

#endif