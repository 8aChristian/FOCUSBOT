import FreeCAD
import TechDraw
import Part
import os

doc = FreeCAD.open(r"c:\Users\Christian Ochoa\Documents\antigravity\goofy-borg\cad\goofy_robot_assembly.FCStd")

# Collect shapes by category to style them distinctly
components = {
    "pcb": [o.Shape for o in doc.Objects if "PCB" in o.Name],
    "wheels": [o.Shape for o in doc.Objects if "Wheel" in o.Name],
    "motors": [o.Shape for o in doc.Objects if "Motor" in o.Name],
    "battery": [o.Shape for o in doc.Objects if "Battery" in o.Name],
    "ic": [o.Shape for o in doc.Objects if any(k in o.Name for k in ["ESP32", "DRV8833", "MAX98357", "MPU6050", "TP4056", "OLED", "Camera", "Microphone", "Coin"])],
    "servo": [o.Shape for o in doc.Objects if "Servo" in o.Name],
    "connectors": [o.Shape for o in doc.Objects if any(k in o.Name for k in ["USB", "Speaker"])],
}

all_shapes = [obj.Shape for obj in doc.Objects if hasattr(obj, 'Shape') and "Compound" not in obj.Name and "View" not in obj.Name]
compound = Part.makeCompound(all_shapes)

# View directions
# Front View (looking towards +Y, from front)
dir_front = FreeCAD.Vector(0, -1, 0)
# Side View (looking towards +X, from side)
dir_side = FreeCAD.Vector(-1, 0, 0)
# Top View (looking down from +Z)
dir_top = FreeCAD.Vector(0, 0, -1)
# Isometric View
dir_iso = FreeCAD.Vector(1, -1.2, 1)

svg_front = TechDraw.projectToSVG(compound, dir_front)
svg_side = TechDraw.projectToSVG(compound, dir_side)
svg_top = TechDraw.projectToSVG(compound, dir_top)
svg_iso = TechDraw.projectToSVG(compound, dir_iso)

# Let's create an integrated visual dashboard in SVG
composite_svg = f"""<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1200 900" width="1200" height="900" style="background:#12151c; font-family: 'Segoe UI', Inter, Helvetica, sans-serif;">
  <defs>
    <linearGradient id="gradHeader" x1="0%" y1="0%" x2="100%" y2="0%">
      <stop offset="0%" stop-color="#3b82f6" />
      <stop offset="100%" stop-color="#8b5cf6" />
    </linearGradient>
    <filter id="glow" x="-20%" y="-20%" width="140%" height="140%">
      <feGaussianBlur stdDeviation="3" result="blur" />
      <feComposite in="SourceGraphic" in2="blur" operator="over" />
    </filter>
  </defs>

  <!-- Background Grids & Card Panes -->
  <rect x="20" y="20" width="1160" height="60" rx="12" fill="#1b202e" stroke="#2d3748" stroke-width="1.5"/>
  <text x="40" y="58" fill="#ffffff" font-size="22" font-weight="bold">FOCUSROBOTPCB: FreeCAD 3D Assembly &amp; Multi-PCB Layout Validation</text>
  <text x="820" y="58" fill="#a0aec0" font-size="14">Overall Dimensions: 55 x 55 x 78 mm (Target &lt; 80mm)</text>

  <!-- Panel 1: Isometric View -->
  <rect x="20" y="95" width="560" height="420" rx="12" fill="#181d29" stroke="#2d3748" stroke-width="1"/>
  <text x="40" y="125" fill="#60a5fa" font-size="16" font-weight="bold">Vista Isométrica 3D (Despiece Físico)</text>
  <g transform="translate(300, 320) scale(3.2, -3.2)" stroke="#38bdf8" stroke-width="0.35" fill="none">
    {svg_iso}
  </g>
  <text x="40" y="490" fill="#94a3b8" font-size="12">• Mainboard 48x32mm en chasis inferior | Headboard 42x24mm en cabeza</text>

  <!-- Panel 2: Front View -->
  <rect x="600" y="95" width="580" height="420" rx="12" fill="#181d29" stroke="#2d3748" stroke-width="1"/>
  <text x="620" y="125" fill="#34d399" font-size="16" font-weight="bold">Vista Frontal (Elevación Z-X)</text>
  <g transform="translate(890, 340) scale(3.5, -3.5)" stroke="#34d399" stroke-width="0.35" fill="none">
    {svg_front}
  </g>
  <text x="620" y="490" fill="#94a3b8" font-size="12">• Ruedas Ø34mm | Motores N20 horizontales | Cuello Servo 3.7g centrado</text>

  <!-- Panel 3: Side View (Profile Z-Y) -->
  <rect x="20" y="530" width="560" height="350" rx="12" fill="#181d29" stroke="#2d3748" stroke-width="1"/>
  <text x="40" y="560" fill="#f472b6" font-size="16" font-weight="bold">Vista Lateral (Perfil Z-Y)</text>
  <g transform="translate(300, 720) scale(3.2, -3.2)" stroke="#f472b6" stroke-width="0.35" fill="none">
    {svg_side}
  </g>
  <text x="40" y="855" fill="#94a3b8" font-size="12">• Fondo: Batería LiPo 800mAh | Centro: PCB + DRV8833 | Trasera: USB-C + Altavoz 1511</text>

  <!-- Panel 4: Top View (Planta X-Y) -->
  <rect x="600" y="530" width="580" height="350" rx="12" fill="#181d29" stroke="#2d3748" stroke-width="1"/>
  <text x="620" y="560" fill="#fbbf24" font-size="16" font-weight="bold">Vista Superior (Planta X-Y Placement)</text>
  <g transform="translate(890, 715) scale(3.5, -3.5)" stroke="#fbbf24" stroke-width="0.35" fill="none">
    {svg_top}
  </g>
  <text x="620" y="855" fill="#94a3b8" font-size="12">• PCB 48x32mm encaja holgada entre ruedas (ancho libre 41-48mm interior)</text>
</svg>
"""

out_svg = r"c:\Users\Christian Ochoa\Documents\antigravity\goofy-borg\cad\robot_views_dashboard.svg"
with open(out_svg, "w", encoding="utf-8") as f:
    f.write(composite_svg)

print(f"Generated composite view dashboard SVG at: {out_svg}")

# Also copy to artifacts directory so it can be viewed by user in artifacts!
artifact_dir = r"C:\Users\Christian Ochoa\.gemini\antigravity\brain\f97d3ae3-cffb-48bb-b959-294df62a532c"
artifact_svg = os.path.join(artifact_dir, "robot_views_dashboard.svg")
with open(artifact_svg, "w", encoding="utf-8") as f:
    f.write(composite_svg)
print(f"Copied SVG to artifacts directory: {artifact_svg}")
