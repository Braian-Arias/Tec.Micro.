.include "m328Pdef.inc"

.cseg

.org 0x0000
    rjmp inicio

; ----------------------------------------------------------
; INICIO
; ----------------------------------------------------------

inicio:

    ; configurar pila
    ldi r16, HIGH(RAMEND)
    out SPH, r16

    ldi r16, LOW(RAMEND)
    out SPL, r16


    ; PORTD
    ; PD2 = boton siguiente
    ; PD4 = boton anterior
    ; PD3, PD5, PD6 y PD7 = filas 1 a 4

    ldi r16, 0b11101000
    out DDRD, r16

    ; filas apagadas y pull-up de botones
    ldi r16, 0b11111100
    out PORTD, r16


    ; PORTB
    ; PB0 a PB3 = filas 5 a 8
    ; PB4 = C2
    ; PB5 = C1

    ldi r16, 0b00111111
    out DDRB, r16

    ldi r16, 0b00001111
    out PORTB, r16


    ; PORTC
    ; PC5 = C3
    ; PC4 = C4
    ; PC3 = C5
    ; PC2 = C6
    ; PC1 = C7
    ; PC0 = C8

    ldi r16, 0b00111111
    out DDRC, r16

    clr r16
    out PORTC, r16


    ; r18 guarda la figura actual
    ; 0 = sonrisa

    clr r18

; ----------------------------------------------------------
; PROGRAMA PRINCIPAL
; ----------------------------------------------------------

principal:

    rcall mostrar_actual


    ; boton D2
    sbis PIND, PIND2
    rjmp siguiente


    ; boton D4
    sbis PIND, PIND4
    rjmp anterior


    rjmp principal

; ----------------------------------------------------------
; SIGUIENTE
; ----------------------------------------------------------

siguiente:

    inc r18

    ; figuras 0 a 5
    cpi r18, 6
    brlo esperar_d2

    clr r18


esperar_d2:

    ; seguir refrescando mientras esta apretado
    rcall mostrar_actual

    sbis PIND, PIND2
    rjmp esperar_d2


    ; pequeño antirrebote
    ldi r22, 5


rebote_d2:

    rcall mostrar_actual

    dec r22
    brne rebote_d2

    rjmp principal

; ----------------------------------------------------------
; ANTERIOR
; ----------------------------------------------------------

anterior:

    cpi r18, 0
    brne restar_figura

    ; si esta en 0 pasa a 5
    ldi r18, 5
    rjmp esperar_d4


restar_figura:

    dec r18


esperar_d4:

    rcall mostrar_actual

    sbis PIND, PIND4
    rjmp esperar_d4


    ldi r22, 5


rebote_d4:

    rcall mostrar_actual

    dec r22
    brne rebote_d4

    rjmp principal

; ----------------------------------------------------------
; ELEGIR FIGURA
; ----------------------------------------------------------

mostrar_actual:

    cpi r18, 0
    brne comprobar1
    rjmp sonrisa


comprobar1:

    cpi r18, 1
    brne comprobar2
    rjmp guino


comprobar2:

    cpi r18, 2
    brne comprobar3
    rjmp corazon


comprobar3:

    cpi r18, 3
    brne comprobar4
    rjmp cara3


comprobar4:

    cpi r18, 4
    brne comprobar5
    rjmp asterisco


comprobar5:

    rjmp xd

; ----------------------------------------------------------
; 0 - CARITA SONRIENDO
; ----------------------------------------------------------

sonrisa:

    ldi r16, 0b00111100
    ldi r17, 0
    rcall mostrar_fila

    ldi r16, 0b01000010
    ldi r17, 1
    rcall mostrar_fila

    ldi r16, 0b10100101
    ldi r17, 2
    rcall mostrar_fila

    ldi r16, 0b10000001
    ldi r17, 3
    rcall mostrar_fila

    ldi r16, 0b10000001
    ldi r17, 4
    rcall mostrar_fila

    ldi r16, 0b10100101
    ldi r17, 5
    rcall mostrar_fila

    ldi r16, 0b01011010
    ldi r17, 6
    rcall mostrar_fila

    ldi r16, 0b00111100
    ldi r17, 7
    rcall mostrar_fila

    ret

; ----------------------------------------------------------
; 1 - CARITA GUIÑANDO
; ----------------------------------------------------------

guino:

    ldi r16, 0b00111100
    ldi r17, 0
    rcall mostrar_fila

    ldi r16, 0b01000010
    ldi r17, 1
    rcall mostrar_fila

    ; un ojo normal y otro guiñado
    ldi r16, 0b10100001
    ldi r17, 2
    rcall mostrar_fila

    ldi r16, 0b10011001
    ldi r17, 3
    rcall mostrar_fila

    ldi r16, 0b10000001
    ldi r17, 4
    rcall mostrar_fila

    ldi r16, 0b10100101
    ldi r17, 5
    rcall mostrar_fila

    ldi r16, 0b01011010
    ldi r17, 6
    rcall mostrar_fila

    ldi r16, 0b00111100
    ldi r17, 7
    rcall mostrar_fila

    ret

; ----------------------------------------------------------
; 2 - CORAZON
; ----------------------------------------------------------
corazon:

    ldi r16, 0b00000000
    ldi r17, 0
    rcall mostrar_fila

    ldi r16, 0b01100110
    ldi r17, 1
    rcall mostrar_fila

    ldi r16, 0b11111111
    ldi r17, 2
    rcall mostrar_fila

    ldi r16, 0b11111111
    ldi r17, 3
    rcall mostrar_fila

    ldi r16, 0b01111110
    ldi r17, 4
    rcall mostrar_fila

    ldi r16, 0b00111100
    ldi r17, 5
    rcall mostrar_fila

    ldi r16, 0b00011000
    ldi r17, 6
    rcall mostrar_fila

    ldi r16, 0b00000000
    ldi r17, 7
    rcall mostrar_fila

    ret

; ----------------------------------------------------------
; 3 - :3
; ----------------------------------------------------------

cara3:

    ldi r16, 0b00000000
    ldi r17, 0
    rcall mostrar_fila

    ldi r16, 0b01011100
    ldi r17, 1
    rcall mostrar_fila

    ldi r16, 0b01000010
    ldi r17, 2
    rcall mostrar_fila

    ldi r16, 0b00001100
    ldi r17, 3
    rcall mostrar_fila

    ldi r16, 0b01000010
    ldi r17, 4
    rcall mostrar_fila

    ldi r16, 0b01000010
    ldi r17, 5
    rcall mostrar_fila

    ldi r16, 0b00011100
    ldi r17, 6
    rcall mostrar_fila

    ldi r16, 0b00000000
    ldi r17, 7
    rcall mostrar_fila

    ret

; ----------------------------------------------------------
; 4 - ASTERISCO
; ----------------------------------------------------------

asterisco:

    ldi r16, 0b00000000
    ldi r17, 0
    rcall mostrar_fila

    ldi r16, 0b00100100
    ldi r17, 1
    rcall mostrar_fila

    ldi r16, 0b00011000
    ldi r17, 2
    rcall mostrar_fila

    ldi r16, 0b01111110
    ldi r17, 3
    rcall mostrar_fila

    ldi r16, 0b00011000
    ldi r17, 4
    rcall mostrar_fila

    ldi r16, 0b00100100
    ldi r17, 5
    rcall mostrar_fila

    ldi r16, 0b00000000
    ldi r17, 6
    rcall mostrar_fila

    ldi r16, 0b00000000
    ldi r17, 7
    rcall mostrar_fila

    ret

; ----------------------------------------------------------
; 5 - XD
; ----------------------------------------------------------

xd:

    ldi r16, 0b10101110
    ldi r17, 0
    rcall mostrar_fila

    ldi r16, 0b01001001
    ldi r17, 1
    rcall mostrar_fila

    ldi r16, 0b10101001
    ldi r17, 2
    rcall mostrar_fila

    ldi r16, 0b00001001
    ldi r17, 3
    rcall mostrar_fila

    ldi r16, 0b10101001
    ldi r17, 4
    rcall mostrar_fila

    ldi r16, 0b01001001
    ldi r17, 5
    rcall mostrar_fila

    ldi r16, 0b10101110
    ldi r17, 6
    rcall mostrar_fila

    ldi r16, 0b00000000
    ldi r17, 7
    rcall mostrar_fila

    ret

; ----------------------------------------------------------
; MOSTRAR UNA FILA
;
; r16 = dibujo de la fila
; r17 = numero de fila
; ----------------------------------------------------------

mostrar_fila:

    ; guardar dibujo
    mov r21, r16


    ; apagar F1-F4
    ; mantener pull-up D2 y D4

    ldi r19, 0b11111100
    out PORTD, r19


    ; apagar F5-F8
    ; apagar C1 y C2

    ldi r19, 0b00001111
    out PORTB, r19


    ; apagar C3-C8

    clr r19
    out PORTC, r19


    ; ----------------------------------------
    ; C1 y C2
    ;
    ; bit 7 -> C1
    ; bit 6 -> C2
    ; ----------------------------------------

    mov r19, r21
    andi r19, 0b11000000

    lsr r19
    lsr r19

    ori r19, 0b00001111

    out PORTB, r19


    ; ----------------------------------------
    ; C3 hasta C8
    ; ----------------------------------------

    mov r19, r21
    andi r19, 0b00111111

    out PORTC, r19


    ; elegir fila

    cpi r17, 0
    brne revisar_f2
    rjmp f1


revisar_f2:

    cpi r17, 1
    brne revisar_f3
    rjmp f2


revisar_f3:

    cpi r17, 2
    brne revisar_f4
    rjmp f3


revisar_f4:

    cpi r17, 3
    brne revisar_f5
    rjmp f4


revisar_f5:

    cpi r17, 4
    brne revisar_f6
    rjmp f5


revisar_f6:

    cpi r17, 5
    brne revisar_f7
    rjmp f6


revisar_f7:

    cpi r17, 6
    brne usar_f8
    rjmp f7


usar_f8:

    rjmp f8

; ----------------------------------------------------------
; FILAS
; ----------------------------------------------------------

f1:

    cbi PORTD, PD3
    rjmp mantener


f2:

    cbi PORTD, PD5
    rjmp mantener


f3:

    cbi PORTD, PD6
    rjmp mantener


f4:

    cbi PORTD, PD7
    rjmp mantener


f5:

    cbi PORTB, PB0
    rjmp mantener


f6:

    cbi PORTB, PB1
    rjmp mantener


f7:

    cbi PORTB, PB2
    rjmp mantener


f8:

    cbi PORTB, PB3

; -----------------------------------------------------------
; PAUSA PARA REFRESCAR MATRIZ
; -----------------------------------------------------------

mantener:

    ldi r20, 60

espera:

    dec r20
    brne espera

    ret