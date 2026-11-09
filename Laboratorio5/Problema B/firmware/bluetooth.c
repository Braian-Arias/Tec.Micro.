#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "bluetooth.h"

#define START 0x7E
#define STOP  0x7F
#define RX_SIZE 32
#define TX_SIZE 32

static volatile uint8_t rx[RX_SIZE], tx[TX_SIZE];
static volatile uint8_t rx_in, rx_out, tx_in, tx_out;
static volatile uint8_t rx_overflow;
static volatile uint8_t tx_activo;

ISR(USART_RX_vect)
{
    uint8_t b = UDR0;
    uint8_t next = (uint8_t)((rx_in + 1) % RX_SIZE);
    if (next == rx_out) rx_overflow = 1;
    else { rx[rx_in] = b; rx_in = next; }
}

ISR(USART_UDRE_vect)
{
    if (tx_out == tx_in) UCSR0B &= ~(1 << UDRIE0);
    else { UDR0 = tx[tx_out]; tx_out = (uint8_t)((tx_out + 1) % TX_SIZE); }
}

void bt_inicializar(void)
{
    // 16 MHz, 9600 bps, modo normal: UBRR0 = 103; error aprox. 0,16 %.
    UBRR0H = 0;
    UBRR0L = 103;
    UCSR0A = 0;
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
}

void bt_habilitar(uint8_t habilitar)
{
    if (habilitar) {
        PRR &= ~(1 << PRUSART0);
        bt_inicializar();
    } else {
        UCSR0B = 0;
        PRR |= (1 << PRUSART0);
    }
}

void bt_esperar_envio(void)
{
    if (!tx_activo) return;
    while (tx_out != tx_in || !(UCSR0A & (1 << TXC0))) { }
    tx_activo = 0;
}

static uint8_t recibir(uint8_t *b)
{
    uint8_t ok = 0;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if (rx_out != rx_in) {
            *b = rx[rx_out];
            rx_out = (uint8_t)((rx_out + 1) % RX_SIZE);
            ok = 1;
        }
    }
    return ok;
}

static uint8_t enviar_byte(uint8_t b)
{
    uint8_t ok = 0;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        uint8_t next = (uint8_t)((tx_in + 1) % TX_SIZE);
        if (next != tx_out) {
            tx[tx_in] = b;
            tx_in = next;
            UCSR0B |= (1 << UDRIE0);
            ok = 1;
        }
    }
    return ok;
}

void bt_enviar(uint8_t comando, uint8_t dato)
{
    // La trama completa cabe en el buffer de TX si hay espacio suficiente.
    uint8_t libres;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        libres = (uint8_t)((tx_out + TX_SIZE - tx_in - 1) % TX_SIZE);
    }
    if (libres < 5 || (PRR & (1 << PRUSART0))) return;
    UCSR0A |= (1 << TXC0); // limpiar bandera de transmisión completa
    tx_activo = 1;
    enviar_byte(START);
    enviar_byte(comando);
    enviar_byte(dato);
    enviar_byte((uint8_t)(comando ^ dato));
    enviar_byte(STOP);
}

static uint8_t comando_pendiente, dato_pendiente, hay_comando;
static uint8_t fase, cmd, data, checksum;
static uint32_t ultimo_byte_ms;

void bt_procesar(uint32_t ahora_ms)
{
    uint8_t b;
    if (fase && (uint32_t)(ahora_ms - ultimo_byte_ms) > 100UL) {
        fase = 0;
        bt_enviar(BT_ERROR, 6);
    }
    if (rx_overflow) {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { rx_overflow = 0; rx_out = rx_in; }
        fase = 0;
        bt_enviar(BT_ERROR, 3);
    }
    while (!hay_comando && recibir(&b)) {
        ultimo_byte_ms = ahora_ms;
        if (fase == 0) { if (b == START) fase = 1; continue; }
        if (fase == 1) { cmd = b; fase = 2; }
        else if (fase == 2) { data = b; fase = 3; }
        else if (fase == 3) { checksum = b; fase = 4; }
        else if (fase == 4) {
            fase = 0;
            if (b != STOP) bt_enviar(BT_ERROR, 1);
            else if (checksum != (uint8_t)(cmd ^ data)) bt_enviar(BT_ERROR, 2);
            else { comando_pendiente = cmd; dato_pendiente = data; hay_comando = 1; }
        }
    }
}

uint8_t bt_obtener_comando(uint8_t *comando, uint8_t *dato)
{
    if (!hay_comando) return 0;
    *comando = comando_pendiente;
    *dato = dato_pendiente;
    hay_comando = 0;
    return 1;
}
