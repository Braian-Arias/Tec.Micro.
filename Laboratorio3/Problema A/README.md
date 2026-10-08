# Problema A - Control de temperatura

Esta carpeta contiene el codigo, la simulacion, la documentacion y las evidencias
del Problema A del Laboratorio 3.

## Carpetas

- `codigo`: programa en C para el ATmega328P.
- `simulacion`: proyecto de Proteus y archivo HEX.
- `documentacion`: explicacion del funcionamiento y maquina de estados.
- `evidencias`: capturas de las pruebas realizadas.

## Armado fisico

- [Carpeta del armado fisico](Armado%20fisico/).
- [Materiales utilizados](Armado%20fisico/componentes.md).
- [Conexiones completas](Armado%20fisico/conexiones.md).
- [Guia de pruebas](Armado%20fisico/pruebas.md).
- [Evidencias del armado](Armado%20fisico/evidencias/).

## Documentacion del funcionamiento

- [Funcionamiento del LM35](documentacion/sensor-lm35.md).
- [LCD y comunicacion I2C](documentacion/lcd-i2c.md).

El motor utiliza una fuente externa y no se alimenta desde el pin `5V` del
Arduino. El negativo de esa fuente se conecta al GND comun del circuito.

## Visualizacion de resultados

- [Programa de registro y graficas](visualizacion/README.md).
- [Codigo Python](visualizacion/registrar_y_graficar.py).
- [Datos del ultimo registro](visualizacion/evidencias/ultimo_registro.csv).
- [Grafica del ultimo registro](visualizacion/evidencias/ultimo_registro.png).

El programa guarda las mediciones recibidas por UART y grafica la temperatura,
las acciones del calefactor y del ventilador, el punto medio y el rango ideal.

## Rangos utilizados

| Temperatura | Calefactor | Motor |
|---|---|---|
| 0 a 16 C | Encendido | Apagado |
| 17 a 28 C | Apagado | Apagado |
| 29 a 39 C | Apagado | Velocidad baja |
| 40 a 50 C | Apagado | Velocidad media |
| 51 a 100 C | Apagado | Velocidad alta |

## Estado de la entrega

- [ ] Agregar el codigo completo `main.c`.
- [x] Proyecto de Proteus agregado.
- [x] Archivo HEX agregado.
- [x] Captura de compilacion agregada.
- [x] Tabla de rangos agregada.
- [ ] Agregar captura del circuito completo.
- [ ] Agregar pruebas de los cinco rangos.
- [ ] Agregar evidencia del LCD.
- [ ] Agregar evidencia de la terminal UART.
- [ ] Agregar evidencia del cambio del punto medio.
- [x] Agregar programa para registrar y graficar resultados.
- [ ] Agregar CSV y grafica de una prueba con el circuito real.
