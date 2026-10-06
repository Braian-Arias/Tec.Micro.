# Sensor de temperatura LM35

El LM35 es un sensor analogico. Su salida aumenta aproximadamente 10 mV por cada
grado Celsius. Por ejemplo, a 25 C entrega cerca de 250 mV.

La salida del sensor se conecta al canal ADC0 del ATmega328P. El conversor analogico
digital transforma el voltaje en un valor entre 0 y 1023. Como se usa una referencia
de 5 V, el programa calcula la temperatura con una cuenta equivalente a:

```text
temperatura = lectura_ADC * 500 / 1023
```

El numero 500 representa los 500 grados que corresponderian a una salida de 5 V,
teniendo en cuenta que el LM35 entrega 10 mV por grado.

Para evitar que la lectura cambie demasiado por pequeñas variaciones, el programa
toma diez muestras, calcula su promedio y luego obtiene la temperatura entera.
La medicion se actualiza aproximadamente cada cinco segundos.

## Conexion

- Pin 1 del LM35 a 5 V.
- Pin 2, VOUT, a PC0/ADC0 del ATmega328P.
- Pin 3 a GND.

