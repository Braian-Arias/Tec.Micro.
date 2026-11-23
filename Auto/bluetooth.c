#include "bluetooth.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#define START_BYTE 0xAA
#define TELEMETRIA_BYTE 0xBB
#define RX_BUF_SIZE 32
#define TX_BUF_SIZE 32
#define TIMEOUT_BYTE_MS 100

static volatile uint8_t rx_buf[RX_BUF_SIZE];
static volatile uint8_t rx_head = 0, rx_tail = 0;

static volatile uint8_t tx_buf[TX_BUF_SIZE];
static volatile uint8_t tx_head = 0, tx_tail = 0;

ISR(USART_RX_vect) {
    uint8_t b = UDR0;
    uint8_t next = (rx_head + 1) % RX_BUF_SIZE;
    if (next != rx_tail) {
        rx_buf[rx_head] = b;
        rx_head = next;
    }
}

ISR(USART_UDRE_vect) {
    if (tx_tail == tx_head) {
        UCSR0B &= ~(1 << UDRIE0); // Deshabilitar interrupción cuando el buffer esté vacío
    } else {
        UDR0 = tx_buf[tx_tail];
        tx_tail = (tx_tail + 1) % TX_BUF_SIZE;
    }
}

void bluetooth_init(void) {
    // Baudrate 9600 bps para F_CPU = 16 MHz -> UBRR0 = 103
    UBRR0H = 0;
    UBRR0L = 103;

    UCSR0A = 0;
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 bits de datos, 1 bit de parada
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0); // Habilitar RX, TX e interrupción RX
}

static bool rx_read_byte(uint8_t *b) {
    bool ok = false;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if (rx_tail != rx_head) {
            *b = rx_buf[rx_tail];
            rx_tail = (rx_tail + 1) % RX_BUF_SIZE;
            ok = true;
        }
    }
    return ok;
}

bool bluetooth_obtener_trama(trama_cmd_t *trama_out, uint32_t ms_actual) {
    static uint8_t estado_parser = 0;
    static uint8_t cmd = 0, d1 = 0, d2 = 0;
    static uint32_t t_ultimo_byte = 0;
    uint8_t b;

    // Timeout de descarte por trama trunca (100 ms)
    if (estado_parser != 0 && (ms_actual - t_ultimo_byte > TIMEOUT_BYTE_MS)) {
        estado_parser = 0;
    }

    while (rx_read_byte(&b)) {
        t_ultimo_byte = ms_actual;

        switch (estado_parser) {
            case 0: // Esperar byte de inicio 0xAA
                if (b == START_BYTE) estado_parser = 1;
                break;
            case 1: // CMD
                cmd = b;
                estado_parser = 2;
                break;
            case 2: // D1
                d1 = b;
                estado_parser = 3;
                break;
            case 3: // D2
                d2 = b;
                estado_parser = 4;
                break;
            case 4: // Checksum XOR
                estado_parser = 0;
                if (b == (cmd ^ d1 ^ d2)) {
                    trama_out->cmd = cmd;
                    trama_out->d1  = d1;
                    trama_out->d2  = d2;
                    return true;
                }
                break;
        }
    }
    return false;
}

void bluetooth_enviar_telemetria(uint8_t vbat_dv, uint8_t estado, uint8_t flags) {
    uint8_t chk = TELEMETRIA_BYTE ^ vbat_dv ^ estado ^ flags;
    uint8_t paquete[5] = { TELEMETRIA_BYTE, vbat_dv, estado, flags, chk };

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        for (uint8_t i = 0; i < 5; i++) {
            uint8_t next = (tx_head + 1) % TX_BUF_SIZE;
            if (next != tx_tail) {
                tx_buf[tx_head] = paquete[i];
                tx_head = next;
            }
        }
        UCSR0B |= (1 << UDRIE0); // Habilitar interrupción de transmisión
    }
}
