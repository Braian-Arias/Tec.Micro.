# Funcionamiento

## Secuencia

1. Encender el WS2812 en rojo.
2. Esperar un momento y leer la fotocelda.
3. Repetir la lectura con luz verde.
4. Repetir la lectura con luz azul.
5. Comparar las tres lecturas con las referencias guardadas.
6. Elegir el color que tenga la menor diferencia.
7. Mover el servo al angulo de ese color.
8. Mostrar el resultado en el WS2812, LCD y UART.

## Referencias iniciales de simulacion

| Color | Rojo | Verde | Azul | Angulo |
|---|---:|---:|---:|---:|
| Rojo | 800 | 220 | 180 | 20 grados |
| Verde | 220 | 800 | 240 | 65 grados |
| Azul | 180 | 250 | 800 | 115 grados |
| Amarillo | 780 | 720 | 180 | 160 grados |

La diferencia se calcula sumando la distancia entre cada lectura y su referencia:

```text
diferencia = |R medida - R referencia|
            + |G medida - G referencia|
            + |B medida - B referencia|
```

Se selecciona la referencia con la diferencia mas pequeña. Si esa diferencia es
mayor a 600, la muestra se informa como `DESCONOCIDO` y el servo va a 90 grados.

## Importante para el armado fisico

Las referencias de esta tabla son solamente valores de prueba. Cuando se consiga
el WS2812 se debe medir cada plastico varias veces y reemplazar los valores por los
promedios reales.
