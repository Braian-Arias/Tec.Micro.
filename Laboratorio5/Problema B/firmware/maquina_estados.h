#ifndef MAQUINA_ESTADOS_H
#define MAQUINA_ESTADOS_H

#include <stdint.h>

// Estados principales del sistema
typedef enum
{
	SYSTEM_ACTIVO,
	SYSTEM_STANDBY,
	SYSTEM_APAGADO
} SystemState;

void fsm_inicializar(uint32_t tiempo_actual);
void fsm_actualizar(uint32_t tiempo_actual, uint8_t actividad_usuario);
SystemState fsm_obtener_estado(void);
void fsm_fijar_estado(SystemState estado, uint32_t tiempo_actual);

#endif
