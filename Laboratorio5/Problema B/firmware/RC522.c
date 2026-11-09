
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <string.h>

#include "RC522.h"
#include "SPI.h"
#include "UART.h"

/* Inicializa el pin de reset del RC522 */
void mfrc522_resetPinInit(void)
{
    DDRB |= (1 << PB1);
    PORTB |= (1 << PB1);
}

/* Reinicia el módulo RC522 */
void mfrc522_reset(void)
{
    PORTB &= ~(1 << PB1);
    _delay_ms(2);
    PORTB |= (1 << PB1);
    _delay_ms(50);
}

/* Escribe un registro del RC522 */
void mfrc522_write(uint8_t reg, uint8_t value)
{
    PORTB &= ~(1 << PB2);
    spi_transfer((reg << 1) & 0x7E);
    spi_transfer(value);
    PORTB |= (1 << PB2);
}

/* Lee un registro del RC522 */
uint8_t mfrc522_read(uint8_t reg)
{
    uint8_t value;

    PORTB &= ~(1 << PB2);
    spi_transfer(((reg << 1) & 0x7E) | 0x80);
    value = spi_transfer(0x00);
    PORTB |= (1 << PB2);

    return value;
}

/* Activa bits de un registro */
void mfrc522_setBitMask(uint8_t reg, uint8_t mask)
{
    uint8_t value = mfrc522_read(reg);
    mfrc522_write(reg, value | mask);
}

/* Desactiva bits de un registro */
void mfrc522_clearBitMask(uint8_t reg, uint8_t mask)
{
    uint8_t value = mfrc522_read(reg);
    mfrc522_write(reg, value & (uint8_t)(~mask));
}

/* Inicializa el RC522 */
void mfrc522_init(void)
{
    mfrc522_resetPinInit();
    mfrc522_reset();

    mfrc522_write(0x2A, 0x8D);
    mfrc522_write(0x2B, 0x3E);
    mfrc522_write(0x2D, 30);
    mfrc522_write(0x2C, 0);
    mfrc522_write(0x15, 0x40);
    mfrc522_write(0x11, 0x3D);

    /* Activa la antena */
    mfrc522_setBitMask(0x14, 0x03);
    _delay_ms(10);
}

/*
 * Detecta una tarjeta y obtiene sus primeros 4 bytes de UID.
 *
 * Devuelve:
 * 1 = tarjeta detectada y UID válido
 * 0 = no se detectó una tarjeta válida
 */
uint8_t mfrc522_standard(uint8_t *card_uid)
{
    uint8_t irq;
    uint8_t error;
    uint8_t cantidad;
    uint8_t datos[5];
    uint16_t tiempo;

    /* Limpia el UID de salida */
    for (uint8_t i = 0; i < 4; i++)
    {
        card_uid[i] = 0;
    }

    /* Solicita una tarjeta */
    mfrc522_write(0x0D, 0x07);
    mfrc522_write(0x04, 0x7F);
    mfrc522_write(0x0A, 0x80);
    mfrc522_write(0x09, 0x26);
    mfrc522_write(0x01, 0x0C);
    mfrc522_setBitMask(0x0D, 0x80);

    tiempo = 1000;

    do
    {
        irq = mfrc522_read(0x04);
        tiempo--;
    }
    while (!(irq & 0x30) && tiempo > 0);

    mfrc522_clearBitMask(0x0D, 0x80);

    if (tiempo == 0)
    {
        return 0;
    }

    error = mfrc522_read(0x06);

    if (error & 0x1B)
    {
        return 0;
    }

    cantidad = mfrc522_read(0x0A);

    /* ATQA debe contener 2 bytes */
    if (cantidad < 2)
    {
        return 0;
    }

    /* Vacía la respuesta ATQA */
    while (cantidad > 0)
    {
        (void)mfrc522_read(0x09);
        cantidad--;
    }

    /* Anticolisión: solicita los 4 bytes de UID y el BCC */
    mfrc522_write(0x0D, 0x00);
    mfrc522_write(0x04, 0x7F);
    mfrc522_write(0x0A, 0x80);
    mfrc522_write(0x09, 0x93);
    mfrc522_write(0x09, 0x20);
    mfrc522_write(0x01, 0x0C);
    mfrc522_setBitMask(0x0D, 0x80);

    tiempo = 1000;

    do
    {
        irq = mfrc522_read(0x04);
        tiempo--;
    }
    while (!(irq & 0x30) && tiempo > 0);

    mfrc522_clearBitMask(0x0D, 0x80);

    if (tiempo == 0)
    {
        return 0;
    }

    error = mfrc522_read(0x06);

    if (error & 0x1B)
    {
        return 0;
    }

    cantidad = mfrc522_read(0x0A);

    /* UID de 4 bytes más el BCC */
    if (cantidad != 5)
    {
        while (cantidad > 0)
        {
            (void)mfrc522_read(0x09);
            cantidad--;
        }

        return 0;
    }

    for (uint8_t i = 0; i < 5; i++)
    {
        datos[i] = mfrc522_read(0x09);
    }

    /* Comprueba el byte de control (BCC) */
    if ((uint8_t)(datos[0] ^ datos[1] ^ datos[2] ^ datos[3]) != datos[4])
    {
        return 0;
    }

    /* Copia solamente los 4 bytes del UID */
    for (uint8_t i = 0; i < 4; i++)
    {
        card_uid[i] = datos[i];
    }

    return 1;
}
