# FOCUSBOT: Open-Source Autonomous AI Desktop Companion Robot

<p align="center">
  <img src="docs/images/renderfocusbotdefrente.jpg" alt="FocusBot Front View Render" width="48.5%">
  <img src="docs/images/renderfocusbotlado.jpg" alt="FocusBot Side View Render" width="48.5%">
</p>

<p align="center">
  <a href="https://ohwr.org/cern_ohl_s_v2.txt"><img src="https://img.shields.io/badge/Hardware_License-CERN--OHL--S-0284c7.svg?style=for-the-badge" alt="CERN-OHL-S"></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/Software_License-MIT-38bdf8.svg?style=for-the-badge" alt="MIT"></a>
  <img src="https://img.shields.io/badge/Status-Production%20Grade%20v12.0-34d399.svg?style=for-the-badge" alt="Production Grade">
  <a href="https://www.freecad.org/"><img src="https://img.shields.io/badge/CAD-FreeCAD%201.0%20OCC-a855f7.svg?style=for-the-badge" alt="FreeCAD"></a>
  <a href="https://www.kicad.org/"><img src="https://img.shields.io/badge/EDA-KiCad%2010.0-fbbf24.svg?style=for-the-badge" alt="KiCad 10.0"></a>
  <a href="https://www.espressif.com/"><img src="https://img.shields.io/badge/Compute-ESP32--S3%20Xtensa%20LX7-ef4444.svg?style=for-the-badge" alt="ESP32-S3"></a>
</p>

---

## 📖 Índice General

1. [Descripción del Proyecto](#-descripción-del-proyecto)
2. [Arquitectura Mecánica y Carcasa DFA](#-arquitectura-mecánica-y-carcasa-dfa)
3. [Guía de Impresión 3D](#-guía-de-impresión-3d-paso-a-paso)
4. [Guía de Ensamblaje Mecatrónico](#-guía-de-ensamblaje-mecatrónico-paso-a-paso)
5. [Arquitectura Electrónica y PCBs](#-arquitectura-electrónica-y-pcbs)
6. [Guía de Carga de Firmware (PlatformIO)](#-guía-de-carga-de-firmware)
7. [Configuración de la IA y APIs de LLM](#-configuración-de-la-ia-y-apis-de-llm)
8. [Expresiones de Ojos y Movimientos Cutes](#-expresiones-de-ojos-y-movimientos-cutes)
9. [Lista de Materiales (BOM)](#-lista-de-materiales-bom)
10. [Licencia y Comunidad](#-licencia-y-comunidad)

---

## 🤖 Descripción del Proyecto

**FocusBot** es un robot de escritorio autónomo, compacto (**8.0 cm** de altura) y de código abierto diseñado como compañero de productividad, estudio e investigación mecatrónica. Equipado con visión por computadora, cuello articulado, locomoción diferencial, sintetizador de audio digital, micrófono MEMS y una pantalla IPS panorámica de 2.0 pulgadas con ojos procedurales bio-inspirados estilo Cozmo/Anki, FocusBot monitoriza tus sesiones de estudio, previene la procrastinación y te acompaña con una personalidad tierna y robótica mediante Modelos de Lenguaje (LLMs).

Es **100% Hardware y Software Libre**, optimizado para fabricarse con impresoras 3D domésticas (FDM/Resina) y PCBs de 2 capas estándar.

---

## 📐 Arquitectura Mecánica y Carcasa DFA

FocusBot incorpora una arquitectura monocoque **Top-Down DFA (Design for Assembly)** modelada en FreeCAD con el núcleo Open CASCADE:

<p align="center">
  <img src="docs/images/case_architecture.png" alt="FocusBot Case Architecture & Engineering Views" width="850">
</p>

### Características Clave de la Carcasa:
- **Carcasa Frontal Monolítica**: La cabeza frontal (`Carcasa_Cabeza_Frontal`) unifica el marco del visor y la estructura en una sola pieza sin juntas perimetrales ni holguras defectuosas. Cuenta con ventana activa de $43.5 \times 23.0\text{ mm}$ ($R=2.0\text{ mm}$) que aloja el panel LCD ST7789V de 2.0", y un orificio coaxial $\varnothing 2.6\text{ mm}$ para la cámara OV2640 a $Z = 71.0\text{ mm}$.
- **Orejas de Gato Bajas y Orgánicas**: Orejas curvadas y redondeadas de $5.3\text{ mm}$ de elevación sobre el techo, con cierre en vértice suave (cero tapas cónicas ni discos planos).
- **Difusor LED de Pecho en Forma Stadium (Cápsula Redondeada)**: Ranura frontal horizontal en el pecho de $19.6 \times 5.4\text{ mm}$ con bisel embutido de $23.6 \times 7.8\text{ mm}$ y pinhole de micrófono centrado arriba ($Z = 32.2\text{ mm}$), diseñada para emitir el brillo cyan característico del robot.
- **Tornillería Invisible y Cierre Bottom-Up**: La tapa del torso se fija mediante 4 tornillos M2 introducidos desde la base inferior hacia bosses ciegos en la cúpula, dejando el capó exterior 100% limpio y liso.
- **Cuello Flotante sin Fricción**: Holgura perimetral de aire de $0.80\text{ mm}$ entre cabeza y torso alrededor de una tornamesa de $\varnothing 18.0\text{ mm}$, garantizando rotación libre de -90° a +90° con $0.0000\text{ mm}^3$ de colisión.
- **Plano de Suelo Cinemático ($Z = 0.00\text{ mm}$)**: 4 puntos de apoyo coplanares formados por 2 ruedas cilíndricas lisas de tracción ($\varnothing 34.0\text{ mm}$) y 2 bolas de rodamiento omnidireccionales de acero ($\varnothing 8.0\text{ mm}$).

---

## 🖨️ Guía de Impresión 3D Paso a Paso

Todas las piezas se encuentran en formato `.stl` y `.step` en [`cad/stl/`](cad/stl/) y [`cad/3d_print_ready/`](cad/3d_print_ready/).

### Tabla de Piezas y Materiales:

| Pieza STL | Archivo STL | Material Recomendado | Color | Cant. | Soportes |
| :--- | :--- | :--- | :--- | :---: | :---: |
| **Cabeza Frontal** | `01_Carcasa_Cabeza_Frontal.stl` | PLA+ / PETG | Blanco Mate / Crema | 1 | No |
| **Cabeza Trasera + Orejas** | `02_Carcasa_Cabeza_Trasera.stl` | PLA+ / PETG | Blanco Mate / Crema | 1 | Solo en cuello (árbol) |
| **Chasis Torso Base** | `03_Carcasa_Torso_Chasis.stl` | PLA+ / PETG | Blanco Mate / Crema | 1 | No (apoyo plano) |
| **Tapa Superior Torso** | `04_Carcasa_Torso_Tapa.stl` | PLA+ / PETG | Blanco Mate / Crema | 1 | No |
| **Difusor LED Pecho** | `06_Difusor_Luz_Pecho.stl` | PETG Translúcido / Resina Clear | Blanco Natural / Clear | 1 | No |
| **Rueda Tracción Izq.** | `07_Rueda_Traccion_L.stl` | PLA+ o TPU 95A | Negro / Gris Oscuro | 1 | No |
| **Rueda Tracción Der.** | `08_Rueda_Traccion_R.stl` | PLA+ o TPU 95A | Negro / Gris Oscuro | 1 | No |
| **Aro Rueda Izq.** | `09_Aro_Cyan_Rueda_L.stl` | PLA | Cyan / Azul Eléctrico | 1 | No |
| **Aro Rueda Der.** | `10_Aro_Cyan_Rueda_R.stl` | PLA | Cyan / Azul Eléctrico | 1 | No |

### Parámetros Óptimos de Laminación (PrusaSlicer / Bambu Studio / Cura / OrcaSlicer):
- **Altura de capa (Layer Height)**: `0.16 mm` (o `0.12 mm` para la cabeza frontal para lograr máxima suavidad en el marco de la pantalla).
- **Perímetros / Paredes**: `4 perímetros` (mínimo `1.6 mm` de grosor de pared para asegurar la firmeza de las roscas de tornillos M2).
- **Relleno (Infill)**: `20% a 25% Gyroid` o Honeycomb.
- **Capas superiores e inferiores**: 5 capas sólidas arriba, 4 abajo.
- **Soportes**: Desactivados por defecto. Si tu impresora tiene puentes limitados, activa únicamente **Tree Supports (Soportes Tipo Árbol)** con ángulo de voladizo de >60° en la carcasa trasera de la cabeza.
- **Orientación de impresión**:
  - `Carcasa_Torso_Chasis`: Base plana hacia la cama de impresión.
  - `Carcasa_Torso_Tapa`: Superficie plana de partición hacia la cama.
  - `Carcasa_Cabeza_Frontal`: Cara frontal orientada hacia arriba o cara plana de unión hacia la cama.
  - `Ruedas`: Cara externa plana asentada en la cama.

---

## 🛠️ Guía de Ensamblaje Mecatrónico Paso a Paso

```mermaid
flowchart TD
    A["1. Chasis Inferior: Insertar 2x Bolas 8mm y 2x Motores N20"] --> B["2. Batería: Colocar LiPo 500mAh en su bahía inferior"]
    B --> C["3. Mainboard: Fijar PCB con 4x M2x4mm y conectar motores"]
    C --> D["4. Pecho: Insertar difusor LED stadium en la ranura frontal"]
    D --> E["5. Torso: Cerrar Tapa y atornillar con 4x M2x10mm desde abajo"]
    E --> F["6. Cabeza: Asentar LCD 2.0'', cámara OV2640 y Headboard PCB"]
    F --> G["7. Instalar servo SG90 y parlante 1511 en carcasa trasera"]
    G --> H["8. Conectar FPC 24 pines a través del cuello rotativo"]
    H --> I["9. Cerrar cabeza con 4x M2x8mm y acoplar al servo horn"]
```

### Paso 1: Chasis Inferior y Locomoción
1. Inserta a presión las dos **bolas de rodamiento omnidireccionales de acero de $\varnothing 8.0\text{ mm}$** en sus alojamientos semiesféricos frontal y trasero del chasis base.
2. Coloca los dos **motores N20** en sus cunas laterales orientando los ejes hacia los orificios de rueda.
3. Introduce la **batería LiPo 1S de 500 mAh** ($38 \times 20 \times 7\text{ mm}$) en la bahía del suelo.

### Paso 2: Instalación de la Mainboard PCB
1. Posiciona la placa principal (`hardware/mainboard/focusbot_mainboard.kicad_pcb`) sobre los 4 pilares MH1-MH4 a $Z = 23.5\text{ mm}$.
2. Asegúrate de que el conector USB-C y el interruptor SW1 encajen perfectamente en sus aberturas traseras.
3. Atornilla la PCB con 4 tornillos M2 de 4 mm.
4. Suelda o conecta los terminales de los motores N20 a los pads `MOT_L` y `MOT_R` y el conector JST de la batería LiPo.

### Paso 3: Cierre del Torso y Difusor Frontal
1. Inserta a presión el **Difusor de luz de pecho** (`06_Difusor_Luz_Pecho.stl`) en la ranura redondeada frontal.
2. Pasa el cable plano FFC/FPC de 24 pines desde la Mainboard hacia arriba a través del conducto central de la tornamesa.
3. Coloca la **Carcasa Torso Tapa** sobre el chasis e introduce **4 tornillos M2 de 10 mm desde la parte inferior** del chasis para unir ambas mitades sólidamente.
4. Fija el brazo del servo (horn de 1 brazo) en el cajeado de la tornamesa con 2 tornillos M1.7.

### Paso 4: Ensamblaje de la Cabeza
1. En la **Carcasa Cabeza Frontal**, asienta el panel LCD ST7789V de 2.0" en su cuna interior cuidando que quede centrado con la ventana de visualización.
2. Coloca la cámara OV2640 en el cajeado superior con su lente alineado al orificio pinhole de $Z = 71.0\text{ mm}$.
3. Desliza la **Headboard PCB** verticalmente por las guías laterales hasta topar con el panel.
4. En la **Carcasa Cabeza Trasera**, monta el **micro-servo SG90** invertido (eje hacia abajo) fijándolo con 2 tornillos a sus aletas, y coloca el parlante miniatura 1511 en su alojamiento.
5. Conecta el cable FPC de 24 pines al conector ZIF de la Headboard.
6. Cierra la cabeza introduciendo **4 tornillos M2 de 8 mm** desde los avellanados de la carcasa trasera.
7. Presiona la cabeza sobre el eje estriado del servo acoplado a la tornamesa del torso.
8. Coloca a presión las ruedas de tracción y sus aros decorativos cyan en los ejes D-shaft de los motores N20.

---

## ⚡ Arquitectura Electrónica y PCBs

FocusBot cuenta con dos placas de circuito impreso diseñadas en KiCad 10 con **0 violaciones de DRC y 0 conexiones huérfanas**:

<p align="center">
  <img src="docs/images/mainboard_3d.png" alt="Mainboard 3D" width="48%">&nbsp;
  <img src="docs/images/headboard_3d.png" alt="Headboard 3D" width="48%">
</p>

### Distribución de Pines del ESP32-S3:

| Periférico | Función | Pines ESP32-S3 | Bus / Protocolo |
| :--- | :--- | :--- | :--- |
| **Pantalla LCD 2.0"** | ST7789V IPS (320x240) | MOSI: `GPIO35`, SCLK: `GPIO36`, DC: `GPIO47`, CS: `GPIO48` | SPI a 40 MHz |
| **Cámara IA** | OV2640 2MP DVP | SIOC: `GPIO4`, SIOD: `GPIO5`, XCLK: `GPIO6`, PCLK: `GPIO7`, HREF: `GPIO14`, VSYNC: `GPIO43`, D0-D7: `GPIO37,38,39,40,41,42,2,1` | DVP 8-bit + SCCB |
| **Audio DAC** | MAX98357A (Parlante) | BCLK: `GPIO12`, WS: `GPIO11`, DIN: `GPIO10` | I2S Digital |
| **Micrófono MEMS** | INMP441 (Voz / Caricias) | BCLK: `GPIO12`, WS: `GPIO11`, DOUT: `GPIO13` | I2S Digital |
| **Motores DC** | DRV8833 Dual H-Bridge | L_IN1: `GPIO15`, L_IN2: `GPIO16`, R_IN1: `GPIO17`, R_IN2: `GPIO18` | PWM 20 kHz (LEDC) |
| **Servo Cuello** | SG90 Pan (Giro Horiz.) | PWM: `GPIO21` | Servo 50 Hz |
| **Sensor Batería** | Divisor 1:2 LiPo | ADC: `GPIO44` | ADC1 Canal 3 |
| **Puerto Nativo USB** | Flasheo / JTAG / CDC | D-: `GPIO19`, D+: `GPIO20` | USB 2.0 FS Nativo |

---

## 💻 Guía de Carga de Firmware

El ESP32-S3-WROOM-1 cuenta con **controlador USB nativo en silicio**. No necesitas adaptadores UART externos (CH340/CP2102); la placa se programa directamente con un cable USB-C estándar conectado a la PC.

### Requisitos Previos:
1. Instalar [Visual Studio Code](https://code.visualstudio.com/).
2. Instalar la extensión **PlatformIO IDE** en VS Code (o instalar PlatformIO CLI vía `pip install platformio`).
3. Clonar este repositorio y abrir la carpeta del proyecto en VS Code.

### Compilación y Subida:

1. Abre una terminal en la raíz del proyecto y entra en `firmware`:
   ```powershell
   cd firmware
   ```

2. **Compilar el código**:
   ```powershell
   pio run
   ```
   *El sistema descargará automáticamente el toolchain de Espressif, FreeRTOS y las librerías necesarias.*

3. **Cargar el firmware en el robot**:
   Conecta FocusBot a la PC mediante el cable USB-C, enciende el interruptor SW1 y ejecuta:
   ```powershell
   pio run -t upload
   ```

4. **Abrir monitor serie para ver diagnósticos**:
   ```powershell
   pio device monitor -b 115200
   ```

> [!TIP]
> **Modo Bootloader Manual (en caso de puerto bloqueado):**
> Si el puerto COM no responde al flashear: mantén presionado el botón `BOOT` (SW_BOOT en la PCB), presiona y suelta el botón `RESET`, luego suelta `BOOT`. El ESP32-S3 entrará en modo de descarga forzado por hardware.

---

## 🧠 Configuración de la IA y APIs de LLM

FocusBot puede interactuar con Inteligencia Artificial conversacional a través de dos métodos:

### Método 1: App Móvil Companion por Bluetooth (Recomendado)
El robot actúa como periférico Bluetooth Low Energy (`FocusBot-Companion`). La aplicación en el teléfono procesa el audio con reconocimiento de voz y llama a la API de tu modelo preferido (ChatGPT, Claude o Gemini). La app retransmite el texto al robot, el cual parsea automáticamente las etiquetas de actuación.

### Método 2: Wi-Fi Directo en el Microcontrolador
Si deseas que el propio robot se conecte a internet y llame a las APIs directamente:

1. Ve a la carpeta `firmware/include/`.
2. Copia el archivo de ejemplo:
   ```powershell
   Copy-Item firmware/include/credentials.h.example firmware/include/credentials.h
   ```
3. Abre `firmware/include/credentials.h` y coloca tus credenciales:
   ```cpp
   // Red Wi-Fi
   #define FOCUSBOT_WIFI_SSID       "Mi_Casa_WiFi"
   #define FOCUSBOT_WIFI_PASSWORD   "MiContrasenaSegura"

   // Proveedor seleccionado ("OPENAI" | "GEMINI" | "CLAUDE" | "OLLAMA_LOCAL")
   #define LLM_PROVIDER             "OPENAI"

   // API Keys según tu cuenta:
   #define OPENAI_API_KEY           "sk-proj-xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
   #define GEMINI_API_KEY           "AIzaSyxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
   #define CLAUDE_API_KEY           "sk-ant-api03-xxxxxxxxxxxxxxxxxxxxxxxxx"
   ```

### Personalidad del Robot (System Prompt):
El archivo [`firmware/include/focusbot_llm_persona.h`](firmware/include/focusbot_llm_persona.h) contiene el prompt del sistema que define su comportamiento:
- **Voz y tono**: Robot cute, tierno, infantil pero inteligente, con sonidos robóticos cortos (*"bip bop!"*, *"whirrr"*).
- **Control de Actuación por Etiquetas**: El LLM emite etiquetas entre corchetes que el firmware traduce en hardware físico instantáneamente:

```text
[HAPPY]   -> Ojos cyan felices y alegres
[GLEE]    -> Arcos gigantes de felicidad extrema + sacudida de cuerpo
[AWE]     -> Ojos enormes brillantes de asombro + inclinación de cabeza
[WINK]    -> Guiño cómplice de un solo ojo
[HEART]   -> Ojos de corazones magenta + ronroneo
[WORRIED] -> Ojos preocupados (alerta cuando el usuario abandona la mesa)
[ANGRY]   -> Ojos rojos y ceño fruncido (alerta de procrastinación/celular)
[NOD]     -> Movimiento vertical del cuello diciendo "Sí"
[SHAKE]   -> Movimiento horizontal del cuello diciendo "No"
[TILT]    -> Inclinación lateral curiosa de la cabeza
[WIGGLE]  -> Bailecito de izquierda a derecha con las ruedas
[SPIN]    -> Giro de 360 grados de festejo
```

---

## 👀 Expresiones de Ojos y Movimientos Cutes

El motor visual procedural ([`display_engine.cpp`](firmware/src/display_engine.cpp)) implementa **31 estados emocionales** con renderizado multi-capa a 50 Hz en la pantalla IPS:

<p align="center">
  <img src="docs/images/renderfocusbotdefrente.jpg" alt="FocusBot Face Display" width="60%">
</p>

- **Efecto Glow Multi-capa**: Halo exterior atenuado, anillo medio difuso, núcleo cyan brillante, reflejos especulares orgánicos dobles y pupila oscura interior.
- **Seguimiento Facial en Tiempo Real**: La cámara OV2640 calcula el centroide de movimiento del usuario y los ojos lo siguen suavemente en los ejes X e Y (`setLookTarget(nx, ny)`).
- **Detector de Caricias (Petting Sensor)**: Al tocar suavemente la cabeza del robot, el micrófono MEMS detecta el patrón acústico de toque y activa ronroneo sintetizado con ojos de corazón.
- **Milestones de Concentración**: Cada 25 minutos de estudio continuo sin distracciones, el robot reproduce una fanfarria triunfal, sus ojos se tornan verde esmeralda brillante (`ILLUM_GREEN_WIN`) y ejecuta un giro de victoria (`spinJoy()`).

---

## 📋 Lista de Materiales (BOM)

### Electrónica y Actuadores:
| Ref | Componente | Descripción | Cant. | Formato / Encapsulado |
| :--- | :--- | :--- | :---: | :--- |
| **U1** | ESP32-S3-WROOM-1 | MCU Dual-Core 240MHz, 16MB Flash, 8MB PSRAM | 1 | SMD Module |
| **U6** | DRV8833PWP | Driver Motores Dual H-Bridge 1.5A RMS | 1 | HTSSOP-16 |
| **U2** | TP4056 | Cargador Lineal de Batería LiPo 1A USB-C | 1 | SOP-8-PP |
| **U3/U4** | DW01A + FS8205A | Protección de Sobrecarga y Sobredescarga LiPo | 1 | SOT-23-6 / TSSOP-8 |
| **U5** | SY8089AAAC | Regulador Step-Down Conmutado 2A 3.3V | 1 | SOT-23-5 |
| **U7** | MAX98357A | Amplificador de Audio I2S Clase D 3.2W | 1 | QFN-16 |
| **U8** | MPU-6050 | IMU 6-DOF (Giroscopio + Acelerómetro) | 1 | QFN-24 |
| **M1, M2** | Motores N20 | Motorreductor metálico DC 6V 300 RPM con eje D-shaft | 2 | N20 Estándar |
| **SRV1** | Servo SG90 | Micro-servo 9g para paneo horizontal de cuello | 1 | Cuna cabeza |
| **DISP1**| ST7789V 2.0" IPS | Panel LCD 240x320 SPI 3.3V bare panel | 1 | Marco frontal |
| **CAM1** | Cámara OV2640 | Módulo sensor de cámara 2.0 MP DVP | 1 | Cuna coaxial |
| **MIC1** | INMP441 | Micrófono MEMS digital omnidireccional I2S | 1 | Headboard |
| **SPK1** | Parlante 1511 | Micro-parlante rectangular 8 Ohm 1W (15x11x3.5 mm) | 1 | Cavidad trasera |
| **BAT1** | Batería LiPo 1S | Batería recargable 3.7V 500 mAh (38x20x7 mm) | 1 | Bahía suelo |
| **J_USB** | Conector USB-C | Receptáculo USB Tipo C 16 Pines | 1 | Mid-Mount SMD |

### Tornillería y Mecánica:
| Tipo | Especificación | Uso | Cant. |
| :--- | :--- | :--- | :---: |
| **Tornillo M2** | M2 x 8 mm Cabeza Cilíndrica Allen | Cierre de carcasa de cabeza (trasera) | 4 |
| **Tornillo M2** | M2 x 10 mm Cabeza Cilíndrica Allen | Cierre de chasis torso a tapa (desde abajo) | 4 |
| **Tornillo M2** | M2 x 4 mm Cabeza Alomada | Fijación de Mainboard PCB a standoffs | 4 |
| **Tornillo M1.7**| M1.7 x 4 mm Autorroscante | Sujeción de aletas de servo SG90 | 2 |
| **Bolas Caster**| $\varnothing 8.0\text{ mm}$ Acero Inoxidable | Apoyo omnidireccional delantero y trasero | 2 |
| **Cable FPC** | FPC 24 Pines Paso 0.5 mm (100 mm) | Interconexión entre Mainboard y Headboard | 1 |

---

## 🌐 Licencia y Comunidad

FocusBot es un proyecto de hardware y software libre bajo las siguientes licencias:
- **Hardware**: [CERN-OHL-S v2 (Strongly Reciprocal)](https://ohwr.org/cern_ohl_s_v2.txt)
- **Firmware y Software**: [Licencia MIT](LICENSE)
- **Documentación y Gráficos**: [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/)

¿Quieres colaborar? Puedes aportar mejoras al enrutamiento de PCB, soporte para Micro-ROS, modelos de visión neuronal ligera o accesorios personalizados para el chasis mediante Issues y Pull Requests.

---

<p align="center">
  <b>Diseñado con ingeniería de precisión para la comunidad maker y de robótica libre.</b><br>
  <sub>Mantenido por 8aChristian y colaboradores.</sub>
</p>
