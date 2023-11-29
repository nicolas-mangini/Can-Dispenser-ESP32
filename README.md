# Dispensador de Latas
![BANNER](https://github.com/nicolas-mangini/can-dispenser-esp32/assets/72108522/1cd40197-d99e-4ca4-8d18-c77e998b9a33)

## Definición y especificación de requerimientos
Los requisitos de nuestro proyecto describen los servicios y elementos que el mismo ofrece y las restricciones asociadas a su funcionamiento, es decir, las propiedades o restricciones que se deben satisfacer han sido determinadas de forma precisa. Entre estos requisitos enumeramos por secciones a cada uno

### Hardware
- <ins>Dispenser:</ins> Un dispenser físico impreso en 3D que dispensa latas. 
- <ins>Microcontrolador:</ins> ESP32 para interfaz con sensores, actuadores y gestión de la comunicación. 
- <ins>Actuadores:</ins> Servo para dispensar productos e indicadores LED. 
- <ins>Conectividad:</ins> Módulo ESP32 con Wi-Fi para conectar el microcontrolador a la nube.
### Sistema de broker en la Nube
- <ins>Broker MQTT:</ins> Configuración de un broker de mosquitto para facilitar la comunicación entre la máquina expendedora y la plataforma en la nube. 
- <ins>Temas:</ins> Definición de temas (topics) MQTT para diversos tipos de intercambio de datos, como actualizar la existencia de productos y el dispensado de la máquina. 

### Plataforma en la Nube
- <ins>Servicio en la Nube:</ins> Uso de plataforma en la nube, AWS puntualmente, para el almacenamiento, procesamiento y gestión de datos.

### Panel de Control para los Usuarios
- <ins>Determinación de categorías de usuarios:</ins> Cualquier usuario anónimo suscrito al broker accede a la máquina expendedora. 
- <ins>Interfaz Web:</ins> Disponemos de un panel de control basado en la web accesible desde navegadores y dispositivos móviles. 
- <ins>Monitoreo en Tiempo Real:</ins> Se muestra el estado en tiempo real de la máquina y la disponibilidad de los productos. 
- <ins>Funciones de Control:</ins> Permitimos a los usuarios activar la dispensación de productos de forma remota. Mientras que otro tipo de usuarios es capaz de verificar la existencia de stock y ajustar la configuración de la máquina expendedora. 

### Seguridad y Privacidad
- <ins>Comunicación Segura:</ins> Implementación de cifrado TLS para la comunicación segura entre la máquina expendedora y la nube. 
- <ins>Cifrado de Datos:</ins> Cifrar datos sensibles en reposo y durante la transmisión.
- <ins>Consideraciones de Privacidad:</ins> Garantizar que los datos del usuario sean anónimos y se manejen con consideraciones de privacidad. 

## Hitos del Proyecto y tareas completadas: 
- Creación de instancias en Amazon. 
- Configuración de acceso a las instancias mediante SSH. 
- Instalación de un broker MQTT (Mosquitto) en la instancia principal.
- Creación y configuración de una segunda instancia. 
- Instalación de MongoDB en la segunda instancia. 
- Desarrollo y prueba exitosa de un script Node que suscribe tópicos y guarda datos en la base de datos. 
- Generación de clave y certificado para la CA falsa, así como para el servidor Mosquitto.
- Configuración y certificación del servidor y cliente MQTT. 
- Configuración del proyecto con platformIO para generar tópicos de suscripción y manejo de stock. 
- Configuración del programa para guardar mensajes de manejo de stock en la base de datos. 
- Programa para mostrar el stock en la matriz LED. 
- Desarrollo de dispositivo para dispensar latas mediante una mecanismo impreso en 3D.

## Definición general del Proyecto
Crear una máquina expendedora utilizando prácticas de IoT, específicamente una máquina de latas con control de stock mediante una base de datos MongoDB. Además, mostrar el stock en una matriz LED y permitir a los usuarios suscribirse mediante MQTT para realizar peticiones a la máquina. 

## Problemas y Desafíos Superados
- <ins>Problemas con la versión de Node y paquetes defectuosos:</ins> Creación de una nueva instancia. Esto involucró una reinstalación limpia de Node y la replicación de configuraciones, asegurando así un entorno estable para el desarrollo y la ejecución del proyecto. 
- <ins>Dificultades con la instalación de la base de datos mediante Docker:</ins> Se optó por otra solución. 
- <ins>Interrogantes sobre el mecanismo para soltar latas:</ins> Impresión de materiales y elementos para el dispensador en una impresora 3D. 
- <ins>Implementación de escritura automática en la base de datos:</ins> Una implementación e integración de back y front mediante un script de python para el registro de datos.
- <ins>Escala del proyecto para agregar más máquinas expendedoras o manejar un aumento significativo en el tráfico:</ins> Concluimos que esto puede lograrse mediante la implementación de una arquitectura distribuida, implicando a la replicación y coordinación de instancias, así como el uso de servicios en la nube para manejar el aumento de tráfico y la adición de nuevas máquinas expendedoras. 
- <ins>Integración de compuerta de dispensación de latas con el sistema de control:</ins> La integración de la compuerta con el sistema de control se logra mediante protocolos de comunicación estándar a través del MQTT. La compuerta es controlada por publicaciones y suscripciones desde la interfaz de usuario. 
- <ins>Protocolos o tecnologías utilizadas:</ins> En el proyecto utilizamos varios protocolos y tecnologías para garantizar la conectividad, comunicación y funcionalidad del sistema. Entre ellas MQTT, cifrado TLS, AWS IoT, MongoDB y PlatformIO. Colaborando de manera sinérgica para crear un sistema integral que garantice la conectividad segura, la comunicación eficiente y el manejo de datos en tiempo real. 
- <ins>Uso de interfaz de usuario:</ins> La implementación de una interfaz de usuario nos resultó altamente beneficiosa. Los usuarios tienen una experiencia intuitiva para monitorear el estado de la máquina, verificar el stock, realizar pedidos y recibir notificaciones. Esta misma está implementada en una interfaz web. 

## Arquitectura del proyecto: 
### Diagrama del proyecto
Consiste en un diagrama donde se representan todas las partes principales que componen el proyecto. Este mismo proyecta la organización y la relación entre los elementos que la componen: 

![image](https://github.com/nicolas-mangini/can-dispenser-esp32/assets/72108522/99023945-5283-4ac0-9f75-86d8e3a9aa35)

### Descripción individual de los elementos
- <ins>ESP32:</ins> El ESP32 es un microcontrolador versátil y potente que desempeña un papel crucial en el proyecto del dispensador de latas. Funciona como el cerebro del sistema, el código del script, los actuadores y la comunicación con la nube. Con capacidades inalámbricas, el ESP32 también nos permite la conectividad Wi-Fi, facilitando la comunicación remota y la integración con servicios en la nube. 

- <ins>Push Button:</ins> El botón pulsador (Push Button) cumple una función esencial en la interacción usuario-máquina. Permite a los usuarios realizar acciones específicas, como solicitar una lata. Su simplicidad y facilidad de uso hacen que sea una interfaz realmente intuitiva para los usuarios. 

- <ins>2 LEDs:</ins> Los LEDs (Light Emitting Diodes) tienen una doble función en el proyecto. Actuar como indicadores visuales para los usuarios, mostrando información sobre el estado de la máquina (por ejemplo, disponibilidad de productos). Además, pueden utilizarse como herramientas de diagnóstico para señalar posibles problemas de cableado o estados específicos del sistema. 

- <ins>Buzzer:</ins> El buzzer agrega una dimensión auditiva al proyecto. Puede utilizarse para proporcionar retroalimentación audible a los usuarios, indicando, por ejemplo, que su solicitud ha sido procesada o alertando sobre eventos importantes. Su capacidad para transmitir información mediante sonidos complementa las señales visuales proporcionadas por los LEDs. 

- <ins>Matriz LED:</ins> La matriz LED es un componente visual clave en el proyecto. Se utiliza para mostrar información detallada sobre el stock de productos, brindando a los usuarios una representación clara de la disponibilidad del stock. La matriz LED nos presenta datos de manera gráfica, como números o patrones, mejorando así la experiencia visual del usuario. 

- <ins>Servo:</ins> El servo es un actuador esencial para el mecanismo de dispensación de latas. Se utiliza para controlar el mecanismo que permite la salida controlada de las latas. Su capacidad para realizar movimientos precisos y controlados garantiza una dispensación eficiente y precisa de los productos. 

Estos componentes, en conjunto, proporcionan una interfaz completa y eficaz entre el usuario y la máquina expendedora, ofreciendo funcionalidades clave como la solicitud de productos, la visualización del stock y la retroalimentación auditiva, intuitiva y visual para mejorar la experiencia global del usuario. 

## DEMO
https://github.com/nicolas-mangini/can-dispenser-esp32/assets/72108522/7613198d-2dad-4eb9-a2d3-b34b3739ddbb

https://github.com/nicolas-mangini/can-dispenser-esp32/assets/72108522/da9cac3b-66ad-488b-a3ad-6f9ebf2c0450

https://github.com/nicolas-mangini/can-dispenser-esp32/assets/72108522/b809f0b6-1e81-4d86-8e73-073f61684296

## Conclusión
El presente proyecto de Internet de las cosas (IoT) busca proporcionar a los participantes una inmersión práctica en la concepción, desarrollo e implementación de un sistema avanzado de máquina expendedora. Mediante la integración de tecnologías de comunicación en la nube, así como el almacenamiento y análisis de datos, se busca dotar a los participantes de conocimientos significativos en relación con las aplicaciones y desafíos que caracterizan el ámbito del IoT en situaciones del mundo real. La implementación del panel de control destinado a los usuarios, por su parte, facilitará una gestión y monitoreo fluidos e intuitivos, enriqueciendo la experiencia global del usuario. 
