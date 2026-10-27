
# Conexiones del proyecto – Arduino UNO

Documentación del conexionado lógico entre el Arduino UNO (ATmega328P), el sensor MPU6050, la matriz LED WS2812B 8×8, los pulsadores, el buzzer y el terminal serie.

## 1. Resumen de pines del Arduino

| Pin Arduino | Pin ATmega328P | Función |
|---|---|---|
| A4 | PC4 | SDA – Comunicación I2C |
| A5 | PC5 | SCL – Comunicación I2C |
| D1 | PD1 | TX – Comunicación serie |
| D2 | PD2 | Pulsador HOLD |
| D3 | PD3 | Pulsador CALIBRAR |
| D6 | PD6 | Datos matriz LED |
| D8 | PB0 | Control del buzzer |

**Frecuencia del Arduino:** 16 MHz.

## 2. Sensor MPU6050

Sensor acelerómetro y giroscopio con comunicación I2C.

| Pin MPU6050 | Conexión |
|---|---|
| VCC | Alimentación compatible con el módulo |
| GND | GND común |
| SCL | A5 |
| SDA | A4 |
| AD0 | GND |
| INT | Sin conectar |

**Configuración:**
- Dirección I2C: `0x68`.
- Velocidad I2C: `100 kHz`.
- Verificar si el módulo requiere adaptación de niveles lógicos entre 5 V y 3,3 V.

## 3. Matriz LED WS2812B 8×8

Matriz de 64 LEDs RGB direccionables individualmente.

| Pin matriz | Conexión |
|---|---|
| DIN | D6 |
| 5 V | Fuente regulada de 5 V |
| GND | GND común |

**Consideraciones:**
- Resistencia de `330 Ω` en serie con la señal de datos D6.
- Utilizar una fuente de 5 V adecuada al consumo de los 64 LEDs.
- Incorporar un capacitor de reserva entre 5 V y GND.
- Compartir GND entre la matriz, la fuente y el Arduino.

## 4. Pulsadores

Se utilizan dos pulsadores como entradas digitales.

| Pulsador | Pin Arduino | Conexión |
|---|---|---|
| HOLD | D2 | Pulsador entre D2 y GND |
| CALIBRAR | D3 | Pulsador entre D3 y GND |

**Configuración:**
- Resistencias `INPUT_PULLUP` internas activadas.
- Sin pulsar: estado `HIGH`.
- Pulsado: estado `LOW`.
- Antirrebote por software: `30 ms`.

## 5. Buzzer activo

Utilizado para generar avisos sonoros.

| Señal | Conexión |
|---|---|
| Control lógico | D8 |
| Alimentación | Según especificaciones del buzzer |
| GND | GND común |

**Importante:** Si el consumo del buzzer supera la capacidad admisible del pin, utilizar un transistor de control.

## 6. Terminal serie (UART)

Comunicación serie para monitoreo y depuración.

| Señal | Conexión |
|---|---|
| TX Arduino | D1 |
| RX terminal | D1 (TX Arduino) |

**Configuración:**

| Parámetro | Valor |
|---|---|
| Velocidad | 38400 baud |
| Bits de datos | 8 |
| Paridad | Ninguna |
| Bits de parada | 1 |
| Formato | 8N1 |

**Nota:** El pin D1 también es utilizado por la interfaz USB-serie del Arduino UNO.

## 7. Recomendaciones de montaje

- Todas las conexiones deben compartir una referencia GND común.
- Comprobar los niveles de tensión del MPU6050 antes de conectarlo.
- No alimentar los 64 LEDs de la matriz directamente desde un pin del Arduino.
- Utilizar una fuente de alimentación adecuada para la matriz.
- Verificar la polaridad y las conexiones antes de energizar el circuito.
