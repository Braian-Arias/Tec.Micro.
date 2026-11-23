#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t cmd;
    uint8_t d1;
    uint8_t d2;
} trama_cmd_t;

// Inicializa el puerto serie USART0 a 9600 baudios con interrupciones RX/TX y Ring Buffers
void bluetooth_init(void);

// Procesa el Ring Buffer serie y decodifica la trama oficial UTEC [0xAA, CMD, D1, D2, CHK]
bool bluetooth_obtener_trama(trama_cmd_t *trama_out, uint32_t ms_actual);

// Transmite la trama de telemetría [0xBB, Vbat, Estado, Flags, CHK] sin bloquear
void bluetooth_enviar_telemetria(uint8_t vbat_dv, uint8_t estado, uint8_t flags);

#endif /* BLUETOOTH_H_ */
