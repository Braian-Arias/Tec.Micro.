#ifndef UART_H_
#define UART_H_

#include <stdint.h>

// Inicializar UART a 9600 baudios
void uart_inicializar(void);

// Enviar un caracter
void uart_enviar_caracter(char dato);

// Enviar texto
void uart_enviar_texto(const char *texto);

// Enviar numero entero
void uart_enviar_numero(uint32_t numero);

// Esperar hasta terminar la transmision
void uart_esperar_fin(void);

// Enviar datos a MATLAB
// Formato: tiempo_ms,estado,corriente_mA
void uart_enviar_matlab(
uint32_t tiempo_ms,
uint8_t estado,
uint16_t corriente_centesimas);

#endif