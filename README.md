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

## 📋 SuperBOM: Lista Maestra de Componentes, Enlaces y Costos

Guía de compra optimizada para **máximo ahorro**, priorizando la fabricación de placas vírgenes (*bare PCBs*) y la adquisición de componentes por separado sin ensamblaje costoso de fábrica.

---

### 1. Componentes Externos (Modulares / Sin soldar a placa)

Se conectan directamente a las placas mediante cables y conectores:

| Componente | Enlaces de Compra Directa | Rango de Precio Estimado | Formato Habitual |
| :--- | :--- | :---: | :--- |
| **Batería LiPo 3.7V 500mAh**<br>(Conector JST-PH 2.0 mm) | • [AliExpress (Opción Económica)](https://www.aliexpress.com/item/32814862683.html)<br>• [Amazon (Envío Rápido)](https://www.amazon.com/dp/B0D3F67N6F) | **$4.00 – $9.00 USD** | Unidad con circuito de protección PCM integrado |
| **Micromotores N20 metálicos**<br>(6V 300 RPM con reductora) | • [AliExpress (N20 Gearmotor)](https://www.aliexpress.com/item/1005005519903657.html) | **$1.50 – $2.50 USD** | Por unidad (se requieren 2: Izquierdo y Derecho) |
| **Cables chicote JST-SH 1.0 mm**<br>(2 pines pre-crimpeados para motores/parlante) | • [AliExpress (Cables JST-SH 1.0 mm)](https://www.aliexpress.com/item/1005011595323701.html) | **$1.50 – $2.50 USD** | Paquete de 10 a 20 cables con conector hembra |
| **Micro-servo SG90 9g**<br>(Giro horizontal de cuello) | • [Amazon (Opción 1)](https://www.amazon.com/dp/B07L2SF3R4)<br>• [Amazon (Opción Alternativa)](https://www.amazon.com/dp/B01M5LIKLQ) | **$2.50 – $4.00 USD** | Unidad individual con juego de brazos/horns |
| **Micro-Parlante 1511**<br>(8 $\Omega$ 1W, $15 \times 11 \times 3.5\text{ mm}$) | • [AliExpress (Parlante 1511 Box)](https://www.aliexpress.com/i/1680403579.html) | **$0.90 – $1.70 USD** | Unidad suelta o lote de 5 a 10 piezas ($2.50 - $3.50) |
| **Cámara OV2640**<br>(Flex cinta 24 pines paso 0.5 mm) | • [AliExpress (OV2640 Módulo Cámara)](https://www.aliexpress.com/wholesale?catId=0&SearchText=OV2640) | **$2.80 – $4.50 USD** | Módulo de cámara con lente estándar |
| **Pantalla IPS LCD ST7789 2.0"**<br>(SPI 240x320, bare panel) | • [AliExpress (ST7789 2.0" IPS)](https://www.aliexpress.com/item/1005009314410563.html)<br>• [AliExpress (Opción Alternativa)](https://www.aliexpress.com/item/1005012615180387.html) | **$3.50 – $5.50 USD** | Panel individual IPS 240x320 |

---

### 2. Elementos de Interconexión Flexible y Zócalos ZIF

| Componente | Enlaces de Compra Directa | Rango de Precio Estimado | Formato Habitual |
| :--- | :--- | :---: | :--- |
| **Cable plano flexible FPC 24 pines**<br>(Paso 0.5 mm, Tipo A o Tipo B) | • [AliExpress (FPC 24P 0.5 mm)](https://www.aliexpress.com/i/1005006038283095.html)<br>• [AliExpress (Opción Alternativa)](https://www.aliexpress.com/i/4000022157163.html) | **$0.50 – $1.00 USD** | Paquete de 2 a 5 cables |
| **Conector ZIF 24 pines 0.5 mm SMD**<br>(Seguro abatible / Bottom Contact) | • [AliExpress (ZIF 24P 0.5 mm)](https://www.aliexpress.com/i/1005004233136813.html)<br>• [AliExpress (Opción Alternativa)](https://www.aliexpress.com/item/32850399481.html) | **$1.50 – $2.50 USD** | Tira o lote de 10 conectores para placa |
| **Conector hembra USB-C SMD 16 pines**<br>(Carga y datos nativos) | • [AliExpress (USB-C 16P SMD)](https://www.aliexpress.com/i/1005006257173539.html) | **$1.70 – $2.50 USD** | Lote de 5 a 10 conectores hembra |

---

### 3. Circuitos Integrados y Semiconductores para Soldar (SMD)

Comprados sueltos o en lote económico para soldar manualmente o con pasta de estaño y aire caliente:

| Componente / Circuito Integrado | Enlace Catálogo JLCPCB / LCSC / AliExpress | Precio Unitario Estimado |
| :--- | :--- | :---: |
| **ESP32-S3-WROOM-1-N8R8 / N16R8** | • [JLCPCB (C2913201)](https://jlcpcb.com/partdetail/3198299-ESP32_S3_WROOM_1N8R8/C2913201)<br>• [LCSC (C2913201)](https://www.lcsc.com/product-detail/C2913201.html) | **$3.50 – $4.40 USD** |
| **DRV8833PWP (Driver Puente H Dual)** | • [AliExpress (Chips DRV8833)](https://www.aliexpress.com/item/1005008600208107.html)<br>• [AliExpress (Opción 2)](https://www.aliexpress.com/item/1005005236496976.html) | **$0.70 – $1.30 USD** |
| **TP4056 (Cargador LiPo)** | • [AliExpress / LCSC](https://www.aliexpress.com/item/1005008600208107.html) | **$0.15 – $0.30 USD** |
| **DW01A + FS8205A (Protección LiPo)** | • LCSC / AliExpress (SOT-23-6 / TSSOP-8) | **$0.10 – $0.25 USD** |
| **SY8089AAAC (Regulador Step-Down 2A)** | • [LCSC (C78988)](https://www.lcsc.com/product-detail/DC-DC-Converters_Silergy-Corp-SY8089AAAC_C78988.html) | **$0.20 – $0.40 USD** |
| **MAX98357AETE+T (Amplificador I2S)** | • [JLCPCB (C910544)](https://jlcpcb.com/partdetail/MaximIntegrated-MAX98357AETET/C910544)<br>• [LCSC (C910544)](https://www.lcsc.com/product-detail/C910544.html) | **$0.90 – $1.20 USD** |
| **MPU-6050 (IMU 6 Ejes)** | • JLCPCB / LCSC / AliExpress (QFN-24) | **$1.20 – $2.20 USD** |
| **INMP441 (Micrófono MEMS LGA-9)** | • [AliExpress (INMP441 SMD suelto)](https://he.aliexpress.com/item/1005007633632817.html) | **$1.50 – $3.00 USD** |
| **Componentes Pasivos y Discretos 0603**<br>(Inductor 2.2µH, AO3401A, SS14, BAT54, R, C) | • Kits surtidos 0603 en AliExpress / LCSC | **$0.50 – $1.50 USD**<br>*(costo total agrupado)* |

---

### 4. Fabricación de PCBs y Costo de Impresión 3D

| Ítem | Proveedor / Proceso | Detalle de Costo | Costo por Robot |
| :--- | :--- | :--- | :---: |
| **Mainboard PCB (Placa Virgen)** | JLCPCB / PCBWay (2 capas, FR4) | Pedido estándar de 5 placas por **$2.00 USD** | **$0.40 – $1.00 USD** |
| **Headboard PCB (Placa Virgen)** | JLCPCB / PCBWay (2 capas, FR4) | Pedido estándar de 5 placas por **$2.00 USD** | **$0.40 – $1.00 USD** |
| **Carcasa Impresa en 3D** | Impresora FDM doméstica (PLA+ / PETG / TPU) | Consumo total de filamento: ~110–130 gramos (carcasa, ruedas, difusor) a ~$18–$22/kg | **$2.00 – $3.00 USD** |
| **Tornillería y Bolas de Rodamiento** | 8x Tornillos M2 + 2x Bolas acero 8 mm | Comprados en ferretería o kit surtido M2 | **$0.80 – $1.50 USD** |

---

### 💰 Resumen de Inversión Global (Total Build Cost)

*Sin incluir gastos de envío internacional:*

| Categoría | Rango de Costo (USD) |
| :--- | :---: |
| **1. Componentes Modulares Externos** (Cámara, LCD, servo, 2 motores N20, batería LiPo, parlante y cables) | **$20.00 – $32.00 USD** |
| **2. Circuitos Integrados y Semiconductores SMD** | **$9.00 – $15.00 USD** |
| **3. Placas de Circuito Impreso Vírgenes (Mainboard + Headboard)** | **$0.80 – $2.00 USD** |
| **4. Materiales de Impresión 3D y Tornillería** | **$2.80 – $4.50 USD** |
| **COSTO TOTAL ESTIMADO POR ROBOT COMPLETO** | **$32.60 – $53.50 USD** |

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
