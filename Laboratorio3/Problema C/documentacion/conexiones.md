# Conexiones previstas

## Pines principales

| Elemento | Conexion Arduino | Puerto ATmega328P |
|---|---|---|
| Fotocelda o lectura roja en simulacion | `A0` | `PC0 / ADC0` |
| Lectura verde solamente en simulacion | `A1` | `PC1 / ADC1` |
| Lectura azul solamente en simulacion | `A2` | `PC2 / ADC2` |
| LCD SDA | `A4` | `PC4 / SDA` |
| LCD SCL | `A5` | `PC5 / SCL` |
| Datos del WS2812 | `D7` | `PD7` |
| Señal del servomotor | `D9` | `PB1 / OC1A` |
| UART RX | `D0` | `PD0 / RXD` |
| UART TX | `D1` | `PD1 / TXD` |

## Fotocelda para el futuro armado fisico

```text
5V --- LDR ---+--- A0
              |
             10 kohm
              |
             GND
```

El punto entre la LDR y la resistencia se conecta a `A0`.

## WS2812

| Pin WS2812 | Conexion |
|---|---|
| `5V` | 5 V |
| `GND` | GND comun |
| `DIN` | D7 mediante resistencia de 330 ohm |

Se debe usar `DIN`, no `DOUT`. El programa envia los colores en orden GRB.

## Servomotor de 9 g

| Cable habitual | Conexion |
|---|---|
| Marron o negro | GND |
| Rojo | 5 V de fuente adecuada |
| Naranja o amarillo | D9 |

Si se usa una fuente separada para el servo, su negativo debe conectarse al GND
del Arduino. En la simulacion todos los GND son comunes.

## LCD I2C

| LCD | Arduino |
|---|---|
| `GND` | GND |
| `VCC` | 5 V |
| `SDA` | A4 |
| `SCL` | A5 |

El programa usa la direccion `0x27`.
