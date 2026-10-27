
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#include "SPI.h"
#include "RC522.h"
#include "LCD_I2C.h"
#include "UART.h"

static uint8_t uid_leido[4];

// Convertir un valor de 4 bits a hexadecimal 
static char hex_digito(uint8_t valor)
{
    valor &= 0x0F;

    if (valor < 10)
    {
        return (char)('0' + valor);
    }

    return (char)('A' + valor - 10);
}

// Mostrar el UID de la tarjeta en la LCD 
static void mostrar_uid_lcd(const uint8_t *uid)
{
    lcd_limpiar();

    lcd_posicion(0, 0);
    lcd_texto("Tarjeta detect.");

    lcd_posicion(1, 0);
    lcd_texto("UID: ");

    for (uint8_t i = 0; i < 4; i++)
    {
        char texto[3];

        texto[0] = hex_digito(uid[i] >> 4);
        texto[1] = hex_digito(uid[i]);
        texto[2] = '\0';

        lcd_texto(texto);

        if (i < 3)
        {
            lcd_texto(" ");
        }
    }
}

int main(void)
{
    // Iniciar UART a 9600 baudios 
    uart_init(103);

    // Iniciar LCD mediante I2C 
    i2c_iniciar();
    lcd_iniciar();

    // Iniciar SPI y lector RFID 
    spi_init();
    mfrc522_resetPinInit();
    mfrc522_init();

    // Mostrar pantalla inicial 
    lcd_limpiar();

    lcd_posicion(0, 0);
    lcd_texto("CAJERO AUTOMAT.");

    lcd_posicion(1, 0);
    lcd_texto("Acerque tarjeta");

    // Mensajes de inicializacion por UART 
    uart_print("\r\n========================\r\n");
    uart_print(" CAJERO - ACTUALIZACION 1\r\n");
    uart_print("========================\r\n");

    uart_print("SPI inicializado.\r\n");
    uart_print("LCD I2C inicializado.\r\n");
    uart_print("RC522 inicializado.\r\n");
    uart_print("Esperando tarjeta...\r\n");

    while (1)
    {
        // Intentar leer una tarjeta RFID 
        if (mfrc522_standard(uid_leido))
        {
            // Mostrar UID por UART 
            uart_print("\r\nTarjeta detectada\r\n");
            uart_print("UID: ");
            uart_print_hex_array(uid_leido, 4);

            // Mostrar UID en LCD 
            mostrar_uid_lcd(uid_leido);

            _delay_ms(1500);

            // Volver a la pantalla inicial 
            lcd_limpiar();

            lcd_posicion(0, 0);
            lcd_texto("CAJERO AUTOMAT.");

            lcd_posicion(1, 0);
            lcd_texto("Acerque tarjeta");

            // Evitar lecturas repetidas inmediatas
            _delay_ms(500);
        }
    }

    return 0;
}
```
