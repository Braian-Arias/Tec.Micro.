#ifndef ENERGIA_H
#define ENERGIA_H
#include <stdint.h>
#include "maquina_estados.h"
void energia_aplicar_estado(SystemState estado);
void energia_dormir(SystemState estado);
void energia_despertar_perifericos(void);
void energia_wdt_interrupcion_8s(void);
uint8_t energia_obtener_evento_wdt(void);
#endif
