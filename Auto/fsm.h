#ifndef FSM_H_
#define FSM_H_

#include <stdint.h>
#include <stdbool.h>
#include "bluetooth.h"

typedef enum {
    ESTADO_INICIO = 0,
    ESTADO_ESPERA_ENLACE = 1,
    ESTADO_MANUAL = 2,
    ESTADO_FAILSAFE = 3,
    ESTADO_STANDBY = 4,
    ESTADO_BATERIA_BAJA = 5
} estado_robot_t;

// Inicializa la máquina de estados
void fsm_init(void);

// Actualiza el estado principal del robot y aplica comandos de movimiento y pinza
void fsm_update(const trama_cmd_t *trama, bool nueva_trama, uint8_t vbat_dv, uint32_t ms_actual);

// Obtiene el estado numérico actual
estado_robot_t fsm_obtener_estado(void);

#endif /* FSM_H_ */
