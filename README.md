# Proyecto Extremo - Fundamentos y Técnicas en Computación Gráfica

## Tabla de contenido

* [Modelos](#modelos)
* [Transformaciones afines](#transformaciones-afines)
* [Métodos de despliegue](#métodos-de-despliegue)
* [Iluminación](#iluminación)
* [Función de transferencia](#función-de-transferencia)
* [Instalación y uso](#instalación-y-uso)
* [Metodología](#metodología)
* [Integrantes](#integrantes)

# Modelos

Desde el menú de AntTweakBar se puede seleccionar el modelo a desplegar. Entre ellos se tienen:

	- Engine (8 bits) que representa a un motor.
		
	- Head (8 bits) que representa a una cabeza humana.
	
	- Torso (8 bits) que representa a una torso humano.
	
	- Walnut (16 bits) que representa a una nuez.

# Transformaciones afines

A cualquier modelo cargado se le puede aplicar libremente transformaciones afines:

	- La rotación puede ser realizada manteniendo el click derecho del ratón y moviendo el modelo.
	
	- La traslación puede ser realizada manteniendo el click izquierdo del ratón y moviendo el modelo. Si se desea trasladar el modelo en el eje Z, se debe realizar el mismo procedimiento manteniendo presionada la tecla 'Z'.
	
	- El escalamiento se puede ajustar con la rueda del ratón.
	
	- Se puede acercar y alejar la cámara con las teclas 'W' y 'S' respectivamente.

# Métodos de despliegue

Se implementaron los métodos de despliegue Ray Casting y Texturas 2D. Se puede seleccionar cualquiera de éstos desde el menú de AntTweakBar.

# Iluminación

La iluminación fue implementada para el método de despliegue Ray Casting, siendo Phong el método de interpolación, Lambert el modo difuso y Blinn-Phong el modo especular. Ésta puede ser encendida, apagada y trasladada desde el menú de AntTweakBar.

Una comparación entre la iluminación estando desactivada y activada puede observarse en la siguiente imagen:

![alt tag](https://i.gyazo.com/562ddafeaa4245a9c556321791bdd4c6.png)

# Función de transferencia

La función de transferencia se encontrará inicialmente en la función identidad. Se pueden crear nuevos puntos libremente y trasladarlos como se desee. Cada punto en la función puede ser eliminado si se mantiene clickeado con el ratón y se presiona la tecla 'D'. Cabe destacar que no se pueden eliminar ni el primero ni el último punto.

Cada punto tendrá un color asociado que puede ser seleccionado desde los pickers de color.

# Herramientas

En la presente tarea se utilizaron las siguientes herramientas con sus respectivas versiones:

| Herramienta                         	 | Versión   													   |                            
|----------------------------------------|-----------------------------------------------------------------|
| Microsoft Visual Studio        	 	 | 2015      													   |


# Instalación y uso
Se deberá descargar el siguiente repositorio o clonarlo con el comando:

git clone https://github.com/josemalvarezg1/Fundamentos-y-Tecnicas-en-Computacion-Grafica-Proyecto-Extremo.git

Se puede abrir el archivo .sln en Visual Studio o ejecutar directamente el proyecto desde la carpeta bin/.

![alt tag](https://i.gyazo.com/6f105082745ee9f754a08151bab59f11.png)

# Metodología
Una documentación sobre la metodología de programación que se utilizó para el desarrollo de este proyecto se encuentra en el archivo Proyecto Extremo - Documentación.pdf.

# Integrantes

**José Manuel Alvarez - CI 25038805**

**Alejandro Barone - CI 24206267**