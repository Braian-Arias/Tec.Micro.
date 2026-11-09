#ifndef ENTRADAS_SALIDAS_H
#define ENTRADAS_SALIDAS_H

#include <stdint.h>
#include "maquina_estados.h"

void es_inicializar(void);
void es_actualizar_led(uint32_t tiempo_actual, SystemState estado);
void es_encender_led(void);
void es_apagar_led(void);

#endif