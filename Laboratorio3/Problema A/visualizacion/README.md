# Visualizacion de resultados

El programa `registrar_y_graficar.py` recibe por UART los datos del ATmega328P,
los guarda en un archivo CSV y crea una grafica PNG.

La grafica muestra:

1. Temperatura medida a lo largo del tiempo.
2. Punto medio y rango ideal.
3. Encendido y apagado del calefactor.
4. Porcentaje de velocidad del ventilador.
5. Lecturas mayores a 100 C marcadas como error del sensor.

## Ultimo registro realizado

La prueba utilizada como evidencia duro 103,7 segundos y contiene 20 mediciones
con un punto medio de 22 C.

- [Datos del ultimo registro](evidencias/ultimo_registro.csv).
- [Grafica del ultimo registro](evidencias/ultimo_registro.png).
- [Captura de la terminal](evidencias/registro_prueba.png).

Durante la prueba se recorrieron los estados `CALENTAR`, `ESTABLE`, `VENT_BAJO`,
`VENT_MEDIO`, `VENT_ALTO` y `ERROR_SENSOR`. Las lecturas de 109 a 181 C se
conservan en el CSV, pero en la grafica se marcan como errores porque estan fuera
del intervalo admitido por el programa.

## Tabla usada para comprobar el funcionamiento

Con el punto medio colocado en 22 C, los resultados esperados son:

| Temperatura | Calefactor | Ventilador | Estado esperado |
|---:|---|---|---|
| 0 a 16 C | Encendido | Apagado | `CALENTAR` |
| 17 a 28 C | Apagado | Apagado | `ESTABLE` |
| 29 a 39 C | Apagado | 35 % | `VENT_BAJO` |
| 40 a 50 C | Apagado | 65 % | `VENT_MEDIO` |
| 51 a 100 C | Apagado | 100 % | `VENT_ALTO` |
| Mayor a 100 C | Apagado | Apagado | `ERROR_SENSOR` |

En este registro, la medicion de 16 C aparecio como `ESTABLE`. Esto muestra una
diferencia de un grado entre el programa cargado y la tabla requerida. El CSV no
se modifico porque representa lo que realmente envio el circuito.

## Formato recibido por UART

El ATmega328P envia una linea cada cinco segundos:

```text
DATO,38,22,0,35,VENT_BAJO
```

El orden es: temperatura, punto medio, calefactor, porcentaje del ventilador y
estado del sistema.

## Registrar una nueva prueba

Cerrar el monitor serial de Arduino, Microchip Studio o cualquier otro programa
que este usando el puerto. Luego ejecutar:

```powershell
py registrar_y_graficar.py --puerto COM3 --duracion 120
```

Los archivos nuevos se guardan automaticamente dentro de la carpeta `resultados`.
Para volver a crear la grafica de la evidencia actual se puede ejecutar:

```powershell
py registrar_y_graficar.py --archivo evidencias/ultimo_registro.csv
```
