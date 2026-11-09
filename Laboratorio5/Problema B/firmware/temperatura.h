#ifndef TEMPERATURA_H
#define TEMPERATURA_H

#include <stdint.h>

void temp_inicializar(void);
uint8_t temp_iniciar_lectura(void);
void temp_actualizar(void);
int16_t temp_obtener(void);

#endif