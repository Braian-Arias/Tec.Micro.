# Laboratorio 4 - Problema E

Nivel con ATmega328P a 16 MHz, MPU6050 y matriz RGB WS2812B de 8 x 8.

## Archivos

- `main.c`: programa completo para Microchip Studio.
- `evidencias/maquina_de_estados.png`: diagrama de estados.
- `evidencias/conexiones.png`: esquema de conexiones.
- `simulacion/Problema_E.pzw`: proyecto para PICSimLab 0.9.2, con el firmware compilado a partir de este main.c.
- `evidencias/captura_compilacion.png`: pendiente; se agrega despues de compilar en Microchip Studio.

## Compilar

Crear un proyecto GCC C Executable Project para ATmega328P. Reemplazar main.c por el de esta carpeta, usar optimizacion -Os y agregar m en AVR/GNU Linker > Libraries. F_CPU=16000000UL ya esta definido. Compilar con F7. No agregar los archivos de la version modular junto a este main.c.

## Simular

Abrir simulacion/Problema_E.pzw en PICSimLab. Comprobar Arduino Uno, ATmega328P, reloj de 16 MHz y terminal a 38400 baud. En Spare Parts aparecen MPU6050, matriz WS2812B, dos botones y buzzer.

Para la posicion nivelada ajustar ax=0, ay=0, az=1 g; debe verse un punto azul en (4,4). Para roll positivo de unos 20 grados usar ax=0, ay=0.342, az=0.940; para pitch positivo usar ax=-0.342, ay=0, az=0.940. Mantener el giroscopio en cero en estas pruebas estaticas. Boton 1: HOLD. Boton 2: calibracion, con el sensor quieto.

## Estado de las evidencias

El codigo compilo con AVR-GCC de Microchip Studio y paso 21 comprobaciones automatizadas del firmware. El paquete PICSimLab se verifico estructuralmente; falta probarlo dentro de PICSimLab y registrar los resultados observados. Los diagramas documentan el diseno; no son capturas de una ejecucion. La captura de compilacion y las evidencias del montaje fisico estan pendientes.
