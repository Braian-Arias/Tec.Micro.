# Protocolo de tramas — Problema B

La comunicación usa UART0 a 9600 bit/s, 8N1, con el HC-05 como puente Bluetooth SPP. Cada mensaje contiene **cinco bytes binarios**, tanto desde la app hacia el microcontrolador como en sentido contrario:

`[0x7E][COMANDO][DATO][COMANDO XOR DATO][0x7F]`

`0x7E` inicia la trama; `0x7F` la termina. `COMANDO` y `DATO` ocupan un byte cada uno. El cuarto byte se calcula con XOR bit a bit. No se envían caracteres ASCII de los números hexadecimales. La trama no incluye longitud ni escape: si `COMANDO`, `DATO` o el XOR valen `0x7F`, el receptor debe contar los cinco bytes y no tratar ese valor intermedio como final anticipado.

| Solicitud (app → AVR) | Dato aceptado | Respuesta (AVR → app) |
| --- | --- | --- |
| `0x01` consultar estado | `0` | `0x81`, estado `0` ACTIVO, `1` STANDBY, `2` APAGADO |
| `0x02` consultar sensores | `0` | Dos tramas: `0x82`, ADC de 10 bits dividido por 4; `0xC2`, temperatura entera en °C |
| `0x03` cambiar estado | `0`, `1` o `2` | `0x83`, estado solicitado |
| `0x04` controlar LED | `0` apagar, `1` encender | `0x84`, valor aplicado |
| `0x05` emergencia | `1` | `0x85`, valor `1`; pasa a APAGADO |
| `0x06` consultar alarmas | `0` | `0x86`, contador de alarmas módulo 256 |

Los códigos de respuesta y error figuran en el campo `COMANDO` de una trama con el mismo formato. La app solo puede recuperar la emergencia mediante pulsación física, según el firmware actual. En APAGADO la UART se deshabilita, de modo que una solicitud enviada en ese estado no obtiene respuesta.

| Error (`COMANDO = 0x7D`) | Significado |
| --- | --- |
| Dato `1` | Byte final distinto de `0x7F` |
| Dato `2` | XOR incorrecto |
| Dato `3` | Desbordamiento del búfer de recepción |
| Dato `4` | Comando o dato no admitido |
| Dato `5` | Hay una emergencia activa; se rechaza el comando |
| Dato `6` | Trama incompleta: pasaron más de 100 ms desde el último byte |

La ISR de recepción deposita bytes en un búfer circular de 32 posiciones. El programa principal valida los delimitadores, el XOR y el comando. La respuesta se coloca en un búfer circular de transmisión y la ISR de UART la envía. El tiempo límite de 100 ms se aplica solo a una trama iniciada.

**Ejemplo:** consultar estado se envía como `7E 01 00 01 7F`. Si el sistema responde ACTIVO, devuelve `7E 81 00 81 7F`. Para encender el LED: `7E 04 01 05 7F`; la confirmación es `7E 84 01 85 7F`.

El protocolo se extrajo de `firmware/bluetooth.c`, `firmware/bluetooth.h` y `firmware/main.c`. Los valores de respuesta describen la implementación actual; su funcionamiento físico y temporal requiere las evidencias de comunicación y simulación correspondientes.
