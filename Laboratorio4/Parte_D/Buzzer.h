
#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>

#define NOTA_DO   262
#define NOTA_RE   294
#define NOTA_MI   330
#define NOTA_FA   349
#define NOTA_SOL  392
#define NOTA_LA   440
#define NOTA_SI   494
#define NOTA_DO2  523

void buzzer_inicializar(void);
void reproducir_nota(uint16_t frecuencia);
void buzzer_detener(void);

#endif
