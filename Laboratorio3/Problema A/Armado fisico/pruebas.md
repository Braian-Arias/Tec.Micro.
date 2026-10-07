# Guia de pruebas del armado fisico

## Antes de encender

- Revisar que no existan cables sueltos o cortocircuitos.
- Confirmar que el LM35 tenga 5 V, A0 y GND en el orden correcto.
- Confirmar que el LCD use A4 para SDA y A5 para SCL.
- Revisar que el IRLZ44N este conectado como Gate, Drain y Source.
- Confirmar que la franja gris del diodo apunte al positivo del motor.
- Unir el negativo de la fuente externa con el GND del Arduino.
- No conectar el positivo externo al pin 5 V del Arduino.

## Prueba por etapas

1. Encender solamente el Arduino, LM35 y LCD.
2. Comprobar que el LCD muestre una temperatura posible y `PM:22`.
3. Conectar el LED calefactor y comprobar su funcionamiento.
4. Conectar el motor mediante el MOSFET y la fuente externa.
5. Probar cada rango y comparar el resultado con la tabla.

| Temperatura | Resultado esperado |
|---:|---|
| 0 a 16 C | Calefactor encendido y motor apagado |
| 17 a 28 C | Calefactor y motor apagados |
| 29 a 39 C | Motor en velocidad baja |
| 40 a 50 C | Motor en velocidad media |
| 51 a 100 C | Motor en velocidad alta |

## Observaciones

- El LM35 real puede variar aproximadamente 1 o 2 C respecto de otro termometro.
- Si el motor solamente hace ruido en velocidad baja, el PWM puede no alcanzar
  para vencer la fuerza inicial del motor.
- Para probar valores exactos se puede reemplazar temporalmente el LM35 por el
  potenciometro indicado en [conexiones.md](conexiones.md).
- El LM35 y el potenciometro de prueba no deben compartir A0 al mismo tiempo.

## Evidencias recomendadas

- Foto general del circuito apagado.
- Foto del circuito encendido mostrando el LCD.
- Una evidencia por cada rango de temperatura.
- Captura del monitor serial con temperatura, punto medio y estado.
- Foto donde se vea la fuente externa y el GND comun.
