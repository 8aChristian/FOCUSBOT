import FreeCAD

doc = FreeCAD.open(r"c:\Users\Christian Ochoa\Documents\antigravity\goofy-borg\cad\goofy_robot_case.FCStd")
head_r = doc.getObject("Carcasa_Cabeza_Trasera").Shape

# Find all cylindrical holes in head_r
cyl_holes = []
for f in head_r.Faces:
    surf = f.Surface
    if "Cylinder" in str(type(surf)):
        bb = f.BoundBox
        cyl_holes.append((surf.Radius, bb.XMin, bb.XMax, bb.YMin, bb.YMax, bb.ZMin, bb.ZMax))

print(f"Total cylindrical surfaces in head_r: {len(cyl_holes)}")
# Print those near the bottom (Z < 45)
for r, xmin, xmax, ymin, ymax, zmin, zmax in cyl_holes:
    if zmin < 45.0:
        print(f"  R={r:.2f}, X=[{xmin:.2f}, {xmax:.2f}], Y=[{ymin:.2f}, {ymax:.2f}], Z=[{zmin:.2f}, {zmax:.2f}]")
