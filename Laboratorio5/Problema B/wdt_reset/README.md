# Punto 7 — WDT como mecanismo de reset

Prueba independiente para ATmega328P a 16 MHz. El programa atiende el WDT en funcionamiento normal. Al pulsar **D2**, entra intencionalmente en un bucle que deja de atenderlo. El WDT está configurado en modo **reset**, con un período nominal de 2 s.

## Archivos

- `wdt_reset.c`: fuente para Microchip Studio o `avr-gcc`.
- `wdt_reset.hex`: firmware compilado.
- `WDT_Reset_PICSimLab.pzw`: proyecto para PICSimLab, con el HEX, un pulsador en D2, tres indicadores y un registro VCD configurados.

## Indicadores

| Pin | Función | Estado esperado |
| --- | --- | --- |
| D2 / PD2 | Pulsador activo en bajo | Suelto: 1; pulsado: 0 |
| D13 / PB5 | Actividad y bloqueo | Parpadea normalmente; queda encendido durante el bloqueo; se apaga al reiniciar |
| D5 / PD5 | Arranque normal | Encendido antes del reset; apagado después |
| D4 / PD4 | Reset causado por WDT | Apagado antes; encendido después del reset |

## Secuencia para demostrarlo

1. Abrir `WDT_Reset_PICSimLab.pzw` en PICSimLab y verificar que la placa sea **Arduino Uno / ATmega328P / 16 MHz**.
2. Al iniciar, comprobar que D5 esté encendido, D4 apagado y D13 parpadee. Esto indica que el programa ejecuta `wdt_reset()` periódicamente.
3. Pulsar brevemente el primer botón de la pieza **Push Buttons** (D2) y soltarlo. D13 debe quedar encendido mientras el programa permanece bloqueado.
4. Esperar el período del WDT. Un reinicio real se reconoce porque D5 se apaga, D13 se apaga y D4 se enciende. D4 solo se enciende si el programa leyó `WDRF` en `MCUSR` al arrancar.
5. Registrar una captura o video que muestre los estados antes y después, y medir el intervalo entre el bloqueo y el cambio de D4. El período de 2 s es nominal; el tiempo simulado puede variar según el simulador.

Después del reset el firmware desactiva el WDT, apaga la salida D13 y queda en estado seguro con D4 encendido. Para repetir la prueba, reiniciar la placa desde PICSimLab. El WDT de esta prueba usa `WDE=1` y `WDIE=0`; no corresponde a un despertar por interrupción.

## Compilación

```text
avr-gcc -mmcu=atmega328p -DF_CPU=16000000UL -Os -std=gnu11 -Wall -Wextra -o wdt_reset.elf wdt_reset.c
avr-objcopy -O ihex -R .eeprom wdt_reset.elf wdt_reset.hex
```

**Estado de la evidencia:** el fuente compila y el proyecto contiene el mismo HEX. Falta ejecutar la secuencia y guardar la observación del reset en PICSimLab; todavía no se adjunta una medición experimental.
