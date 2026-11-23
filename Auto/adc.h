#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>
#include <stdbool.h>

// Inicializa ADC0 (PC0) con Vref = AVCC, prescaler 128 e interrupción por conversión completada
void adc_init(void);

// Inicia una conversión analógica en segundo plano (no bloqueante)
void adc_iniciar_conversion(void);

// Obtiene la lectura bruta del ADC (10 bits) de forma atómica. Devuelve true si hay un dato listo.
bool adc_obtener_resultado(uint16_t *resultado);

// Convierte la lectura de 10 bits a decivoltios de batería (ej. 84 = 8.4 V)
uint8_t adc_convertir_bateria_dv(uint16_t raw);

#endif /* ADC_H_ */
