# Armado fisico

Esta carpeta contiene la informacion necesaria para montar y comprobar el
Problema A con componentes reales.

## Contenido

- [Lista de materiales](componentes.md).
- [Conexiones completas](conexiones.md).
- [Guia de pruebas](pruebas.md).
- [Evidencias del armado](evidencias/README.md).
- [Diagrama del motor y MOSFET](conexion-motor-mosfet.png).

## Orden recomendado

1. Armar y probar el LM35 junto con el LCD.
2. Agregar el LED que representa el calefactor.
3. Armar el circuito del MOSFET sin conectar la fuente del motor.
4. Revisar Gate, Drain, Source, diodo y GND comun.
5. Conectar la fuente externa del motor y probar los rangos.
6. Guardar fotos o capturas en la carpeta `evidencias`.

El Arduino se alimenta mediante USB. El motor utiliza una fuente externa acorde
a su voltaje, pero el negativo de esa fuente se une al GND del Arduino.
