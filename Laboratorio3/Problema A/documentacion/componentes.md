# Lista de componentes

Para la simulacion del control de temperatura se utilizaron los siguientes componentes:

| Cantidad | Componente | Funcion |
|---:|---|---|
| 1 | ATmega328P | Lee el sensor y controla todas las salidas. |
| 1 | LM35 | Mide la temperatura. |
| 1 | LCD LM016L 16x2 | Muestra la temperatura, el punto medio y el estado. |
| 1 | PCF8574 | Comunica el ATmega con el LCD mediante I2C. |
| 1 | Potenciometro de 10 kohm | Ajusta el contraste del LCD. |
| 1 | Virtual Terminal | Permite usar el menu UART. |
| 1 | LED rojo | Representa el calefactor. |
| 1 | Motor DC de 5 V | Representa el ventilador. |
| 1 | IRLZ44N | Permite controlar el motor con la salida PWM. |
| 1 | Diodo 1N4007 | Protege el circuito de los picos producidos por el motor. |
| 2 | Resistencias de 220 ohm | Limitan la corriente del LED y de la compuerta del transistor. |
| 2 | Resistencias de 10 kohm | Se usan en RESET y como resistencia de bajada del transistor. |
| 2 | Resistencias de 4,7 kohm | Mantienen en alto las lineas SDA y SCL del bus I2C. |
| 4 | Capacitores de 100 nF | Ayudan a estabilizar la alimentacion y la referencia analogica. |

La alimentacion utilizada es de 5 V y todas las tierras se conectan a un GND comun.

