# TpF-PdM-FrancoGeronimo-secPdm
Proyecto que permite cargar secuencias de leds mediante el ingreso de datos vía interfaz serial.
## Nota: Probado en cuteCom. En el mismo:

Colocar "none" en la casilla al lado de la opción input, para que no envie ningún caracter, enter, adicional del dato que envía.

Coloque en "Char delay" el valor "1 ms". (Si decide enviar todo la secuencia con un solo envio en cuteCom)

## Formato de la secuencia-> l+(1,2,3)+','+Número(0 a 10000)+Modo(a,e,d)+Número(0 a 100)+','+Número(0 a 10000)+Modo(a,e,d)+Número(0 a 100)+...+'.'
El número antes del modo es la duración del paso. Debe estar en el rango 0 a 10000.

Modos:
a-> "apagar", 
e-> "encender", 
d-> "dimmer".

El número después del modo es la intensidad del paso. Debe estar en el rango 0 a 100.

Acepta hasta un maximo de 10 pasos por secuencia.

Ejemplos de comandos:

l1,1000a0,500e10,500a0,500e60,500a0,500e100,1500d0.

l2,1000a0,2500d100,3000d0.

l3,500e5,1000a0,3000d100,2500d0,2000a0.

## Capa principal -> secPdm.c secPdm.h
## Componentes MEF secuencia-> secTpfPdm.c secTpfPdm.h
## Componentes LedSec-> ledTpfPdm.c ledTpfPdm.h
## Componentes utilFEG-> utilFeg.c utilFeg.h

