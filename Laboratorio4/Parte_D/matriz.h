
#ifndef MATRIZ_H
#define MATRIZ_H

#include <stdint.h>

// Colores lógicos del juego
#define COLOR_APAGADO  0
#define COLOR_ROJO     1
#define COLOR_VERDE    2
#define COLOR_AZUL     3
#define COLOR_AMARILLO 4
#define COLOR_CELESTE  5
#define COLOR_MAGENTA  6
#define COLOR_BLANCO   7

// Inicializar el controlador seleccionado
void matriz_inicializar(void);

// Apagar todos los LEDs
void matriz_limpiar(void);

// Encender un píxel en la fila y columna indicadas
void matriz_pixel(uint8_t fila, uint8_t columna, uint8_t color);

// Actualizar la matriz física
void matriz_actualizar(void);

// Animaciones
void matriz_animacion_exito(void);
void matriz_animacion_error(void);

#endif
