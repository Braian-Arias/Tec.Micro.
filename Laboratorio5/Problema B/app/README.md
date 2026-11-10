# Aplicación MIT App Inventor

Importar `Laboratorio5_ProblemaB.aia` desde **Projects → Import project (.aia) from my computer**. El archivo se importó correctamente en MIT App Inventor y el proyecto compiló como aplicación Android sin errores de bloques. La prueba de comunicación con el HC-05 y el firmware instalado en el ATmega328P requiere un teléfono Android y el montaje físico.

La aplicación usa Bluetooth clásico SPP. Primero hay que emparejar el HC-05 en los ajustes del teléfono y conceder los permisos Bluetooth que solicite Android. Después, en la app, pulsar **Conectar HC-05** y elegir el dispositivo emparejado. Los controles permiten consultar estado, sensores y alarmas; pedir ACTIVO, STANDBY o APAGADO; encender y apagar el LED; y solicitar emergencia.

Cada botón envía la trama binaria de cinco bytes definida en [protocolo_tramas.md](../protocolo_tramas.md). La app recibe bytes, espera una trama completa, comprueba el byte final y el XOR, y muestra el estado, las lecturas, las alarmas o el error recibido. Si una solicitud no tiene respuesta tras unos tres segundos, muestra una advertencia. En APAGADO, el firmware deshabilita la UART, por lo que la aplicación no puede despertarlo por Bluetooth: se usa el pulsador físico.

La emergencia remota es complementaria. Solo funciona con el enlace Bluetooth disponible, y el firmware exige pulsación física para recuperarse. La temperatura mostrada es el entero enviado por el firmware; el valor ADC mostrado es el dato de 8 bits que transmite el protocolo, no el valor bruto de 10 bits.
