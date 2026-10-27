# Capturas de PICSimLab

Capturas originales aportadas por Bruno, conservadas sin editar.

## 01_vista_general_hold.png

Se observan Arduino Uno / ATmega328P, reloj de 16 MHz, MPU6050, matriz RGB, botones, buzzer y terminal a 38400 baud. El terminal muestra E=2 (HOLD), F=4, C=4 y COLOR=0, con un punto azul en la matriz.

Esta imagen documenta el montaje abierto y el estado HOLD mostrado. No demuestra por si sola la respuesta al movimiento, la calibracion ni la persistencia en EEPROM. Para comprobar HOLD hacen falta dos posiciones distintas del sensor con el punto conservado.

Pendientes: inclinacion en X, Y y combinada, secuencia HOLD, calibracion y reinicio, y video de parpadeo/pitido en el borde.

## 02_punto_verde.png

Se observa un punto verde desplazado en la matriz. El MPU6050 muestra aproximadamente ACC X=-0.24 g, ACC Y=-0.80 g y ACC Z=-0.04 g. El terminal no esta desplegado, por lo que no pueden verificarse el estado E ni los angulos reportados. Se conserva como observacion de la matriz; no se clasifica como nivelado ni como una prueba controlada de inclinacion en un solo eje.

## 03_sensor_nivelado_en_hold.png

El sensor muestra ACC X=0.00 g, ACC Y=0.00 g y ACC Z=1.04 g. El terminal muestra R=0.0, P=0.0, F=4, C=4, E=2 (HOLD) y COLOR=0. No se distingue un punto encendido en la matriz. Los giroscopios muestran -80, -95 y -95. Esta captura registra una condicion a revisar; no acredita el nivelado en modo MEDIR. La siguiente comprobacion debe salir de HOLD y verificar E=1 junto con la visualizacion de la matriz.

## 04_nivelado_medir.png

Prueba de nivelado documentada en PICSimLab: ACC X=0.00 g, ACC Y=0.00 g, ACC Z=1.00 g y los tres giroscopios en cero. El terminal a 38400 baud muestra R=0.0, P=0.0, F=4, C=4, E=1 (MEDIR) y COLOR=0. Se observa el punto azul en la posicion central elegida por el programa. La placa es Arduino Uno / ATmega328P a 16 MHz. La captura coincide con el resultado esperado para esta condicion estatica; las demas pruebas siguen pendientes.

## 05_combinada_terminal_por_verificar.png

El sensor muestra ACC X=-0.30 g, ACC Y=0.34 g y ACC Z=0.88 g, con giroscopios en cero. Se observa un punto amarillo desplazado en ambos ejes. Sin offsets, estas aceleraciones corresponden aproximadamente a roll +21 grados y pitch +18 grados. Sin embargo, las lineas visibles del terminal indican E=1, COLOR=1 y valores negativos de roll con columna 0, que no coinciden con la matriz. No se puede determinar con esta captura si se trata de historial del terminal o de otra causa. Se conserva como registro de una discrepancia pendiente de comprobar, sin dar por aprobada la prueba combinada. Repetir con sensor quieto, terminal limpio y nuevas lecturas visibles.

## 06_terminal_error_lectura.png

ACC X=-0.34 g, ACC Y=0.00 g y ACC Z=0.88 g, giroscopios en cero y punto verde desplazado. El terminal visible alterna MPU6050 OK con ERROR: sensor sin lectura valida, e imprime R=93.5, P=-16.8, F=4, C=4, E=1 y COLOR=1. Estas lecturas no coinciden con la condicion mostrada por sensor y matriz. Sin offsets se esperan roll aproximadamente 0 grados y pitch aproximadamente +21 grados. La imagen documenta mensajes de fallo, pero no permite distinguir si pertenecen al historial del terminal o a la ejecucion actual. Pendiente repetir con terminal limpio y observar nuevas lineas durante varios segundos. No se clasifica como prueba aprobada del eje Y ni de recuperacion ante fallos.
