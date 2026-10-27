/* PROBLEMA E - VERSION EN UN SOLO ARCHIVO PARA MICROCHIP STUDIO
 * ATmega328P, 16 MHz. Compilar con -Os y enlazar libm (-lm).
 * Archivo derivado del proyecto modular; no agregar ademas los .c de src/.
 * Mantiene I2C, matriz RGB, HOLD, calibracion EEPROM y buzzer.
 */

/* ==================== config.h ==================== */
#ifndef CONFIG_H
#define CONFIG_H
#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#define MPU_ADDR 0x68
#define UART_UBRR 25 /* 38400 baud, 16 MHz, modo normal */
#define PERIODO_MUESTRA_MS 20UL
#define MUESTRAS_FILTRO 8
#define SIGNO_X 1.0f
#define SIGNO_Y 1.0f
#define MATRIZ_SERPENTINA 0 /* PICSimLab usa filas lineales; panel fisico: verificar. */
#define BRILLO 24
#endif


/* ==================== UART.h ==================== */
#ifndef UART_H
#define UART_H
#include <stdint.h>
void uart_init(unsigned int ubrr);
void uart_send(char c);
void uart_print(const char *s);
void uart_entero(int16_t valor);
void uart_decimal(float valor);
#endif


/* ==================== I2C.h ==================== */
#ifndef I2C_H
#define I2C_H
#include <stdint.h>
void i2c_init(void);
uint8_t i2c_escribir_reg(uint8_t dir, uint8_t reg, uint8_t dato);
uint8_t i2c_leer_regs(uint8_t dir, uint8_t reg, uint8_t *datos, uint8_t n);
#endif


/* ==================== MPU6050.h ==================== */
#ifndef MPU6050_H
#define MPU6050_H
#include <stdint.h>
typedef struct {
    int16_t ax, ay, az, gx, gy, gz;
} Medicion;
uint8_t mpu_init(void);
uint8_t mpu_leer(Medicion *m);
uint8_t mpu_angulos(const Medicion *m, float *roll, float *pitch);
#endif


/* ==================== TIEMPO.h ==================== */
#ifndef TIEMPO_H
#define TIEMPO_H
#include <stdint.h>
void tiempo_init(void);
uint32_t tiempo_ms(void);
#endif


/* ==================== NIVEL.h ==================== */
#ifndef NIVEL_H
#define NIVEL_H
#include <stdint.h>
enum { AZUL, ROJO, VERDE, AMARILLO };
typedef struct { uint8_t fila, columna, color; } Burbuja;
void filtro_reiniciar(void);
void filtro_actualizar(float x, float y, float *fx, float *fy);
uint8_t nivel_coordenada(float angulo);
Burbuja nivel_burbuja(float x, float y);
uint8_t nivel_borde(Burbuja b);
#endif


/* ==================== MATRIZ.h ==================== */
#ifndef MATRIZ_H
#define MATRIZ_H
#include <stdint.h>

void matriz_init(void);
void matriz_mostrar(Burbuja b, uint8_t visible);
#endif


/* ==================== MEMORIA.h ==================== */
#ifndef MEMORIA_H
#define MEMORIA_H
#include <stdint.h>
uint8_t memoria_cargar(float *x, float *y);
void memoria_guardar(float x, float y);
#endif


/* ==================== BOTONES.h ==================== */
#ifndef BOTONES_H
#define BOTONES_H
#include <stdint.h>
#define EVENTO_HOLD 1
#define EVENTO_CAL 2
void botones_init(void);
uint8_t botones_eventos(uint32_t ahora);
#endif


/* ==================== UART.c ==================== */
/* Adaptado de UART.c proporcionado por el estudiante (material del curso). */
#include <avr/io.h>
#include <stdlib.h>

void uart_init(unsigned int ubrr) {
    UCSR0A = 0; /* U2X0=0: divisor 16 */
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}
void uart_send(char c) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}
void uart_print(const char *s) { while (*s) uart_send(*s++); }
void uart_entero(int16_t valor) {
    char texto[8];
    itoa(valor, texto, 10);
    uart_print(texto);
}
void uart_decimal(float valor) {
    /* Imprime una cifra decimal sin habilitar printf de punto flotante. */
    int16_t decimas = (int16_t)(valor * 10.0f + (valor >= 0 ? 0.5f : -0.5f));
    if (decimas < 0) { uart_send('-'); decimas = -decimas; }
    uart_entero(decimas / 10);
    uart_send('.');
    uart_send('0' + decimas % 10);
}


/* ==================== I2C.c ==================== */

#include <avr/io.h>
#include <util/delay.h>

/* Igual esquema TWBR/TWCR/TWDR de clase; se agregan timeout y ACK. */
void i2c_init(void) {
    DDRC &= ~((1 << PC4) | (1 << PC5));
    PORTC &= ~((1 << PC4) | (1 << PC5)); /* pull-up externos */
    TWSR = 0;
    TWBR = 72; /* 16 MHz / (16+2*72) = 100 kHz */
    TWCR = (1 << TWEN);
}
static uint8_t esperar(void) {
    uint16_t intentos = 2000;
    while (!(TWCR & (1 << TWINT))) {
        if (--intentos == 0) return 0;
        _delay_us(1);
    }
    return 1;
}
static uint8_t iniciar(void) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    if (!esperar()) return 0;
    uint8_t estado = TWSR & 0xF8;
    return estado == 0x08 || estado == 0x10;
}
static uint8_t enviar(uint8_t dato, uint8_t esperado) {
    TWDR = dato;
    TWCR = (1 << TWINT) | (1 << TWEN);
    return esperar() && (TWSR & 0xF8) == esperado;
}
static void parar(void) {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    uint16_t intentos = 2000;
    while ((TWCR & (1 << TWSTO)) && --intentos) _delay_us(1);
    if (!intentos) { TWCR = 0; i2c_init(); }
}
uint8_t i2c_escribir_reg(uint8_t dir, uint8_t reg, uint8_t dato) {
    uint8_t ok = iniciar() && enviar(dir << 1, 0x18)
        && enviar(reg, 0x28) && enviar(dato, 0x28);
    parar();
    return ok;
}
uint8_t i2c_leer_regs(uint8_t dir, uint8_t reg, uint8_t *datos, uint8_t n) {
    if (!n) return 0;
    if (!iniciar() || !enviar(dir << 1, 0x18) || !enviar(reg, 0x28)
        || !iniciar() || !enviar((dir << 1) | 1, 0x40)) {
        parar(); return 0;
    }
    for (uint8_t i = 0; i < n; i++) {
        uint8_t ack = i < n - 1;
        TWCR = (1 << TWINT) | (1 << TWEN) | (ack ? (1 << TWEA) : 0);
        if (!esperar() || (TWSR & 0xF8) != (ack ? 0x50 : 0x58)) {
            parar(); return 0;
        }
        datos[i] = TWDR;
    }
    parar();
    return 1;
}


/* ==================== MPU6050.c ==================== */

#include <math.h>
#include <util/delay.h>


uint8_t mpu_init(void) {
    uint8_t identidad;
    if (!i2c_leer_regs(MPU_ADDR, 0x75, &identidad, 1) || identidad != 0x68) return 0;
    if (!i2c_escribir_reg(MPU_ADDR, 0x6B, 0x01)) return 0; /* despertar, PLL X */
    _delay_ms(50);
    return i2c_escribir_reg(MPU_ADDR, 0x1A, 0x03) /* filtro interno ~44 Hz accel */
        && i2c_escribir_reg(MPU_ADDR, 0x19, 19)  /* 1 kHz/(19+1)=50 Hz */
        && i2c_escribir_reg(MPU_ADDR, 0x1B, 0)   /* gyro +/-250 grados/s */
        && i2c_escribir_reg(MPU_ADDR, 0x1C, 0);  /* accel +/-2 g */
}
static int16_t unir(uint8_t alto, uint8_t bajo) {
    return (int16_t)(((uint16_t)alto << 8) | bajo);
}
uint8_t mpu_leer(Medicion *m) {
    uint8_t b[14];
    if (!i2c_leer_regs(MPU_ADDR, 0x3B, b, 14)) return 0;
    m->ax = unir(b[0], b[1]); m->ay = unir(b[2], b[3]); m->az = unir(b[4], b[5]);
    /* b[6..7] es temperatura; b[8..13] son velocidades angulares. */
    m->gx = unir(b[8], b[9]); m->gy = unir(b[10], b[11]); m->gz = unir(b[12], b[13]);
    return 1;
}
uint8_t mpu_angulos(const Medicion *m, float *roll, float *pitch) {
    float ax = m->ax, ay = m->ay, az = m->az;
    if (ax * ax + ay * ay + az * az < 100.0f) return 0;
    *roll = atan2f(ay, az) * 57.2957795f;
    *pitch = atan2f(-ax, sqrtf(ay * ay + az * az)) * 57.2957795f;
    return 1;
}


/* ==================== TIEMPO.c ==================== */
#include <avr/io.h>

static uint16_t tiempo_anterior, resto;
static uint32_t milisegundos;
void tiempo_init(void) {
    TCCR1A = 0;
    TCCR1B = (1 << CS11) | (1 << CS10); /* Normal, prescaler 64 */
    TCNT1 = 0;
    tiempo_anterior = resto = 0;
    milisegundos = 0;
}
uint32_t tiempo_ms(void) {
    /* Timer1 avanza cada 4 us. Debe consultarse antes de 262 ms.
       Sigue contando mientras enviamos WS2812 con interrupciones apagadas. */
    uint16_t actual = TCNT1;
    uint16_t transcurrido = (uint16_t)(actual - tiempo_anterior);
    uint32_t cuentas = (uint32_t)resto + transcurrido;
    tiempo_anterior = actual;
    milisegundos += cuentas / 250;
    resto = cuentas % 250;
    return milisegundos;
}


/* ==================== NIVEL.c ==================== */

#include <math.h>

static float datos_x[MUESTRAS_FILTRO], datos_y[MUESTRAS_FILTRO];
static float suma_x, suma_y;
static uint8_t indice, cantidad;
void filtro_reiniciar(void) {
    suma_x = suma_y = 0;
    indice = cantidad = 0;
    for (uint8_t i = 0; i < MUESTRAS_FILTRO; i++) datos_x[i] = datos_y[i] = 0;
}
void filtro_actualizar(float x, float y, float *fx, float *fy) {
    suma_x -= datos_x[indice]; suma_y -= datos_y[indice];
    datos_x[indice] = x; datos_y[indice] = y;
    suma_x += x; suma_y += y;
    indice = (indice + 1) % MUESTRAS_FILTRO;
    if (cantidad < MUESTRAS_FILTRO) cantidad++;
    *fx = suma_x / cantidad;
    *fy = suma_y / cantidad;
}
uint8_t nivel_coordenada(float angulo) {
    /* Zona muerta por eje. Centro elegido: (4,4), indices desde cero.
       Fuera de ella: interpolacion lineal y redondeo al LED mas cercano. */
    if (fabsf(angulo) <= 3.0f) return 4;
    if (angulo <= -30.0f) return 0;
    if (angulo >= 30.0f) return 7;
    return (uint8_t)(((angulo + 30.0f) * 7.0f / 60.0f) + 0.5f);
}
Burbuja nivel_burbuja(float x, float y) {
    Burbuja b;
    float ax = fabsf(x), ay = fabsf(y);
    b.columna = nivel_coordenada(x);
    b.fila = nivel_coordenada(y);
    if (ax <= 3 && ay <= 3) b.color = AZUL;
    /* Combinada: ambos fuera de zona muerta y magnitudes similares.
       Si un eje supera al otro por mas de 2:1, ese eje predomina. */
    else if (ax > 3 && ay > 3 && ax <= 2 * ay && ay <= 2 * ax) b.color = AMARILLO;
    else if (ax > ay) b.color = ROJO;
    else b.color = VERDE;
    return b;
}
uint8_t nivel_borde(Burbuja b) {
    return b.fila == 0 || b.fila == 7 || b.columna == 0 || b.columna == 7;
}


/* ==================== MATRIZ.c ==================== */
/* WS2812B: 64 pixeles, GRB, MSB primero, salida PD6 (D6).
   C para preparar la imagen; ASM solo para el tiempo de cada bit,
   como se explica en la presentacion 12 del curso. */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

#if F_CPU != 16000000UL
#error "La rutina WS2812 esta calculada exclusivamente para 16 MHz"
#endif
static uint8_t pixels[64 * 3]; /* orden GRB */
static void transmitir(void) {
    uint8_t sreg = SREG;
    cli();
    uint8_t alto = PORTD | (1 << PD6);
    uint8_t bajo = PORTD & ~(1 << PD6);
    uint8_t *p = pixels, dato;
    uint16_t n = sizeof(pixels);
    /* Cada bit: 20 ciclos = 1.25 us; T0H=6 ciclos, T1H=12 ciclos.
       Los siete primeros bits usan el mismo bloque. El ultimo aprovecha
       el tiempo bajo para cargar el siguiente byte sin romper el periodo. */
    __asm__ volatile (
        "ld %[dato], %a[p]+\n\t"
        "1:\n\t"
        ".rept 7\n\t"
        "out %[port], %[alto]\n\t"
        ".rept 4\n\tnop\n\t.endr\n\t"
        "sbrs %[dato], 7\n\t"
        "out %[port], %[bajo]\n\t"
        ".rept 5\n\tnop\n\t.endr\n\t"
        "out %[port], %[bajo]\n\t"
        "lsl %[dato]\n\t"
        ".rept 6\n\tnop\n\t.endr\n\t"
        ".endr\n\t"
        "out %[port], %[alto]\n\t"
        ".rept 4\n\tnop\n\t.endr\n\t"
        "sbrs %[dato], 7\n\t"
        "out %[port], %[bajo]\n\t"
        ".rept 5\n\tnop\n\t.endr\n\t"
        "out %[port], %[bajo]\n\t"
        "sbiw %[n], 1\n\t"
        "breq 2f\n\t"
        "ld %[dato], %a[p]+\n\t"
        "rjmp 1b\n\t"
        "2:\n\t"
        : [p] "+&e" (p), [n] "+&w" (n), [dato] "=&r" (dato)
        : [port] "I" (_SFR_IO_ADDR(PORTD)), [alto] "r" (alto), [bajo] "r" (bajo)
        : "memory", "cc"
    );
    SREG = sreg;
    _delay_us(300); /* Latch/reset, tambien cubre revisiones de reset largo. */
}
void matriz_init(void) {
    DDRD |= (1 << PD6);
    PORTD &= ~(1 << PD6);
    memset(pixels, 0, sizeof(pixels));
    transmitir();
}
void matriz_mostrar(Burbuja b, uint8_t visible) {
    memset(pixels, 0, sizeof(pixels));
    if (visible && b.fila < 8 && b.columna < 8) {
        uint8_t col = b.columna;
        if (MATRIZ_SERPENTINA && (b.fila & 1)) col = 7 - col;
        uint8_t indice = (b.fila * 8 + col) * 3;
        if (b.color == VERDE || b.color == AMARILLO) pixels[indice] = BRILLO;
        if (b.color == ROJO || b.color == AMARILLO) pixels[indice + 1] = BRILLO;
        if (b.color == AZUL) pixels[indice + 2] = BRILLO;
    }
    transmitir();
}



/* ==================== MEMORIA.c ==================== */
#include <avr/io.h>
#include <avr/interrupt.h>

/* EEPROM 0: firma; 1..2: offset roll; 3..4: offset pitch; 5: checksum.
   Offsets int16 en centesimas de grado. Escritura solo al calibrar.
   Firma invalidada antes del cambio, validada al finalizar. */
static uint8_t leer(uint16_t direccion) {
    while (EECR & (1 << EEPE));
    EEAR = direccion;
    EECR |= (1 << EERE);
    return EEDR;
}
static void escribir(uint16_t direccion, uint8_t dato) {
    if (leer(direccion) == dato) return;
    while (EECR & (1 << EEPE));
    EEAR = direccion;
    EEDR = dato;
    EECR &= ~((1 << EEPM1) | (1 << EEPM0));
    uint8_t sreg = SREG;
    cli();
    EECR |= (1 << EEMPE);
    EECR |= (1 << EEPE);
    SREG = sreg;
    while (EECR & (1 << EEPE));
}
uint8_t memoria_cargar(float *x, float *y) {
    *x = *y = 0;
    if (leer(0) != 0xA5) return 0;
    uint8_t b[4], check = 0x5A;
    for (uint8_t i = 0; i < 4; i++) { b[i] = leer(i + 1); check ^= b[i]; }
    if (check != leer(5)) return 0;
    int16_t ox = (int16_t)((uint16_t)b[0] | ((uint16_t)b[1] << 8));
    int16_t oy = (int16_t)((uint16_t)b[2] | ((uint16_t)b[3] << 8));
    if (ox < -18000 || ox > 18000 || oy < -9000 || oy > 9000) return 0;
    *x = ox / 100.0f; *y = oy / 100.0f;
    return 1;
}
void memoria_guardar(float x, float y) {
    int16_t ox = (int16_t)(x * 100.0f + (x >= 0 ? 0.5f : -0.5f));
    int16_t oy = (int16_t)(y * 100.0f + (y >= 0 ? 0.5f : -0.5f));
    uint8_t b[4] = {(uint8_t)ox, (uint8_t)((uint16_t)ox >> 8),
                    (uint8_t)oy, (uint8_t)((uint16_t)oy >> 8)};
    uint8_t check = 0x5A;
    escribir(0, 0); /* Un corte no deja una calibracion incompleta como valida. */
    for (uint8_t i = 0; i < 4; i++) { escribir(i + 1, b[i]); check ^= b[i]; }
    escribir(5, check);
    escribir(0, 0xA5);
}


/* ==================== BOTONES.c ==================== */
#include <avr/io.h>

static uint8_t boton_anterior[2] = {1, 1}, estable[2] = {1, 1};
static uint32_t cambio[2];
void botones_init(void) {
    DDRD &= ~((1 << PD2) | (1 << PD3));
    PORTD |= (1 << PD2) | (1 << PD3); /* pulsador entre pin y GND */
}
uint8_t botones_eventos(uint32_t ahora) {
    uint8_t eventos = 0;
    for (uint8_t i = 0; i < 2; i++) {
        uint8_t lectura = !!(PIND & (1 << (PD2 + i)));
        if (lectura != boton_anterior[i]) { boton_anterior[i] = lectura; cambio[i] = ahora; }
        if (lectura != estable[i] && (uint32_t)(ahora - cambio[i]) >= 30) {
            estable[i] = lectura;
            if (!lectura) eventos |= (1 << i); /* un evento al apretar */
        }
    }
    return eventos;
}


/* ==================== main.c ==================== */

#include <avr/io.h>
#include <math.h>








/* Cinco estados explicitos: coinciden con diagramas/maquina_estados.png. */
enum { INICIO, MEDIR, HOLD, CALIBRAR, ERROR_SENSOR };
static uint8_t estado = INICIO;
static Burbuja burbuja = {4, 4, AZUL};
static float offset_x, offset_y, x, y, fx, fy;
static uint8_t en_borde, visible = 1, pitando;
static uint32_t tiempo_pitido, tiempo_parpadeo;
static uint8_t n_cal;
static float suma_cal_x, suma_cal_y, min_x, max_x, min_y, max_y;

static void apagar_buzzer(void) { PORTB &= ~(1 << PB0); pitando = 0; }
static void comenzar_calibracion(void) {
    estado = CALIBRAR;
    n_cal = 0; suma_cal_x = suma_cal_y = 0;
    apagar_buzzer();
    matriz_mostrar(burbuja, 1);
    uart_print("CAL: mantener inmovil 0.64 s\r\n");
}
static void muestra_calibracion(float roll, float pitch) {
    if (!n_cal) { min_x = max_x = roll; min_y = max_y = pitch; }
    if (roll < min_x) min_x = roll;
    if (roll > max_x) max_x = roll;
    if (pitch < min_y) min_y = pitch;
    if (pitch > max_y) max_y = pitch;
    suma_cal_x += roll; suma_cal_y += pitch; n_cal++;
    if (n_cal == 32) {
        if (max_x - min_x <= 1.5f && max_y - min_y <= 1.5f) {
            offset_x = suma_cal_x / 32; offset_y = suma_cal_y / 32;
            memoria_guardar(offset_x, offset_y);
            memoria_cargar(&offset_x, &offset_y);
            uart_print("CAL: guardada en EEPROM\r\n");
        } else uart_print("CAL: movimiento; repetir\r\n");
        filtro_reiniciar(); en_borde = 0; estado = MEDIR;
    }
}
static void actualizar_burbuja(uint32_t ahora) {
    burbuja = nivel_burbuja(fx, fy);
    uint8_t nuevo_borde = nivel_borde(burbuja);
    if (nuevo_borde && !en_borde) {
        PORTB |= (1 << PB0); pitando = 1; tiempo_pitido = ahora;
        visible = 1; tiempo_parpadeo = ahora;
    }
    en_borde = nuevo_borde;
    if (!en_borde) visible = 1;
    matriz_mostrar(burbuja, visible);
}
static void informar(void) {
    uart_print("R="); uart_decimal(x);
    uart_print(" P="); uart_decimal(y);
    uart_print(" F="); uart_entero(burbuja.fila);
    uart_print(" C="); uart_entero(burbuja.columna);
    uart_print(" E="); uart_entero(estado);
    uart_print(" COLOR="); uart_entero(burbuja.color);
    uart_print("\r\n");
}
int main(void) {
    Medicion m;
    uint32_t muestra = 0, reporte = 0, reintento = 0;
    float roll, pitch;
    uart_init(UART_UBRR); i2c_init(); botones_init(); matriz_init();
    DDRB |= (1 << PB0); apagar_buzzer();
    memoria_cargar(&offset_x, &offset_y);
    filtro_reiniciar(); tiempo_init();
    uart_print("BURBUJA E: R=roll, P=pitch; 38400 8N1\r\n");
    while (1) {
        uint32_t ahora = tiempo_ms();
        uint8_t eventos = botones_eventos(ahora);
        uint8_t nueva = 0;

        /* La medicion sigue activa en HOLD, pero la imagen no se modifica. */
        if (estado != INICIO && estado != ERROR_SENSOR &&
            (uint32_t)(ahora - muestra) >= PERIODO_MUESTRA_MS) {
            muestra = ahora;
            if (!mpu_leer(&m) || !mpu_angulos(&m, &roll, &pitch)) {
                estado = ERROR_SENSOR; reintento = ahora;
                apagar_buzzer(); uart_print("ERROR: sensor sin lectura valida\r\n");
            } else {
                x = SIGNO_X * (roll - offset_x);
                y = SIGNO_Y * (pitch - offset_y);
                /* En el plano de trabajo se usa la cara superior hacia arriba.
                   Normalizar roll evita saltos al cruzar +/-180 grados. */
                if (x > 180) x -= 360;
                if (x < -180) x += 360;
                filtro_actualizar(x, y, &fx, &fy);
                nueva = 1;
            }
        }

        switch (estado) {
        case INICIO:
            if (mpu_init()) {
                filtro_reiniciar(); en_borde = 0; visible = 1;
                estado = MEDIR;
                uart_print("MPU6050 OK\r\n");
            } else {
                estado = ERROR_SENSOR; reintento = ahora;
                uart_print("ERROR: revisar MPU6050/I2C\r\n");
            }
            break;
        case MEDIR:
            if (eventos & EVENTO_CAL) comenzar_calibracion();
            else if (eventos & EVENTO_HOLD) {
                estado = HOLD; apagar_buzzer(); matriz_mostrar(burbuja, 1);
            } else if (nueva) actualizar_burbuja(ahora);
            break;
        case HOLD:
            if (eventos & EVENTO_CAL) comenzar_calibracion();
            else if (eventos & EVENTO_HOLD) {
                estado = MEDIR; en_borde = 0;
                actualizar_burbuja(ahora);
            }
            break;
        case CALIBRAR:
            if (nueva) muestra_calibracion(roll, pitch);
            break;
        case ERROR_SENSOR:
            /* Un fallo no se presenta como una medicion valida. */
            burbuja.fila = burbuja.columna = 4; burbuja.color = ROJO;
            if ((uint32_t)(ahora - tiempo_parpadeo) >= 250) {
                tiempo_parpadeo = ahora; visible = !visible;
                matriz_mostrar(burbuja, visible);
            }
            if ((uint32_t)(ahora - reintento) >= 1000) estado = INICIO;
            break;
        }

        if (pitando && (uint32_t)(ahora - tiempo_pitido) >= 100) apagar_buzzer();
        if (estado == MEDIR && en_borde &&
            (uint32_t)(ahora - tiempo_parpadeo) >= 250) {
            tiempo_parpadeo = ahora; visible = !visible;
            matriz_mostrar(burbuja, visible);
        }
        if (estado != ERROR_SENSOR && estado != INICIO &&
            (uint32_t)(ahora - reporte) >= 250) {
            reporte = ahora; informar();
        }
    }
}
