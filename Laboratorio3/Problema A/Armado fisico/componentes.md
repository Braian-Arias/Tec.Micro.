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
