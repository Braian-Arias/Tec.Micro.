# Componentes

## Componentes pedidos por el problema

| Cantidad | Componente | Uso |
|---:|---|---|
| 1 | Arduino UNO R3 con ATmega328P | Ejecuta el programa. |
| 1 | Fotocelda LDR | Mide la luz que atraviesa el plastico. |
| 1 | Resistencia de 10 kohm | Forma el divisor de tension con la LDR. |
| 1 | WS2812 o NeoPixel de 5 V | Ilumina la muestra y muestra el color detectado. |
| 1 | Resistencia de 330 ohm | Protege la entrada de datos del WS2812. |
| 1 | Servomotor de 9 g | Dirige la muestra a una posicion diferente. |
| 1 | LCD 16x2 con I2C | Muestra lecturas y resultado. |
| 1 | Protoboard | Permite realizar el circuito. |
| Varios | Cables jumper | Unen los componentes. |
| 4 o mas | Plasticos translucidos de colores | Muestras para clasificar. |

## Elementos usados solamente en Proteus

| Cantidad | Componente | Uso |
|---:|---|---|
| 3 | Potenciometros de 10 kohm | Simulan las lecturas roja, verde y azul. |
| 1 | Virtual Terminal | Muestra los datos enviados por UART. |
| 1 | Analizador logico | Permite comprobar la señal de datos del WS2812. |

El kit ya contiene el Arduino, las fotoceldas, el servo, el LCD, la protoboard,
resistencias y cables. El WS2812 y las muestras de plastico deben conseguirse.
