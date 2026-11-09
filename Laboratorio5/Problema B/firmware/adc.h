#ifndef ADC_H
#define ADC_H

#include <stdint.h>

void adc_inicializar(void);
void adc_iniciar_conversion(void);
uint8_t adc_obtener_resultado(uint16_t *resultado);

#endif