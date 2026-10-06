# LCD y comunicacion I2C

Se utiliza un LCD 16x2 para mostrar la temperatura, el punto medio y el estado del
sistema. Para no ocupar muchos pines del ATmega328P, el LCD se conecta mediante un
expansor PCF8574.

La comunicacion I2C utiliza solamente dos lineas:

- `SDA`: transporta los datos y se conecta a PC4, pin 27 del ATmega.
- `SCL`: transporta el reloj y se conecta a PC5, pin 28 del ATmega.

Las dos lineas llevan resistencias de 4,7 kohm hacia 5 V. Los pines A0, A1 y A2 del
PCF8574 estan conectados a 5 V, por lo que el programa usa la direccion `0x27`.
La velocidad del bus es aproximadamente 100 kHz.

El PCF8574 envia al LCD las señales RS, RW, E y los cuatro bits de datos D4 a D7.
Se usa el modo de 4 bits, por eso D0, D1, D2 y D3 quedan sin conectar.

En la primera linea del LCD aparece la temperatura. En la segunda se muestra el
punto medio y una indicacion como `CALOR`, `ESTABLE`, `V.BAJA`, `V.MEDIA` o `V.ALTA`.
El potenciometro de 10 kohm conectado a VEE permite ajustar el contraste.

