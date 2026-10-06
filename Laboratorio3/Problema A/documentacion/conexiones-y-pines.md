# Conexiones y pines

La numeracion corresponde al simbolo del ATmega328P de 32 pines usado en Proteus.

## Entradas y salidas del ATmega328P

| Pin del ATmega | Conexion | Uso |
|---|---|---|
| PC0/ADC0, pin 23 | Salida VOUT del LM35 | Lectura de temperatura. |
| PD0/RXD, pin 30 | TXD del Virtual Terminal | Recepcion de comandos. |
| PD1/TXD, pin 31 | RXD del Virtual Terminal | Envio de datos. |
| PB0, pin 12 | Resistencia de 220 ohm y LED | Calefactor. |
| PD6/OC0A, pin 10 | Resistencia de 220 ohm y Gate del IRLZ44N | PWM del motor. |
| PC4/SDA, pin 27 | SDA del PCF8574, pin 15 | Datos I2C. |
| PC5/SCL, pin 28 | SCL del PCF8574, pin 14 | Reloj I2C. |
| RESET, pin 29 | Resistencia de 10 kohm a 5 V | Mantiene el micro fuera de reset. |
| AVCC, pin 18 | 5 V | Alimentacion de la parte analogica. |
| AREF, pin 20 | Capacitor de 100 nF a GND | Referencia del conversor ADC. |

## Sensor LM35

| Pin del LM35 | Conexion |
|---|---|
| 1, +VS | 5 V |
| 2, VOUT | PC0/ADC0 del ATmega, pin 23 |
| 3, GND | GND |

## PCF8574 y LCD

| PCF8574 | LCD LM016L |
|---|---|
| P0, pin 4 | RS, pin 4 |
| P1, pin 5 | RW, pin 5 |
| P2, pin 6 | E, pin 6 |
| P4, pin 9 | D4, pin 11 |
| P5, pin 10 | D5, pin 12 |
| P6, pin 11 | D6, pin 13 |
| P7, pin 12 | D7, pin 14 |

Los pines A0, A1 y A2 del PCF8574 se conectan a 5 V. De esta forma la direccion
I2C utilizada por el programa es `0x27`. SDA y SCL llevan resistencias de 4,7 kohm
hacia 5 V.

## Motor y transistor

La salida PD6 llega al Gate del IRLZ44N por una resistencia de 220 ohm. El Source
va a GND y el Drain al terminal negativo del motor. El positivo del motor va a 5 V.
El diodo 1N4007 se conecta en paralelo con el motor: catodo a 5 V y anodo al Drain.

