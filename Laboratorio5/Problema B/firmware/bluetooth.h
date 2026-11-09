#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <stdint.h>

// Trama: 0x7E, comando, dato, comando XOR dato, 0x7F.
// Los comandos recibidos se procesan en el bucle principal.
void bt_inicializar(void);
void bt_procesar(uint32_t ahora_ms);
uint8_t bt_obtener_comando(uint8_t *comando, uint8_t *dato);
void bt_enviar(uint8_t comando, uint8_t dato);
void bt_habilitar(uint8_t habilitar);
void bt_esperar_envio(void);

#define BT_ESTADO       0x01
#define BT_SENSORES     0x02
#define BT_CAMBIAR      0x03
#define BT_ACTUADOR     0x04
#define BT_EMERGENCIA  0x05
#define BT_ALARMAS     0x06
#define BT_ERROR       0x7D
#define BT_RESPUESTA   0x80

#endif
