# Menu UART y punto medio

La terminal UART permite consultar y cambiar el punto medio sin modificar el codigo.
La comunicacion se configura a 9600 baudios, 8 bits, sin paridad y 1 bit de parada.

## Comandos

| Tecla | Funcion |
|---|---|
| `1` | Aumenta el punto medio en 1 C. |
| `2` | Disminuye el punto medio en 1 C. |
| `3` | Muestra el punto medio actual. |
| `M` | Vuelve a mostrar el menu. |

No es necesario presionar Enter. El valor inicial es 22 C y se puede ajustar entre
0 y 99 C. Cada cambio se guarda en la memoria EEPROM del ATmega328P, por lo que el
valor puede conservarse al reiniciar el sistema.

El punto medio desplaza los limites usados para decidir el estado. Con PM igual a
22 C, los rangos de prueba definidos para el trabajo son:

| Temperatura | Accion |
|---|---|
| 0 a 16 C | Calefactor encendido. |
| 17 a 28 C | Calefactor y motor apagados. |
| 29 a 39 C | Motor a velocidad baja. |
| 40 a 50 C | Motor a velocidad media. |
| 51 a 100 C | Motor a velocidad alta. |

La terminal tambien muestra la temperatura, el punto medio y la accion elegida.
Ademas envia una linea que comienza con `DATO`, preparada para registrar los valores
desde Python o MATLAB.

