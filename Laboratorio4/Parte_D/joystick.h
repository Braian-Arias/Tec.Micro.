
#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <stdint.h>

// Inicializar ADC y entrada del pulsador
void joystick_inicializar(void);

// Leer los ejes del joystick
uint16_t joystick_leer_x(void);
uint16_t joystick_leer_y(void);

// Consultar el pulsador: 1 = presionado, 0 = suelto
uint8_t joystick_boton_presionado(void);

// Actualizar la posición del cursor
void joystick_actualizar_cursor(uint8_t *fila, uint8_t *columna);

#endif
