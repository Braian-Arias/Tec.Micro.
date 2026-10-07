# Materiales utilizados

Esta lista corresponde al armado fisico del Problema A. La simulacion de Proteus
usa algunos elementos adicionales que se indican al final.

## Armado fisico

| Cantidad | Material | Uso |
|---:|---|---|
| 1 | Arduino UNO R3 con ATmega328P | Ejecuta el programa y controla el sistema. |
| 1 | Sensor LM35 | Mide la temperatura ambiente. |
| 1 | LCD 16x2 con modulo I2C | Muestra temperatura, punto medio y estado. |
| 1 | Motor DC | Representa el ventilador. |
| 1 | MOSFET IRLZ44N | Permite controlar el motor desde el pin PWM del Arduino. |
| 1 | Diodo 1N4007 | Protege contra los picos producidos por el motor. |
| 1 | LED rojo | Representa el calefactor. |
| 1 | Resistencia de 220 ohm | Limita la corriente del LED. |
| 1 | Resistencia de 330 ohm | Se conecta entre D6 y el Gate del MOSFET. |
| 1 | Resistencia de 10 kohm | Mantiene apagado el MOSFET cuando no recibe señal. |
| 1 | Fuente externa regulada | Alimenta solamente el motor con su voltaje correspondiente. |
| 1 | Protoboard | Permite realizar las conexiones. |
| Varios | Cables jumper | Unen los componentes. |
| 1 | Cable USB | Alimenta y programa el Arduino, y permite usar UART. |

La fuente externa debe tener el voltaje indicado en el motor y entregar corriente
suficiente para hacerlo arrancar. Para un motor de 5 V se puede utilizar una
fuente regulada de 5 V y al menos 1 A.

## Materiales recomendados

| Cantidad | Material | Uso |
|---:|---|---|
| 1 | Capacitor de 100 nF | Ayuda a reducir ruido en la alimentacion. |
| 1 | Capacitor de 100 uF o mayor | Ayuda a evitar caidas de tension al arrancar el motor. |
| 1 | Multimetro | Permite comprobar voltajes y continuidad. |

## Prueba de temperaturas simuladas

Para probar los rangos sin calentar el LM35 se puede usar temporalmente:

| Cantidad | Material | Uso |
|---:|---|---|
| 1 | Potenciometro de 10 kohm | Simula la salida analogica del LM35. |
| 2 | Resistencias de 100 kohm | En paralelo equivalen a 50 kohm y hacen mas facil el ajuste. |

El potenciometro y el LM35 no deben estar conectados a A0 al mismo tiempo.

## Elementos usados solamente en Proteus

- LCD LM016L.
- PCF8574 separado del LCD.
- Virtual Terminal.
- Resistencias I2C de 4,7 kohm.
- Potenciometro de contraste del LCD.

En el armado fisico, el modulo I2C del LCD ya contiene el PCF8574, el ajuste de
contraste y normalmente las resistencias necesarias.
