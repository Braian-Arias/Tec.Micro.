#ifndef LCD_I2C_H_
#define LCD_I2C_H_

#include <stdint.h>

void i2c_iniciar(void);
void lcd_iniciar(void);
void lcd_posicion(uint8_t fila, uint8_t columna);
void lcd_texto(const char *texto);
void lcd_numero(uint16_t numero);
void lcd_limpiar(void);
void lcd_limpiar_linea(uint8_t fila);

#endif