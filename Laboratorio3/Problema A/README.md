# Problema A

Este trabajo controla la temperatura con un ATmega328P y un sensor LM35.

La temperatura se muestra en un LCD. Segun el valor medido, se enciende un LED
que representa el calefactor o un motor que representa el ventilador.

## Contenido

- `codigo`: programa en C.
- `simulacion`: circuito de Proteus y archivo HEX.
- `documentacion`: explicaciones del programa.
- `Armado fisico`: materiales, conexiones y pruebas.
- `evidencias`: capturas de la simulacion.
- `visualizacion`: programa Python, registros y graficas.

## Funcionamiento

| Temperatura | Resultado |
|---:|---|
| 0 a 16 C | Calefactor encendido |
| 17 a 28 C | Todo apagado |
| 29 a 39 C | Motor en velocidad baja |
| 40 a 50 C | Motor en velocidad media |
| 51 a 100 C | Motor en velocidad alta |
| Mayor a 100 C | Error del sensor |

El punto medio inicial es 22 C y se puede cambiar desde la comunicacion UART.

## Importante

El motor usa una fuente externa. El negativo de esa fuente se conecta al GND del
Arduino, pero el positivo no se conecta al pin de 5 V del Arduino.

La grafica y el CSV de la ultima prueba estan en `visualizacion/evidencias`.
