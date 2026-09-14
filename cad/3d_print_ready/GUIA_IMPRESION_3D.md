# Guía de Impresión 3D: FocusBot / Goofy (Piezas Separadas y Orientadas)

Todas las piezas han sido **pre-orientadas mecánicamente** con su cara plana sobre la cama ($Z=0.00\text{ mm}$) y centradas en $(0, 0)$. **No requieren rotación manual ni soportes internos**.

## Parámetros Recomendados de Laminado (Slicer)

- **Altura de Capa (Layer Height)**: 0.16 mm (0.12 mm para Visor y Aros Cyan)
- **Paredes / Perímetros (Walls)**: 4 paredes (mínimo 1.6 mm para torque de tornillos M2 y ejes)
- **Relleno (Infill)**: 20% - 25% Gyroid o Grid
- **Soportes (Supports)**: **DESACTIVADOS (None)** para piezas orientadas sobre cara de partición
- **Materiales Recomendados**:
  - Carcasa y Tapa: PLA+ o PETG (Blanco Crema / Color deseado)
  - Visor: PLA Negro
  - Ruedas: TPU 95A (o PLA con aro de goma/silicona)
  - Aros Ruedas: PLA Cyan / Azul eléctrico
  - Difusor Pecho: PETG transparente o Resina translúcida

## Manifiesto de Piezas

| Archivo | Material / Carpeta | Dimensiones (X x Y x Z) | Orientación y Notas |
| :--- | :--- | :--- | :--- |
| `01_Carcasa_Cabeza_Frontal` | `cuerpo` | 57.7 x 35.0 x 19.8 mm | Cara de partición plana en la cama. Cavidad interna hacia arriba (Cero soportes en cuna LCD). |
| `02_Carcasa_Cabeza_Trasera` | `cuerpo` | 57.7 x 39.4 x 19.8 mm | Cara de partición plana en la cama. Orejas y alojamiento servo hacia arriba (Cero soportes internos). |
| `03_Carcasa_Torso_Chasis` | `cuerpo` | 81.0 x 78.0 x 21.0 mm | Fondo plano del chasis en la cama. Cunas N20 y torretas PCB hacia arriba (Cero soportes internos). |
| `04_Carcasa_Torso_Tapa` | `cuerpo` | 81.0 x 78.0 x 16.5 mm | Reborde perimetral plano de partición en la cama. Tornamesa de cuello hacia arriba. |
| `06_Difusor_Luz_Pecho` | `difusor` | 17.6 x 2.8 x 2.0 mm | Plana en la cama. Imprimir en PETG transparente o resina translúcida. |
| `07_Rueda_Traccion_L` | `ruedas` | 34.0 x 34.0 x 9.1 mm | Cilíndrica plana como disco en la cama. Capas concéntricas para máxima resistencia. |
| `08_Rueda_Traccion_R` | `ruedas` | 34.0 x 34.0 x 9.1 mm | Cilíndrica plana como disco en la cama. Capas concéntricas para máxima resistencia. |
| `09_Aro_Cyan_Rueda_L` | `cyan` | 24.4 x 24.4 x 1.4 mm | Aro plano en la cama (Espesor 1.4mm). |
| `10_Aro_Cyan_Rueda_R` | `cyan` | 24.4 x 24.4 x 1.4 mm | Aro plano en la cama (Espesor 1.4mm). |
