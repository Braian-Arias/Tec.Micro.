
#ifndef CONFIG_H
#define CONFIG_H

#define F_CPU 16000000UL

// Elegir el controlador:
// 1 = WS2812B
// 2 = MAX7219
#define TIPO_MATRIZ 1

#if (TIPO_MATRIZ != 1) && (TIPO_MATRIZ != 2)
#error "Tipo de matriz no valido"
#endif

#endif
