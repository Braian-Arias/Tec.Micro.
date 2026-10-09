//Problema B. Sensor de temperatura (LM35)

#define F_CPU 16000000UL

#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <util/delay.h>

#define VELOCIDAD_UART 9600UL
#define VALOR_UBRR ((F_CPU / (16UL * VELOCIDAD_UART)) - 1UL)

#define DIRECCION_LCD 0x27
#define BIT_LUZ_LCD 0x08
#define BIT_ENABLE_LCD 0x04
#define BIT_RS_LCD 0x01

#define PUNTO_MEDIO_INICIAL 22
#define DIRECCION_EEPROM_PM 0

#define PWM_BAJO 89
#define PWM_MEDIO 166
#define PWM_ALTO 255

typedef enum {
	ESTADO_CALENTAR,
	ESTADO_ESTABLE,
	ESTADO_VENTILAR_BAJO,
	ESTADO_VENTILAR_MEDIO,
	ESTADO_VENTILAR_ALTO,
	ESTADO_ERROR_SENSOR
} estado_sistema_t;

static uint8_t lcd_disponible = 0;

/* ---------------- ADC Y LM35 ---------------- */

static void adc_iniciar(void)
{
	DDRC &= ~(1 << DDC0);       // A0 como entrada
	DIDR0 |= (1 << ADC0D);      // Apagar la entrada digital de A0
	ADMUX = (1 << REFS0);       // Referencia AVCC (5 V), canal ADC0
	ADCSRA = (1 << ADEN) |      // Encender ADC
	(1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Division 128
}

static uint16_t adc_leer(void)
{
	ADCSRA |= (1 << ADSC);

	while (ADCSRA & (1 << ADSC)) {
		// Esperar a que termine la lectura
	}

	return ADC;
}

static uint16_t leer_temperatura(void)
{
	uint8_t muestra;
	uint32_t suma = 0;
	uint16_t lectura_promedio;

	// Promediar diez lecturas para que el valor sea mas estable.
	for (muestra = 0; muestra < 10; muestra++) {
		suma += adc_leer();
		_delay_ms(2);
	}

	lectura_promedio = (uint16_t)(suma / 10UL);

	// LM35: 10 mV por grado. La referencia del ADC es 5000 mV.
	return (uint16_t)(((uint32_t)lectura_promedio * 500UL + 511UL) / 1023UL);
}

/* ---------------- UART ---------------- */

static void uart_iniciar(void)
{
	UBRR0H = (uint8_t)(VALOR_UBRR >> 8);
	UBRR0L = (uint8_t)VALOR_UBRR;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 bits, sin paridad, 1 parada
}

static void uart_enviar_caracter(char caracter)
{
	while (!(UCSR0A & (1 << UDRE0))) {
		// Esperar hasta que se pueda enviar
	}

	UDR0 = caracter;
}

static void uart_enviar_texto(const char *texto)
{
	while (*texto != '\0') {
		uart_enviar_caracter(*texto);
		texto++;
	}
}

static void uart_enviar_numero(uint16_t numero)
{
	char texto_numero[6];
	utoa(numero, texto_numero, 10);
	uart_enviar_texto(texto_numero);
}

static uint8_t uart_hay_dato(void)
{
	return (UCSR0A & (1 << RXC0)) != 0;
}

static char uart_recibir_caracter(void)
{
	return UDR0;
}

static void uart_mostrar_menu(void)
{
	uart_enviar_texto("\r\n--- MENU ---\r\n");
	uart_enviar_texto("1: aumentar punto medio\r\n");
	uart_enviar_texto("2: disminuir punto medio\r\n");
	uart_enviar_texto("3: mostrar punto medio\r\n");
	uart_enviar_texto("M: mostrar menu\r\n");
}

/* ---------------- EEPROM ---------------- */

static uint8_t eeprom_leer(uint16_t direccion)
{
	while (EECR & (1 << EEPE)) {
		// Esperar si hay una escritura anterior
	}

	EEAR = direccion;
	EECR |= (1 << EERE);
	return EEDR;
}

static void eeprom_guardar(uint16_t direccion, uint8_t dato)
{
	while (EECR & (1 << EEPE)) {
		// Esperar si hay una escritura anterior
	}

	EEAR = direccion;
	EEDR = dato;
	EECR |= (1 << EEMPE);
	EECR |= (1 << EEPE);
}

static uint8_t cargar_punto_medio(void)
{
	uint8_t valor_guardado = eeprom_leer(DIRECCION_EEPROM_PM);

	// 255 significa EEPROM sin usar. Se aceptan valores de 0 a 99 C.
	if (valor_guardado == 255 || valor_guardado > 99) {
		return PUNTO_MEDIO_INICIAL;
	}

	return valor_guardado;
}

/* ---------------- I2C Y LCD ---------------- */

static void i2c_iniciar(void)
{
	TWSR = 0;
	TWBR = 72; // Aproximadamente 100 kHz con reloj de 16 MHz
	TWCR = (1 << TWEN);
}

static uint8_t i2c_esperar(void)
{
	uint16_t intentos = 60000;

	while (!(TWCR & (1 << TWINT)) && intentos > 0) {
		intentos--;
	}

	return intentos > 0;
}

static uint8_t i2c_enviar_byte(uint8_t direccion, uint8_t dato)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	if (!i2c_esperar()) {
		return 0;
	}

	TWDR = (uint8_t)(direccion << 1);
	TWCR = (1 << TWINT) | (1 << TWEN);
	if (!i2c_esperar()) {
		return 0;
	}

	if ((TWSR & 0xF8) != 0x18) {
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
		return 0;
	}

	TWDR = dato;
	TWCR = (1 << TWINT) | (1 << TWEN);
	if (!i2c_esperar()) {
		return 0;
	}

	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	return 1;
}

static uint8_t lcd_expansor(uint8_t dato)
{
	return i2c_enviar_byte(DIRECCION_LCD, (uint8_t)(dato | BIT_LUZ_LCD));
}

static void lcd_pulso_enable(uint8_t dato)
{
	lcd_expansor((uint8_t)(dato | BIT_ENABLE_LCD));
	_delay_us(1);
	lcd_expansor((uint8_t)(dato & ~BIT_ENABLE_LCD));
	_delay_us(50);
}

static void lcd_enviar_nibble(uint8_t nibble, uint8_t modo_dato)
{
	uint8_t salida = (uint8_t)((nibble & 0x0F) << 4);

	if (modo_dato) {
		salida |= BIT_RS_LCD;
	}

	lcd_expansor(salida);
	lcd_pulso_enable(salida);
}

static void lcd_enviar_byte(uint8_t dato, uint8_t modo_dato)
{
	if (!lcd_disponible) {
		return;
	}

	lcd_enviar_nibble((uint8_t)(dato >> 4), modo_dato);
	lcd_enviar_nibble((uint8_t)(dato & 0x0F), modo_dato);
}

static void lcd_comando(uint8_t comando)
{
	lcd_enviar_byte(comando, 0);
}

static void lcd_caracter(char caracter)
{
	lcd_enviar_byte((uint8_t)caracter, 1);
}

static void lcd_iniciar(void)
{
	_delay_ms(50);

	lcd_disponible = lcd_expansor(0);
	if (!lcd_disponible) {
		return;
	}

	// Secuencia de inicio en modo de 4 bits.
	lcd_enviar_nibble(0x03, 0);
	_delay_ms(5);
	lcd_enviar_nibble(0x03, 0);
	_delay_us(150);
	lcd_enviar_nibble(0x03, 0);
	lcd_enviar_nibble(0x02, 0);

	lcd_comando(0x28); // 4 bits, 2 lineas
	lcd_comando(0x0C); // Pantalla encendida, cursor apagado
	lcd_comando(0x06); // Avance hacia la derecha
	lcd_comando(0x01); // Limpiar
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
	char texto_numero[6];
	utoa(numero, texto_numero, 10);
	lcd_texto(texto_numero);
}

static void lcd_limpiar_linea(uint8_t fila)
{
	uint8_t columna;

	lcd_posicion(fila, 0);
	for (columna = 0; columna < 16; columna++) {
		lcd_caracter(' ');
	}
}

/* ---------------- SALIDAS Y DECISIONES ---------------- */

static void salidas_iniciar(void)
{
	DDRB |= (1 << DDB0); // D8: LED que representa el calefactor
	DDRD |= (1 << DDD6); // D6: PWM para el ventilador

	PORTB &= ~(1 << PORTB0);

	// Timer 0 en PWM rapido, salida OC0A (D6), division 64.
	TCCR0A = (1 << COM0A1) | (1 << WGM01) | (1 << WGM00);
	TCCR0B = (1 << CS01) | (1 << CS00);
	OCR0A = 0;
}

static estado_sistema_t decidir_estado(uint16_t temperatura_c, uint8_t punto_medio)
{
	int16_t temperatura = (int16_t)temperatura_c;
	int16_t punto = (int16_t)punto_medio;

	if (temperatura_c > 100) {
		return ESTADO_ERROR_SENSOR;
	}
	if (temperatura <= punto - 7) {
		return ESTADO_CALENTAR;
	}
	if (temperatura <= punto + 6) {
		return ESTADO_ESTABLE;
	}
	if (temperatura <= punto + 17) {
		return ESTADO_VENTILAR_BAJO;
	}
	if (temperatura <= punto + 28) {
		return ESTADO_VENTILAR_MEDIO;
	}

	return ESTADO_VENTILAR_ALTO;
}

static void aplicar_estado(estado_sistema_t estado)
{
	PORTB &= ~(1 << PORTB0);
	OCR0A = 0;

	switch (estado) {
		case ESTADO_CALENTAR:
		PORTB |= (1 << PORTB0);
		break;
		case ESTADO_VENTILAR_BAJO:
		OCR0A = PWM_BAJO;
		break;
		case ESTADO_VENTILAR_MEDIO:
		OCR0A = PWM_MEDIO;
		break;
		case ESTADO_VENTILAR_ALTO:
		OCR0A = PWM_ALTO;
		break;
		case ESTADO_ESTABLE:
		case ESTADO_ERROR_SENSOR:
		default:
		break;
	}
}

static const char *nombre_estado(estado_sistema_t estado)
{
	switch (estado) {
		case ESTADO_CALENTAR:        return "CALENTAR";
		case ESTADO_ESTABLE:         return "ESTABLE";
		case ESTADO_VENTILAR_BAJO:   return "VENT_BAJO";
		case ESTADO_VENTILAR_MEDIO:  return "VENT_MEDIO";
		case ESTADO_VENTILAR_ALTO:   return "VENT_ALTO";
		default:                     return "ERROR_SENSOR";
	}
}

static uint8_t porcentaje_ventilador(estado_sistema_t estado)
{
	switch (estado) {
		case ESTADO_VENTILAR_BAJO:  return 35;
		case ESTADO_VENTILAR_MEDIO: return 65;
		case ESTADO_VENTILAR_ALTO:  return 100;
		default:                    return 0;
	}
}

static void mostrar_en_lcd(uint16_t temperatura_c,
uint8_t punto_medio,
estado_sistema_t estado)
{
	if (!lcd_disponible) {
		return;
	}

	lcd_limpiar_linea(0);
	lcd_posicion(0, 0);
	lcd_texto("Temp:");
	lcd_numero(temperatura_c);
	lcd_texto(" C");

	lcd_limpiar_linea(1);
	lcd_posicion(1, 0);
	lcd_texto("PM:");
	lcd_numero(punto_medio);
	lcd_texto(" ");

	switch (estado) {
		case ESTADO_CALENTAR:       lcd_texto("CALOR"); break;
		case ESTADO_ESTABLE:        lcd_texto("ESTABLE"); break;
		case ESTADO_VENTILAR_BAJO:  lcd_texto("V.BAJA"); break;
		case ESTADO_VENTILAR_MEDIO: lcd_texto("V.MEDIA"); break;
		case ESTADO_VENTILAR_ALTO:  lcd_texto("V.ALTA"); break;
		default:                    lcd_texto("ERROR"); break;
	}
}

static void informar_por_uart(uint16_t temperatura_c,
uint8_t punto_medio,
estado_sistema_t estado)
{
	uint8_t calefactor = (estado == ESTADO_CALENTAR) ? 1 : 0;

	uart_enviar_texto("Temperatura: ");
	uart_enviar_numero(temperatura_c);
	uart_enviar_texto(" C | Punto medio: ");
	uart_enviar_numero(punto_medio);
	uart_enviar_texto(" C | Accion: ");
	uart_enviar_texto(nombre_estado(estado));
	uart_enviar_texto("\r\n");

	// Esta linea sera facil de leer desde Python para hacer la grafica.
	uart_enviar_texto("DATO,");
	uart_enviar_numero(temperatura_c);
	uart_enviar_caracter(',');
	uart_enviar_numero(punto_medio);
	uart_enviar_caracter(',');
	uart_enviar_numero(calefactor);
	uart_enviar_caracter(',');
	uart_enviar_numero(porcentaje_ventilador(estado));
	uart_enviar_caracter(',');
	uart_enviar_texto(nombre_estado(estado));
	uart_enviar_texto("\r\n");
}

static void atender_menu(uint8_t *punto_medio)
{
	char opcion;

	if (!uart_hay_dato()) {
		return;
	}

	opcion = uart_recibir_caracter();

	if (opcion == '1') {
		if (*punto_medio < 99) {
			(*punto_medio)++;
			eeprom_guardar(DIRECCION_EEPROM_PM, *punto_medio);
		}
		uart_enviar_texto("Punto medio: ");
		uart_enviar_numero(*punto_medio);
		uart_enviar_texto(" C\r\n");
		} else if (opcion == '2') {
		if (*punto_medio > 0) {
			(*punto_medio)--;
			eeprom_guardar(DIRECCION_EEPROM_PM, *punto_medio);
		}
		uart_enviar_texto("Punto medio: ");
		uart_enviar_numero(*punto_medio);
		uart_enviar_texto(" C\r\n");
		} else if (opcion == '3') {
		uart_enviar_texto("Punto medio actual: ");
		uart_enviar_numero(*punto_medio);
		uart_enviar_texto(" C\r\n");
		} else if (opcion == 'm' || opcion == 'M') {
		uart_mostrar_menu();
	}
}

static void esperar_y_atender_uart(uint8_t *punto_medio)
{
	uint8_t parte;

	// 50 partes de 100 ms forman los cinco segundos.
	for (parte = 0; parte < 50; parte++) {
		atender_menu(punto_medio);
		_delay_ms(100);
	}
}

int main(void)
{
	uint8_t punto_medio;
	uint16_t temperatura_c;
	estado_sistema_t estado;

	adc_iniciar();
	uart_iniciar();
	salidas_iniciar();
	i2c_iniciar();
	lcd_iniciar();

	punto_medio = cargar_punto_medio();

	uart_enviar_texto("Sistema de control de temperatura iniciado.\r\n");
	if (!lcd_disponible) {
		uart_enviar_texto("Aviso: LCD I2C no encontrado. Revisar direccion y conexiones.\r\n");
	}
	uart_mostrar_menu();

	while (1) {
		temperatura_c = leer_temperatura();
		estado = decidir_estado(temperatura_c, punto_medio);

		aplicar_estado(estado);
		mostrar_en_lcd(temperatura_c, punto_medio, estado);
		informar_por_uart(temperatura_c, punto_medio, estado);

		esperar_y_atender_uart(&punto_medio);
	}
}
