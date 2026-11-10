# WDT como mecanismo de reset

Archivos: `wdt_reset.c` (fuente), `wdt_reset.hex` (firmware) y `WDT_Reset_PICSimLab.pzw` (simulación para Arduino Uno / ATmega328P a 16 MHz).

Para la prueba, abrir el `.pzw` y pulsar el botón **D2**. El programa deja de atender el WDT; tras el reset, **D4** debe encenderse porque se detectó `WDRF` en `MCUSR`. **D5** indica arranque normal y **D13** actividad o bloqueo. El WDT está configurado para un período nominal de **2 s**.

**Pendiente:** registrar la secuencia y medir el tiempo en la simulación. Todavía no hay evidencia experimental.
