import FreeCAD
import TechDraw
import Part
import os

doc = FreeCAD.open(r"c:\Users\Christian Ochoa\Documents\antigravity\goofy-borg\cad\goofy_robot_assembly.FCStd")

# Create a TechDraw Page
page = doc.addObject('TechDraw::DrawPage', 'Page')
template = doc.addObject('TechDraw::DrawSVGTemplate', 'Template')
# Using default blank or built-in A4 landscape
template_path = os.path.join(FreeCAD.getResourceDir(), "Mod", "TechDraw", "Templates", "A4_Landscape_blank.svg")
if os.path.exists(template_path):
    template.Template = template_path
page.Template = template

# Collect all shapes
all_shapes = [obj.Shape for obj in doc.Objects if hasattr(obj, 'Shape') and obj.Name != 'Page' and obj.Name != 'Template']
compound = Part.makeCompound(all_shapes)
comp_obj = doc.addObject("Part::Feature", "FullAssemblyCompound")
comp_obj.Shape = compound

# 1. Front View (Y direction: camera looking from front to back, [0, -1, 0])
view_front = doc.addObject('TechDraw::DrawViewPart', 'ViewFront')
view_front.Source = [comp_obj]
view_front.Direction = FreeCAD.Vector(0, -1, 0)
view_front.X = 70.0
view_front.Y = 120.0
view_front.Scale = 1.0
page.addView(view_front)

# 2. Side View (X direction: looking from right to left, [-1, 0, 0])
view_side = doc.addObject('TechDraw::DrawViewPart', 'ViewSide')
view_side.Source = [comp_obj]
view_side.Direction = FreeCAD.Vector(-1, 0, 0)
view_side.X = 150.0
view_side.Y = 120.0
view_side.Scale = 1.0
page.addView(view_side)

# 3. Top View (Z direction: looking from top down, [0, 0, -1])
view_top = doc.addObject('TechDraw::DrawViewPart', 'ViewTop')
view_top.Source = [comp_obj]
view_top.Direction = FreeCAD.Vector(0, 0, -1)
view_top.X = 230.0
view_top.Y = 120.0
view_top.Scale = 1.0
page.addView(view_top)

doc.recompute()

svg_out = r"c:\Users\Christian Ochoa\Documents\antigravity\goofy-borg\cad\assembly_drawing.svg"
TechDraw.writeSVGPage(page, svg_out)
print(f"TechDraw SVG successfully written to: {svg_out}")
