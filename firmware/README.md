# FocusBot v12.0 — Production Firmware

Complete ESP32-S3 firmware for FocusBot: AI Desk Companion & Productivity Robot.

---

## 1. Guía Crítica de Flasheo: ¿Antes o Después de Soldar?

> [!IMPORTANT]
> **EL FIRMWARE SE SUBE DESPUÉS DE SOLDAR.**
> 
> **¿Por qué?**
> 1. **USB-C Nativo:** El ESP32-S3-WROOM-1 cuenta con controlador USB Serial/JTAG integrado en hardware (`GPIO19: D-` y `GPIO20: D+`). La placa principal de FocusBot ya tiene conectadas estas líneas directamente al puerto USB-C de la placa.
> 2. **ROM Bootloader en Silicio:** El chip integra una ROM interna que se activa automáticamente al conectar el cable USB-C. No necesitas zócalos de programación, pines pogo ni programadores externos (JTAG/FTDI).
> 3. **Flujo de Producción:**
>    - Se ensambla/suelda toda la PCB con sus componentes (o se recibe armada por JLCPCB SMT).
>    - Se conecta el robot a la PC con un cable USB-C estándar.
>    - La PC detecta el puerto `USB JTAG/serial debug unit (COMx)`.
>    - Se ejecuta `pio run -t upload` o se sube desde Arduino IDE / VS Code.

---

## 2. Asignación de Pines (Pinout Map)

| Periférico | Pin ESP32-S3 | Función | Protocolo / Configuración |
| :--- | :--- | :--- | :--- |
| **ST7789 IPS LCD** | GPIO35, 36, 47, 48 | MOSI, SCLK, DC, CS | SPI @ 40MHz, 240x320 px |
| **OV2640 AI Camera** | GPIO4, 5, 6, 7, 14, 43, 37..42, 2, 1 | DVP Bus + SCCB | 8-bit Parallel + I2C SCCB @ 20MHz |
| **MAX98357A Audio DAC** | GPIO12, 11, 10 | BCLK, WS, DIN | I2S DMA Audio TX (Synth / Chimes) |
| **INMP441 MEMS Mic** | GPIO12, 11, 13 | BCLK, WS, DOUT | I2S Audio RX (VAD & Voice Ear) |
| **DRV8833 Motors** | GPIO15, 16, 17, 18 | M1_IN1, M1_IN2, M2_IN1, M2_IN2 | LEDC PWM @ 20kHz (Inaudible) |
| **SG90 Pan Servo** | GPIO21 | PWM Cuello | 50Hz PWM (35° a 145°) |
| **MPU-6050 IMU** | GPIO8, 9 | SDA, SCL | I2C Fast Mode @ 400kHz |
| **LiPo Battery Sense** | GPIO44 | ADC Voltage Sense | Divisor 1:2 (3.2V a 4.2V) |

---

## 3. Modos de Operación y Comandos de Voz

1. **Secuencia Wake-Up (Arranque):**
   - Autodiagnóstico en pantalla ST7789 comprobando Display, Cámara, Audio, Micrófono, Motores, Servo y Batería.
   - Apertura procedural de ojos con brillo cyan y acorde ascendente C5-E5-G5-C6.

2. **Default Explore Mode:**
   - Paseo autónomo y suave por el escritorio (DRV8833 con PWM ultrasónico a 20 kHz sin silbidos).
   - Movimiento curioso de cuello (SG90) y animación procedural de ojos con parpadeos naturales y sacadas de mirada.
   - Escucha activa constante con micrófono digital INMP441.

3. **Focus Mode (IA Anti-Procrastinación):**
   - Monitoreo en tiempo real por visión con cámara OV2640.
   - Detección de presencia del usuario y nivel de enfoque.
   - Alerta sonora y ojos de disgusto/alerta cuando el usuario se ausenta o procrastina con el celular.
   - Puntuación de sesión de trabajo productiva (*Focus Score*).

4. **Connected LLM Companion:**
   - Emparejamiento BLE con app móvil (iOS/Android).
   - Puente de voz y texto con modelos de lenguaje (OpenAI ChatGPT, Google Gemini, Anthropic Claude).
   - Expresiones faciales sincronizadas con las respuestas del asistente.

5. **Micro-Features:**
   - **Reconocimiento de caricias (*Petting Sensor*):** Al tocar suavemente la cabeza de FocusBot, el micrófono detecta el pulso de impacto mecánico y responde con caritas de corazones (`EXPR_HEART`) y sonido de ronroneo (`SND_PETTED_PURR`).
