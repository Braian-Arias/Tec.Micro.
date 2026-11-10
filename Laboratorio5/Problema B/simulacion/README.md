# Simulación del WDT como despertador — Problema B

Proyecto para **PICSimLab 0.9.2**, con Arduino Uno / ATmega328P a 16 MHz. El archivo `.pzw` incluye el firmware compilado y la configuración de la placa; no requiere Microchip Studio ni cargar un `.hex` aparte.

## Abrir

1. [Descargar Laboratorio5_WDT.pzw](https://github.com/Braian-Arias/Tec.Micro./raw/refs/heads/main/Laboratorio5/Problema%20B/simulacion/Laboratorio5_WDT.pzw). Si se abre la página del archivo en GitHub, usar **Download raw file**.
2. En PICSimLab, seleccionar **File > Load Workspace** y abrir el archivo descargado.
3. Dejar la placa encendida. Usar **Reset** para comenzar la secuencia desde el inicio.

## Qué observar

Sin intervenir, el firmware recorre esta secuencia desde el reinicio:

| Tiempo de simulación aproximado | Comportamiento esperado |
| --- | --- |
| 0 a 10 s | Estado ACTIVO. El LED integrado D13 (PB5) cambia cada 200 ms. |
| 10 a 30 s | Estado STANDBY, con Sleep Idle. El LED continúa cambiando cada 200 ms. |
| A partir de 30 s | Estado APAGADO, con Sleep Power-down. El LED se apaga al entrar. |
| Cada 8 s en APAGADO | El WDT genera una interrupción; se lee el ADC y el LED D13 cambia de estado. Después, el microcontrolador vuelve a dormir. |

Para observar la señal, abrir el osciloscopio de PICSimLab. El canal 1 está configurado en **PB5/13** y la base de tiempo en **1000 ms/div**. En APAGADO se espera un cambio cada aproximadamente 8 segundos (un ciclo completo de encendido y apagado dura aproximadamente 16 segundos). El tiempo real puede variar según la velocidad del simulador.

Este proyecto muestra la prueba del **WDT como despertador**. El LED permite observar los despertares periódicos; por sí solo no distingue ACTIVO de STANDBY. No contiene una prueba de reset por WDT ni el montaje de comunicación Bluetooth.
