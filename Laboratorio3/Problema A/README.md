# Problema A - Control de temperatura

Esta carpeta contiene el codigo, la simulacion, la documentacion y las evidencias
del Problema A del Laboratorio 3.

## Carpetas

- `codigo`: programa en C para el ATmega328P.
- `simulacion`: proyecto de Proteus y archivo HEX.
- `documentacion`: explicacion del funcionamiento y maquina de estados.
- `evidencias`: capturas de las pruebas realizadas.

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

