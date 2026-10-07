# Visualizacion de resultados

El programa `registrar_y_graficar.py` recibe los datos enviados por UART, los
guarda en un archivo CSV y crea una grafica PNG.

La grafica incluye:

1. Evolucion de la temperatura medida.
2. Encendido y apagado del calefactor.
3. Porcentaje de velocidad del ventilador.
4. Punto medio configurado.
5. Rango ideal de temperatura alrededor del punto medio.

Para un punto medio de 22 C, el rango ideal mostrado es de 17 a 28 C.

## Formato recibido por UART

El ATmega328P envia una linea cada cinco segundos:

```text
DATO,30,22,0,35,VENT_BAJO
```

El orden es: temperatura, punto medio, calefactor, porcentaje del ventilador y
estado del sistema.

## Preparacion

1. Instalar Python 3.
2. Abrir una terminal dentro de esta carpeta.
3. Instalar las bibliotecas necesarias:

```powershell
py -m pip install -r requirements.txt
```

## Registrar una prueba real

Cerrar primero el monitor serial de Arduino, Microchip Studio o cualquier programa
que este utilizando el puerto. Luego ejecutar:

```powershell
py registrar_y_graficar.py --puerto COM3 --duracion 120
```

- `COM3` se cambia si el Arduino aparece en otro puerto.
- `120` es la duracion de la prueba en segundos.
- Se recomienda registrar al menos dos minutos porque el circuito envia una
  medicion cada cinco segundos.

Los archivos se guardan automaticamente dentro de `resultados`:

- Un CSV con todas las mediciones.
- Una imagen PNG con las graficas.

## Probar sin conectar el circuito

El archivo `datos_ejemplo.csv` permite comprobar el programa:

```powershell
py registrar_y_graficar.py --archivo datos_ejemplo.csv
```

Este comando genera `datos_ejemplo.png`. Los datos de ejemplo sirven para revisar
el funcionamiento del programa, pero la entrega final debe incluir una prueba
registrada con el circuito real.

## Evidencias para GitHub

Se recomienda subir:

- El CSV de una prueba completa.
- La grafica PNG creada a partir de esa prueba.
- Una foto del circuito durante el registro.
- Una breve explicacion de los cambios observados en la temperatura y las salidas.
