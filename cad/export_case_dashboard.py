import FreeCAD
import TechDraw
import Part
import os

print(">>> [DASHBOARD RENDERER v12] Exporting Goofy Case 2D Technical Views...", flush=True)

fcstd_file = r"c:\Users\Christian Ochoa\Documents\antigravity\goofy-borg\cad\goofy_robot_case.FCStd"
doc = FreeCAD.open(fcstd_file)

exterior_names = [
    "Carcasa_Cabeza_Frontal", "Carcasa_Cabeza_Trasera", "Visor_Frontal_2Pulgadas",
    "Carcasa_Torso_Chasis", "Carcasa_Torso_Tapa", "Bola_Rodamiento_Frontal", "Bola_Rodamiento_Trasera",
    "Difusor_Luz_Pecho", "Rueda_Traccion_L", "Rueda_Traccion_R", "Aro_Cyan_Rueda_L", "Aro_Cyan_Rueda_R"
]

all_shapes = []
for name in exterior_names:
    o = doc.getObject(name)
    if o and hasattr(o, "Shape"):
        all_shapes.append(o.Shape)

compound = Part.makeCompound(all_shapes)

# View directions
dir_front = FreeCAD.Vector(0, 1, 0)
dir_side  = FreeCAD.Vector(-1, 0, 0)
dir_rear  = FreeCAD.Vector(0, -1, 0)
dir_iso   = FreeCAD.Vector(1, 1.2, 1)

svg_front = TechDraw.projectToSVG(compound, dir_front)
svg_side  = TechDraw.projectToSVG(compound, dir_side)
svg_rear  = TechDraw.projectToSVG(compound, dir_rear)
svg_iso   = TechDraw.projectToSVG(compound, dir_iso)

dashboard_svg = f'''<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1200 920" width="1200" height="920" style="background:#070b14; font-family: 'Segoe UI', Inter, -apple-system, sans-serif;">
  <defs>
    <linearGradient id="headerGrad" x1="0%" y1="0%" x2="100%" y2="0%">
      <stop offset="0%" stop-color="#0284c7" />
      <stop offset="50%" stop-color="#38bdf8" />
      <stop offset="100%" stop-color="#a855f7" />
    </linearGradient>
    <linearGradient id="badgeGrad" x1="0%" y1="0%" x2="100%" y2="100%">
      <stop offset="0%" stop-color="#00f2fe" />
      <stop offset="100%" stop-color="#4facfe" />
    </linearGradient>
  </defs>

  <!-- Title Header Bar -->
  <rect x="20" y="20" width="1160" height="72" rx="14" fill="#0f172a" stroke="#1e293b" stroke-width="1.5"/>
  <circle cx="54" cy="56" r="16" fill="#0284c7"/>
  <path d="M 47 56 L 52 61 L 61 50" stroke="#ffffff" stroke-width="2.8" fill="none" stroke-linecap="round" stroke-linejoin="round"/>
  <text x="82" y="48" fill="#f8fafc" font-size="20" font-weight="700">GOOFY ROBOT CASE v12.0: DFA Top-Down Assembly, Smooth Wheels &amp; Visible Fasteners</text>
  <text x="82" y="71" fill="#94a3b8" font-size="13">Bañera Monocasco + Tapa Superior • Ruedas Lisas Ø34mm • 4x M2 Cabeza Traseros Visibles • Altura 8.0 cm</text>
  
  <rect x="920" y="34" width="240" height="44" rx="10" fill="url(#badgeGrad)"/>
  <text x="935" y="61" fill="#0f172a" font-size="13" font-weight="900">DFA TOP-DOWN • Z=24.5mm</text>

  <!-- Panel 1: Isometric 3D Assembly -->
  <rect x="20" y="108" width="560" height="380" rx="14" fill="#0f172a" stroke="#1e293b" stroke-width="1.2"/>
  <rect x="35" y="123" width="8" height="20" rx="4" fill="#38bdf8"/>
  <text x="52" y="139" fill="#f8fafc" font-size="16" font-weight="700">Vista Isométrica 3D (Ruedas Lisas &amp; Ensamble Top-Down)</text>
  <g transform="translate(300, 315) scale(3.1, -3.1)" stroke="#38bdf8" stroke-width="0.35" fill="none">
    {svg_iso}
  </g>
  <rect x="35" y="438" width="530" height="36" rx="8" fill="#070b14"/>
  <text x="50" y="461" fill="#94a3b8" font-size="12">• Neumáticos lisos sin tacos • Guardabarros envolventes • Tapa superior desmontable</text>

  <!-- Panel 2: Front View (Face & Chest) -->
  <rect x="600" y="108" width="580" height="380" rx="14" fill="#0f172a" stroke="#1e293b" stroke-width="1.2"/>
  <rect x="615" y="123" width="8" height="20" rx="4" fill="#34d399"/>
  <text x="632" y="139" fill="#f8fafc" font-size="16" font-weight="700">Vista Frontal (Frente 100% Limpio, Cero Tornillos o Ranuras)</text>
  <g transform="translate(890, 320) scale(3.3, -3.3)" stroke="#34d399" stroke-width="0.35" fill="none">
    {svg_front}
  </g>
  <rect x="615" y="438" width="550" height="36" rx="8" fill="#070b14"/>
  <text x="630" y="461" fill="#94a3b8" font-size="12">• Visor enrasado LCD 2.0" • Ventana despejada sin pilares • Difusor cyan y cámara coaxial</text>

  <!-- Panel 3: Side Profile View (Height & Kinematic Alignment) -->
  <rect x="20" y="508" width="560" height="390" rx="14" fill="#0f172a" stroke="#1e293b" stroke-width="1.2"/>
  <rect x="35" y="523" width="8" height="20" rx="4" fill="#f472b6"/>
  <text x="52" y="539" fill="#f8fafc" font-size="16" font-weight="700">Vista Lateral (Partición Horizontal Torso Z=24.5mm)</text>
  
  <!-- Dimension line for 8.0 cm -->
  <line x1="485" y1="565" x2="485" y2="830" stroke="#f472b6" stroke-width="1.5" stroke-dasharray="4,2"/>
  <line x1="475" y1="565" x2="495" y2="565" stroke="#f472b6" stroke-width="1.5"/>
  <line x1="475" y1="830" x2="495" y2="830" stroke="#f472b6" stroke-width="1.5"/>
  <text x="500" y="690" fill="#f472b6" font-size="16" font-weight="bold">8.0 cm</text>
  <text x="500" y="708" fill="#94a3b8" font-size="11">Alt. Total</text>

  <!-- Parting line callout -->
  <line x1="120" y1="715" x2="440" y2="715" stroke="#ef4444" stroke-width="1.5" stroke-dasharray="6,3"/>
  <text x="350" y="705" fill="#ef4444" font-size="11" font-weight="bold">Tapa Z=24.5</text>

  <g transform="translate(275, 715) scale(3.1, -3.1)" stroke="#f472b6" stroke-width="0.35" fill="none">
    {svg_side}
  </g>
  <rect x="35" y="848" width="530" height="36" rx="8" fill="#070b14"/>
  <text x="50" y="871" fill="#94a3b8" font-size="12">• Bipartición horizontal Z=24.5mm (línea roja) • Ensamble top-down • Suelo Z=0.00mm</text>

  <!-- Panel 4: Rear View (Fasteners & Ports) -->
  <rect x="600" y="508" width="580" height="390" rx="14" fill="#0f172a" stroke="#1e293b" stroke-width="1.2"/>
  <rect x="615" y="523" width="8" height="20" rx="4" fill="#fbbf24"/>
  <text x="632" y="539" fill="#f8fafc" font-size="16" font-weight="700">Vista Trasera (Tornillos M2 Visibles, USB-C &amp; Altavoz)</text>
  <g transform="translate(890, 720) scale(3.3, -3.3)" stroke="#fbbf24" stroke-width="0.35" fill="none">
    {svg_rear}
  </g>
  <rect x="615" y="848" width="550" height="36" rx="8" fill="#070b14"/>
  <text x="630" y="871" fill="#94a3b8" font-size="12">• 4x M2 avellanados cabeza trasera 100% visibles • 4x M2 tapa torso • USB-C y switch SW1</text>
</svg>'''

out_svg = r"c:\Users\Christian Ochoa\Documents\antigravity\goofy-borg\cad\goofy_case_dashboard.svg"
with open(out_svg, "w", encoding="utf-8") as f:
    f.write(dashboard_svg)
print(f"[OK] Exported updated 2D Dashboard SVG: {out_svg}", flush=True)

artifact_svg = r"C:\Users\Christian Ochoa\.gemini\antigravity\brain\f97d3ae3-cffb-48bb-b959-294df62a532c\goofy_case_dashboard.svg"
with open(artifact_svg, "w", encoding="utf-8") as f:
    f.write(dashboard_svg)
print(f"[OK] Copied Dashboard SVG to Artifacts: {artifact_svg}", flush=True)
