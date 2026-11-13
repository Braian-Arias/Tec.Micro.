#include <stdint.h>

#include "maquina_estados.h"

// Estado actual
static SystemState estado_actual = SYSTEM_ACTIVO;

// Momento de la última actividad válida
static uint32_t ultima_actividad_ms = 0;

void fsm_inicializar(uint32_t tiempo_actual)
{
	estado_actual = SYSTEM_ACTIVO;
	ultima_actividad_ms = tiempo_actual;
}

void fsm_actualizar(uint32_t tiempo_actual, uint8_t actividad_usuario)
{
	// Cualquier pulso en el boton vuelve al estado anterior 
	if (actividad_usuario)
	{
		ultima_actividad_ms = tiempo_actual;
		if (estado_actual == SYSTEM_APAGADO)
          { estado_actual = SYSTEM_STANDBY;}
         else if (estado_actual == SYSTEM_STANDBY)
           { estado_actual = SYSTEM_ACTIVO;}
		return;
	}

	// Tiempo desde la última actividad válida
	uint32_t inactividad_ms = (uint32_t)(tiempo_actual - ultima_actividad_ms);

	// ACTIVO -> STANDBY: 10 s sin actividad
	if (estado_actual == SYSTEM_ACTIVO)
	{
		if (inactividad_ms >= 10000UL)
		{
			estado_actual = SYSTEM_STANDBY;
		}
	}
	// STANDBY -> APAGADO: 30 s sin actividad desde el último evento válido
	else if (estado_actual == SYSTEM_STANDBY)
	{
		if (inactividad_ms >= 30000UL)
		{
			estado_actual = SYSTEM_APAGADO;
		}
	}
	// APAGADO permanece hasta recibir actividad
}

SystemState fsm_obtener_estado(void)
{
	return estado_actual;
}
