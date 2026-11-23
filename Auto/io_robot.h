#ifndef IO_ROBOT_H_
#define IO_ROBOT_H_

#include <stdint.h>
#include <stdbool.h>

// Inicializa pines de Entradas (PC1 / A1: Final de Carrera) y Salidas (PB5 / D13: LED)
void io_robot_init(void);

// Lee el estado filtrado (antirrebote por muestreo) del final de carrera de la pinza
// Retorna true si el objeto presiona la pinza (pin a GND)
bool io_pinza_detectada(void);

// Actualiza el parpadeo del LED indicador según el estado actual del robot
void io_led_actualizar(uint8_t estado_robot, uint32_t ms_actual);

#endif /* IO_ROBOT_H_ */
