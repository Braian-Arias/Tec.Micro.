# Guia para la simulacion en Proteus

La simulacion usa tres potenciometros porque Proteus no reproduce de forma simple
la luz que atraviesa el plastico. Cada potenciometro representa una lectura ADC.

## Componentes para buscar

- `ATMEGA328P`
- `PCF8574`
- `LM016L`
- 3 unidades de `POT-HG` de 10 kohm
- `MOTOR-PWMSERVO` o el servomotor disponible
- `VIRTUAL TERMINAL`
- `LOGIC ANALYSER`
- 2 resistencias de 4.7 kohm para I2C
- 1 potenciometro de 10 kohm para contraste del LCD
- Terminales `VCC` y `GND`

## Potenciometros de prueba

En los tres potenciometros:

- Un extremo a 5 V.
- El otro extremo a GND.
- Cursor del primero a `PC0/ADC0`.
- Cursor del segundo a `PC1/ADC1`.
- Cursor del tercero a `PC2/ADC2`.

En el simbolo QFP de Proteus corresponden normalmente a las patas 23, 24 y 25.

## Servomotor

- Señal a `PB1/OC1A`, normalmente pata 13 del simbolo QFP.
- Alimentacion a 5 V y GND.

## UART

- `PD1/TXD` del ATmega a `RXD` del Virtual Terminal.
- `PD0/RXD` del ATmega a `TXD` del Virtual Terminal.
- Configurar el terminal en 9600 baudios, 8 bits, sin paridad y 1 bit de parada.

## LCD con PCF8574

- `PC4/SDA` a `SDA` del PCF8574.
- `PC5/SCL` a `SCL` del PCF8574.
- Agregar una resistencia de 4.7 kohm desde SDA a 5 V.
- Agregar una resistencia de 4.7 kohm desde SCL a 5 V.
- Conectar `A0`, `A1` y `A2` del PCF8574 a 5 V para obtener la direccion `0x27`.
- `P0` a RS del LCD.
- `P1` a RW del LCD.
- `P2` a E del LCD.
- `P4`, `P5`, `P6` y `P7` a D4, D5, D6 y D7 del LCD.
- VSS del LCD a GND, VDD a 5 V y VEE al cursor del potenciometro de contraste.

## Salida del WS2812

Conectar `PD7`, normalmente pata 11 del simbolo QFP, a un canal del analizador
logico. Si la biblioteca de Proteus tiene un componente WS2812B, su entrada `DIN`
puede conectarse al mismo punto mediante una resistencia de 330 ohm.

## Configuracion del ATmega328P

- Clock Frequency: `16MHz`.
- CKDIV8: `Unprogrammed`.
- Program File: seleccionar `firmware/problema_c.hex` cuando este generado.

## Valores para comprobar cada color

| Prueba | A0 | A1 | A2 | Resultado esperado |
|---|---:|---:|---:|---|
| Rojo | 800 | 220 | 180 | ROJO, servo a 20 grados |
| Verde | 220 | 800 | 240 | VERDE, servo a 65 grados |
| Azul | 180 | 250 | 800 | AZUL, servo a 115 grados |
| Amarillo | 780 | 720 | 180 | AMARILLO, servo a 160 grados |

Los valores de los potenciometros no tienen que ser exactos. El programa permite
una diferencia total de hasta 600 para reconocer el color mas cercano.
