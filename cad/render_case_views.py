import FreeCAD
import TechDraw
import Part
import os

fcstd_path = r"c:\Users\Christian Ochoa\Documents\antigravity\goofy-borg\cad\goofy_robot_case.FCStd"
svg_out = r"c:\Users\Christian Ochoa\Documents\antigravity\goofy-borg\cad\case_orthographic_drawing.svg"

print(">>> Generating Case Technical Drawing in FreeCAD...", flush=True)

doc = FreeCAD.open(fcstd_path)

page = doc.addObject('TechDraw::DrawPage', 'Page')
template = doc.addObject('TechDraw::DrawSVGTemplate', 'Template')
template_path = os.path.join(FreeCAD.getResourceDir(), "Mod", "TechDraw", "Templates", "A4_Landscape_blank.svg")
if os.path.exists(template_path):
    template.Template = template_path
page.Template = template

# Collect case parts
case_names = [
    "Base_Chasis_Ruedas", "Cuerpo_Principal", "Carcasa_Frontal_Cabeza",
    "Tapa_Superior_Cabeza", "Rueda_Izquierda", "Rueda_Derecha"
]
case_shapes = [doc.getObject(n).Shape for n in case_names if doc.getObject(n) is not None]
compound = Part.makeCompound(case_shapes)
comp_obj = doc.addObject("Part::Feature", "CaseAssemblyCompound")
comp_obj.Shape = compound

# 1. Front View (Looking at face: +Y -> -Y)
vf = doc.addObject('TechDraw::DrawViewPart', 'ViewFront')
vf.Source = [comp_obj]
vf.Direction = FreeCAD.Vector(0, -1, 0)
vf.X = 65.0
vf.Y = 110.0
vf.Scale = 1.0
page.addView(vf)

# 2. Side View (Looking at side: +X -> -X)
vs = doc.addObject('TechDraw::DrawViewPart', 'ViewSide')
vs.Source = [comp_obj]
vs.Direction = FreeCAD.Vector(-1, 0, 0)
vs.X = 145.0
vs.Y = 110.0
vs.Scale = 1.0
page.addView(vs)

# 3. Top View (Looking down: +Z -> -Z)
vt = doc.addObject('TechDraw::DrawViewPart', 'ViewTop')
vt.Source = [comp_obj]
vt.Direction = FreeCAD.Vector(0, 0, -1)
vt.X = 225.0
vt.Y = 110.0
vt.Scale = 1.0
page.addView(vt)

# 4. Isometric / Perspective View (Axonometric)
v_iso = doc.addObject('TechDraw::DrawViewPart', 'ViewIsometric')
v_iso.Source = [comp_obj]
v_iso.Direction = FreeCAD.Vector(1, -1, 1)
v_iso.X = 225.0
v_iso.Y = 45.0
v_iso.Scale = 0.8
page.addView(v_iso)

doc.recompute()
TechDraw.writeSVGPage(page, svg_out)
print(f"TechDraw Case Drawing successfully written to: {svg_out}", flush=True)
