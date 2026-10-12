// Problema C: clasificador de color de plasticos

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>
#include <util/delay.h>

#define BAUDIOS 9600UL
#define VALOR_UART ((F_CPU / (16UL * BAUDIOS)) - 1UL)

#define DIR_LCD 0x27
#define LUZ_LCD 0x08
#define E_LCD 0x04
#define RS_LCD 0x01

#define PIN_WS2812 PD7
#define LIMITE 600U

#define MODO_SIMULACION 1

// Guardo los datos de cada color en una tabla.
typedef struct {
	const char *nombre;
	uint16_t rojo;
	uint16_t verde;
	uint16_t azul;
	uint8_t angulo;
	uint8_t led_rojo;
	uint8_t led_verde;
	uint8_t led_azul;
} Color;

typedef struct {
	uint16_t rojo;
	uint16_t verde;
	uint16_t azul;
} Lectura;

// Valores de prueba: rojo, verde, azul, angulo y color del LED.
static const Color colores[] = {
	{"ROJO",     800, 220, 180,  20, 120,   0,   0},
	{"VERDE",    220, 800, 240,  65,   0, 120,   0},
	{"AZUL",     180, 250, 800, 115,   0,   0, 120},
	{"AMARILLO", 780, 720, 180, 160, 100,  80,   0}
};

#define NUM_COLORES ((uint8_t)(sizeof(colores) / sizeof(colores[0])))

static uint8_t lcd_ok = 0;

// UART

static void iniciar_uart(void)
{
	UBRR0H = (uint8_t)(VALOR_UART >> 8);
	UBRR0L = (uint8_t)VALOR_UART;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

static void enviar_letra(char caracter)
{
	while (!(UCSR0A & (1 << UDRE0))) {
		// Esperar a que el registro quede libre.
	}

	UDR0 = caracter;
}

static void enviar_texto(const char *texto)
{
	while (*texto != '\0') {
		enviar_letra(*texto);
		texto++;
	}
}

static void enviar_numero(uint16_t numero)
{
	char texto[6];
	utoa(numero, texto, 10);
	enviar_texto(texto);
}

// ADC Y FOTOCELDA

static void iniciar_adc(void)
{
	DDRC &= ~((1 << DDC0) | (1 << DDC1) | (1 << DDC2));
	DIDR0 |= (1 << ADC0D) | (1 << ADC1D) | (1 << ADC2D);
	ADMUX = (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

static uint16_t leer_adc(uint8_t canal)
{
	ADMUX = (uint8_t)((ADMUX & 0xF0) | (canal & 0x0F));
	_delay_us(50);
	ADCSRA |= (1 << ADSC);

	while (ADCSRA & (1 << ADSC)) {
		// Esperar a que termine la conversion.
	}

	return ADC;
}

static uint16_t leer_promedio(uint8_t canal)
{
	uint8_t muestra;
	uint32_t suma = 0;

	// Promedio diez lecturas para reducir las variaciones.
	for (muestra = 0; muestra < 10; muestra++) {
		suma += leer_adc(canal);
		_delay_ms(2);
	}

	return (uint16_t)(suma / 10UL);
}

// I2C Y LCD

static void iniciar_i2c(void)
{
	TWSR = 0;
	TWBR = 72;
	TWCR = (1 << TWEN);
}

static uint8_t esperar_i2c(void)
{
	uint16_t intentos = 60000;

	while (!(TWCR & (1 << TWINT)) && intentos > 0) {
		intentos--;
	}

	return intentos > 0;
}

static uint8_t enviar_i2c(uint8_t direccion, uint8_t dato)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	if (!esperar_i2c()) {
		return 0;
	}

	TWDR = (uint8_t)(direccion << 1);
	TWCR = (1 << TWINT) | (1 << TWEN);
	if (!esperar_i2c()) {
		return 0;
	}

	if ((TWSR & 0xF8) != 0x18) {
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
		return 0;
	}

	TWDR = dato;
	TWCR = (1 << TWINT) | (1 << TWEN);
	if (!esperar_i2c()) {
		return 0;
	}

	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	return 1;
}

static uint8_t enviar_pcf(uint8_t dato)
{
	return enviar_i2c(DIR_LCD, (uint8_t)(dato | LUZ_LCD));
}

static void pulso_lcd(uint8_t dato)
{
	enviar_pcf((uint8_t)(dato | E_LCD));
	_delay_us(1);
	enviar_pcf((uint8_t)(dato & ~E_LCD));
	_delay_us(50);
}

static void lcd_4bits(uint8_t dato, uint8_t es_texto)
{
	uint8_t salida = (uint8_t)((dato & 0x0F) << 4);

	if (es_texto) {
		salida |= RS_LCD;
	}

	enviar_pcf(salida);
	pulso_lcd(salida);
}

static void enviar_lcd(uint8_t dato, uint8_t es_texto)
{
	if (!lcd_ok) {
		return;
	}

	lcd_4bits((uint8_t)(dato >> 4), es_texto);
	lcd_4bits((uint8_t)(dato & 0x0F), es_texto);
}

static void lcd_comando(uint8_t comando)
{
	enviar_lcd(comando, 0);
}

static void lcd_caracter(char caracter)
{
	enviar_lcd((uint8_t)caracter, 1);
}

static void iniciar_lcd(void)
{
	_delay_ms(50);
	lcd_ok = enviar_pcf(0);

	if (!lcd_ok) {
		return;
	}

	lcd_4bits(0x03, 0);
	_delay_ms(5);
	lcd_4bits(0x03, 0);
	_delay_us(150);
	lcd_4bits(0x03, 0);
	lcd_4bits(0x02, 0);

	lcd_comando(0x28);
	lcd_comando(0x0C);
	lcd_comando(0x06);
	lcd_comando(0x01);
	_delay_ms(2);
}

static void lcd_posicion(uint8_t fila, uint8_t columna)
{
	uint8_t posicion = (fila == 0) ? columna : (uint8_t)(0x40 + columna);
	lcd_comando((uint8_t)(0x80 | posicion));
}

static void lcd_texto(const char *texto)
{
	while (*texto != '\0') {
		lcd_caracter(*texto);
		texto++;
	}
}

static void lcd_numero(uint16_t numero)
{
	char texto[6];
	utoa(numero, texto, 10);
	lcd_texto(texto);
}

static void lcd_limpiar(void)
{
	lcd_comando(0x01);
	_delay_ms(2);
}

// WS2812

static inline __attribute__((always_inline)) void enviar_bit(uint8_t valor)
{
	PORTD |= (1 << PIN_WS2812);

	if (valor) {
		_delay_us(0.70);
		PORTD &= ~(1 << PIN_WS2812);
		_delay_us(0.55);
	} else {
		_delay_us(0.35);
		PORTD &= ~(1 << PIN_WS2812);
		_delay_us(0.90);
	}
}

static inline __attribute__((always_inline)) void enviar_byte(uint8_t dato)
{
	uint8_t bit;

	for (bit = 0; bit < 8; bit++) {
		enviar_bit((dato & 0x80) != 0);
		dato <<= 1;
	}
}

static void encender_led(uint8_t rojo, uint8_t verde, uint8_t azul)
{
	uint8_t estado_anterior = SREG;

	cli();
	// El WS2812 recibe los datos en el orden verde, rojo y azul.
	enviar_byte(verde);
	enviar_byte(rojo);
	enviar_byte(azul);
	SREG = estado_anterior;

	_delay_us(80);
}

static void iniciar_led(void)
{
	DDRD |= (1 << DDD7);
	PORTD &= ~(1 << PIN_WS2812);
	encender_led(0, 0, 0);
}

// SERVOMOTOR

static void iniciar_servo(void)
{
	DDRB |= (1 << DDB1); // D9, salida OC1A

	// PWM de 50 Hz: 16 MHz / 8 / (39999 + 1) = 50 Hz.
	TCCR1A = (1 << COM1A1) | (1 << WGM11);
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);
	ICR1 = 39999;
	OCR1A = 3000;
}

static void mover_servo(uint8_t angulo)
{
	uint16_t pulso;

	if (angulo > 180) {
		angulo = 180;
	}

	// 0 grados = 1 ms y 180 grados = 2 ms.
	pulso = (uint16_t)(2000UL + ((uint32_t)angulo * 2000UL) / 180UL);
	OCR1A = pulso;
}

// MEDICION Y CLASIFICACION

static Lectura medir(void)
{
	Lectura lectura;

	encender_led(100, 0, 0);
	_delay_ms(150);
	lectura.rojo = leer_promedio(0);

	encender_led(0, 100, 0);
	_delay_ms(150);
#if MODO_SIMULACION
	lectura.verde = leer_promedio(1);
#else
	lectura.verde = leer_promedio(0);
#endif

	encender_led(0, 0, 100);
	_delay_ms(150);
#if MODO_SIMULACION
	lectura.azul = leer_promedio(2);
#else
	lectura.azul = leer_promedio(0);
#endif

	encender_led(0, 0, 0);
	return lectura;
}

static uint16_t restar(uint16_t a, uint16_t b)
{
	// Resto el menor al mayor para que la diferencia sea positiva.
	if (a >= b) {
		return a - b;
	}
	return b - a;
}

static uint16_t comparar(Lectura lectura,
	const Color *patron)
{
	return (uint16_t)(
		restar(lectura.rojo, patron->rojo) +
		restar(lectura.verde, patron->verde) +
		restar(lectura.azul, patron->azul));
}

static int8_t buscar_color(Lectura lectura, uint16_t *error)
{
	uint8_t color;
	uint8_t elegido = 0;
	uint16_t diferencia;
	uint16_t menor = 65535U;

	// Comparo con los cuatro colores y guardo el que mas se parece.
	for (color = 0; color < NUM_COLORES; color++) {
		diferencia = comparar(lectura, &colores[color]);

		if (diferencia < menor) {
			menor = diferencia;
			elegido = color;
		}
	}

	// Devuelvo tambien la diferencia para mostrarla en el LCD y por UART.
	*error = menor;

	if (menor > LIMITE) {
		return -1;
	}

	return (int8_t)elegido;
}

static void mostrar_lcd(Lectura lectura, int8_t color,
	uint16_t diferencia)
{
	// Primera pantalla: muestro las tres lecturas de la fotocelda.
	lcd_limpiar();
	lcd_posicion(0, 0);
	lcd_texto("R:");
	lcd_numero(lectura.rojo);
	lcd_texto(" G:");
	lcd_numero(lectura.verde);
	lcd_posicion(1, 0);
	lcd_texto("B:");
	lcd_numero(lectura.azul);
	_delay_ms(1200);

	// Segunda pantalla: muestro el color y el angulo del servo.
	lcd_limpiar();
	lcd_posicion(0, 0);
	if (color < 0) {
		lcd_texto("COLOR:NO DEFIN.");
		lcd_posicion(1, 0);
		lcd_texto("ANGULO:90");
	} else {
		lcd_posicion(0, 0);
		lcd_texto("COLOR:");
		lcd_texto(colores[(uint8_t)color].nombre);
		lcd_posicion(1, 0);
		lcd_texto("ANGULO:");
		lcd_numero(colores[(uint8_t)color].angulo);
	}
	_delay_ms(1200);

	// Tercera pantalla: muestro la patron elegida y la diferencia.
	lcd_limpiar();
	lcd_posicion(0, 0);
	if (color < 0) {
		lcd_texto("REF:---");
	} else {
		lcd_texto("REF:");
		lcd_numero(colores[(uint8_t)color].rojo);
		lcd_caracter(',');
		lcd_numero(colores[(uint8_t)color].verde);
		lcd_caracter(',');
		lcd_numero(colores[(uint8_t)color].azul);
	}
	lcd_posicion(1, 0);
	lcd_texto("DIF:");
	lcd_numero(diferencia);
	_delay_ms(1200);
}

static void mostrar_uart(Lectura lectura, int8_t color,
	uint16_t diferencia)
{
	enviar_texto("R=");
	enviar_numero(lectura.rojo);
	enviar_texto(" | G=");
	enviar_numero(lectura.verde);
	enviar_texto(" | B=");
	enviar_numero(lectura.azul);
	enviar_texto(" | COLOR=");

	if (color < 0) {
		enviar_texto("DESCONOCIDO | REF=--- | DIF=");
		enviar_numero(diferencia);
		enviar_texto(" | ANGULO=90\r\n");
		return;
	}

	enviar_texto(colores[(uint8_t)color].nombre);
	enviar_texto(" | REF=");
	enviar_numero(colores[(uint8_t)color].rojo);
	enviar_letra(',');
	enviar_numero(colores[(uint8_t)color].verde);
	enviar_letra(',');
	enviar_numero(colores[(uint8_t)color].azul);
	enviar_texto(" | DIF=");
	enviar_numero(diferencia);
	enviar_texto(" | ANGULO=");
	enviar_numero(colores[(uint8_t)color].angulo);
	enviar_texto("\r\n");
}

static void mover_y_encender(int8_t color)
{
	if (color < 0) {
		mover_servo(90);
		encender_led(40, 40, 40);
		return;
	}

	mover_servo(colores[(uint8_t)color].angulo);
	encender_led(
		colores[(uint8_t)color].led_rojo,
		colores[(uint8_t)color].led_verde,
		colores[(uint8_t)color].led_azul);
}

int main(void)
{
	Lectura lectura;
	uint16_t diferencia;
	int8_t color;

	iniciar_uart();
	iniciar_adc();
	iniciar_i2c();
	iniciar_lcd();
	iniciar_led();
	iniciar_servo();
	mover_servo(90);

	enviar_texto("Clasificador de color iniciado.\r\n");
#if MODO_SIMULACION
	enviar_texto("Modo simulacion: A0=rojo, A1=verde, A2=azul.\r\n");
#else
	enviar_texto("Modo fisico: fotocelda conectada en A0.\r\n");
#endif

	while (1) {
		lectura = medir();
		color = buscar_color(lectura, &diferencia);
		mover_y_encender(color);
		mostrar_lcd(lectura, color, diferencia);
		mostrar_uart(lectura, color, diferencia);
		_delay_ms(2000);
	}
}
