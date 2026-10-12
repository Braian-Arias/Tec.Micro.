# Problema C - Clasificador de color

El sistema ilumina una muestra con rojo, verde y azul, toma una lectura para cada
color y la compara con cuatro referencias. Luego muestra el resultado en el LCD y
por UART, mueve el servomotor y enciende el WS2812 con el color detectado.

## Carpetas

- `codigo`: programa en C para el ATmega328P.
- `firmware`: lugar para el archivo HEX usado por Proteus.
- `simulacion`: conexiones y pruebas para Proteus.
- `documentacion`: componentes, conexiones y funcionamiento.
- `evidencias`: capturas que se agregaran durante las pruebas.

## Estado actual

- [x] Lectura y comprension del problema.
- [x] Seleccion de cuatro colores: rojo, verde, azul y amarillo.
- [x] Programa inicial en C.
- [x] Guia de conexiones para la simulacion.
- [ ] Simulacion funcionando en Proteus.
- [ ] Calibracion con plasticos reales.
- [ ] Armado fisico.
- [ ] Evidencias y video.

Los valores de referencia actuales sirven para la simulacion. No son los valores
definitivos del montaje fisico.
