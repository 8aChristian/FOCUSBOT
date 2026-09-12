import FreeCAD, Part

w_h, d_h, h_h = 56.0, 38.0, 34.0
z_hmin = 40.0
r_v = 6.0
r_h = 2.5

box = Part.makeBox(w_h, d_h, h_h, FreeCAD.Vector(-w_h/2, -d_h/2, z_hmin))
v_edges = [e for e in box.Edges if abs(e.Vertexes[0].Point.x - e.Vertexes[1].Point.x) < 0.01 and abs(e.Vertexes[0].Point.y - e.Vertexes[1].Point.y) < 0.01]
head_solid = box.makeFillet(r_v, v_edges)

z_top, z_bot = z_hmin + h_h, z_hmin
h_edges = [e for e in head_solid.Edges if (abs(e.Vertexes[0].Point.z - z_top) < 0.05 and abs(e.Vertexes[1].Point.z - z_top) < 0.05) or (abs(e.Vertexes[0].Point.z - z_bot) < 0.05 and abs(e.Vertexes[1].Point.z - z_bot) < 0.05)]
head_solid = head_solid.makeFillet(r_h, h_edges)

# Cut at Y = 0
cutter_rear = Part.makeBox(w_h + 20, d_h + 10, h_h + 20, FreeCAD.Vector(-w_h/2 - 10, -(d_h + 10), z_hmin - 10))
cutter_front = Part.makeBox(w_h + 20, d_h + 10, h_h + 20, FreeCAD.Vector(-w_h/2 - 10, 0.0, z_hmin - 10))

head_f_raw = head_solid.cut(cutter_rear)
head_r_raw = head_solid.cut(cutter_front)

print(f"Front half Y bounds: [{head_f_raw.BoundBox.YMin:.2f}, {head_f_raw.BoundBox.YMax:.2f}]")
print(f"Rear half Y bounds:  [{head_r_raw.BoundBox.YMin:.2f}, {head_r_raw.BoundBox.YMax:.2f}]")

# Test candidate boss locations for front half
for bx in [-23.5, -23.0, -22.5, -22.0, -21.5]:
    for bz in [43.0, 44.0, 45.0, 46.0, 68.0, 69.0, 70.0]:
        pt_seam = FreeCAD.Vector(bx, 0.5, bz)
        pt_front = FreeCAD.Vector(bx, 17.5, bz)
        in_seam = head_f_raw.isInside(pt_seam, 0.1, True)
        in_front = head_f_raw.isInside(pt_front, 0.1, True)
        if in_seam and in_front:
            print(f"SOLID at bx={bx}, bz={bz} (seam={in_seam}, front={in_front})")
