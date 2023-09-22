*Luciano Gianfelice, Valentino Di Capua, Nicolas Mangini y Josué Riera*

<a name="_cpxybwdkc7a2"></a>Proyecto Máquina expendedora

**Visión General del Proyecto:** El objetivo de este proyecto es diseñar y desarrollar una máquina expendedora inteligente de Cup Noodles utilizando tecnologías IoT.

**Componentes del Proyecto:**
1\. **Configuración de Hardware:** 

Máquina Expendedora: Una máquina expendedora física o simulada que dispensa Cup Noodles.

Microcontrolador: ESP32 para interfaz con sensores, actuadores y gestión de la comunicación. 

Sensores: Sensor de nivel de existencias de productos.

Actuadores: Motores para dispensar productos, indicadores LED y  un botón. 

Conectividad: Módulo Wi-Fi para conectar el microcontrolador a la nube.

2\. **Sistema de *broker* en la Nube:**
*Broker* MQTT: Configurar un *broker en la nube* usando Mosquitto para facilitar la comunicación entre la máquina expendedora y la plataforma en la nube. 

Temas: Definir temas (*topics*) MQTT para diversos tipos de intercambio de datos, como disponibilidad de productos, estado de la máquina y datos de sensores. 

3\. **Plataforma en la Nube:**
Servicio en la Nube: Utilizamos AWS.

Almacenamiento de Datos:Amazon Bucket S3.

4\. **Panel de Control para los Usuarios:** 

Determinación de categorías de usuarios: Posibles usuarios son los consumidores que querrán comprar y recibir el producto y las personas que se encargan de restockear. 

Interfaz Web: Panel de control para usuarios utilizando guizero. 

Autenticación de Usuarios: Implementar autenticación y autorización de usuarios para un acceso seguro. 

Monitoreo en Tiempo Real: Mostrar el estado en tiempo real de la máquina, disponibilidad de productos (stock de cupnoodles)  y datos de sensores. 

Funciones de Control: Permitir a los usuarios activar la dispensación de productos de forma remota, verificar los niveles de existencias y ajustar la configuración de la máquina expendedora. 

Visualización de Datos: Generar gráficos y diagramas para visualizar tendencias de ventas, popularidad de productos y lecturas de sensores. 

5\. **Seguridad y Privacidad:**
Comunicación Segura: Implementar SSL para la comunicación segura entre la máquina expendedora y la nube.


Cifrado de Datos: Cifrar datos sensibles en reposo y durante la transmisión. 

Consideraciones de Privacidad: Garantizar que los datos del usuario e historial de compras se anonimicen y se manejen con consideraciones de privacidad. 



