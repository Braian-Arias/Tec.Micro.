#include "fsm.h"
#include "pwm.h"
#include "pinza.h"

#define TIMEOUT_FAILSAFE_MS 300UL
#define TIMEOUT_STANDBY_MS 10000UL
#define UMBRAL_BAT_BAJA_DV 66 // 6.6 V

static estado_robot_t estado_actual = ESTADO_INICIO;
static uint32_t t_ultima_trama = 0;
static int16_t v_cmd = 0, w_cmd = 0;

void fsm_init(void) {
    estado_actual = ESTADO_INICIO;
    t_ultima_trama = 0;
    v_cmd = 0;
    w_cmd = 0;
}

void fsm_update(const trama_cmd_t *trama, bool nueva_trama, uint8_t vbat_dv, uint32_t ms_actual) {
    if (nueva_trama) {
        t_ultima_trama = ms_actual;
        if (trama->cmd == 0x01) { // Comando MOVER
            v_cmd = (int16_t)trama->d1 - 100; // Eliminar offset +100
            w_cmd = (int16_t)trama->d2 - 100;
        } else if (trama->cmd == 0x02) { // Comando PINZA
            pinza_pedir_angulo(trama->d1);
        }
    }

    bool desconexion = (ms_actual - t_ultima_trama > TIMEOUT_FAILSAFE_MS);
    bool inactividad  = (ms_actual - t_ultima_trama > TIMEOUT_STANDBY_MS);

    switch (estado_actual) {
        case ESTADO_INICIO:
            motores_detener();
            estado_actual = ESTADO_ESPERA_ENLACE;
            break;

        case ESTADO_ESPERA_ENLACE:
            motores_detener();
            if (nueva_trama) estado_actual = ESTADO_MANUAL;
            break;

        case ESTADO_MANUAL:
            if (desconexion) {
                estado_actual = ESTADO_FAILSAFE;
            } else if (vbat_dv < UMBRAL_BAT_BAJA_DV) {
                estado_actual = ESTADO_BATERIA_BAJA;
            } else if (inactividad) {
                estado_actual = ESTADO_STANDBY;
            } else {
                motores_manejar(v_cmd, w_cmd);
            }
            break;

        case ESTADO_FAILSAFE:
            motores_detener(); // Apagar tracción; la pinza sostiene su posición
            if (!desconexion) estado_actual = ESTADO_MANUAL;
            break;

        case ESTADO_STANDBY:
            motores_detener();
            if (nueva_trama) estado_actual = ESTADO_MANUAL;
            break;

        case ESTADO_BATERIA_BAJA:
            if (desconexion) {
                estado_actual = ESTADO_FAILSAFE;
            } else {
                // Modo seguro: limitar velocidad al 50%
                motores_manejar(v_cmd / 2, w_cmd / 2);
            }
            break;
    }
}

estado_robot_t fsm_obtener_estado(void) {
    return estado_actual;
}
