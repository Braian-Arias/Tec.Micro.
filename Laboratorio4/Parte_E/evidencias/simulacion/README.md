# Capturas de PICSimLab

Capturas originales aportadas por Bruno, conservadas sin editar.

## 01_vista_general_hold.png

Se observan Arduino Uno / ATmega328P, reloj de 16 MHz, MPU6050, matriz RGB, botones, buzzer y terminal a 38400 baud. El terminal muestra E=2 (HOLD), F=4, C=4 y COLOR=0, con un punto azul en la matriz.

Esta imagen documenta el montaje abierto y el estado HOLD mostrado. No demuestra por si sola la respuesta al movimiento, la calibracion ni la persistencia en EEPROM. Para comprobar HOLD hacen falta dos posiciones distintas del sensor con el punto conservado.

Pendientes: nivelado en E=1, inclinacion en X, Y y combinada, secuencia HOLD, calibracion y reinicio, y video de parpadeo/pitido en el borde.

## 02_punto_verde.png

Se observa un punto verde desplazado en la matriz. El MPU6050 muestra aproximadamente ACC X=-0.24 g, ACC Y=-0.80 g y ACC Z=-0.04 g. El terminal no esta desplegado, por lo que no pueden verificarse el estado E ni los angulos reportados. Se conserva como observacion de la matriz; no se clasifica como nivelado ni como una prueba controlada de inclinacion en un solo eje.
