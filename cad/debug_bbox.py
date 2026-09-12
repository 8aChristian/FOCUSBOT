import FreeCAD, Part

w_h, d_h, h_h = 56.0, 38.0, 34.0
z_hmin = 40.0

head_raw = Part.makeBox(w_h, d_h, h_h, FreeCAD.Vector(-w_h/2, -d_h/2, z_hmin))
v_edges = [e for e in head_raw.Edges if abs(e.Vertexes[0].Point.x - e.Vertexes[1].Point.x) < 0.01 and abs(e.Vertexes[0].Point.y - e.Vertexes[1].Point.y) < 0.01]
head_solid = head_raw.makeFillet(6.0, v_edges)

z_top, z_bot = z_hmin + h_h, z_hmin
h_edges = [e for e in head_solid.Edges if (abs(e.Vertexes[0].Point.z - z_top) < 0.05 and abs(e.Vertexes[0].Point.z - z_top) < 0.05) or (abs(e.Vertexes[0].Point.z - z_bot) < 0.05 and abs(e.Vertexes[0].Point.z - z_bot) < 0.05)]
head_solid = head_solid.makeFillet(2.5, h_edges)

print(f"head_solid Y: [{head_solid.BoundBox.YMin:.2f}, {head_solid.BoundBox.YMax:.2f}]")

cutter_rear = Part.makeBox(w_h + 20, 50, h_h + 20, FreeCAD.Vector(-w_h/2 - 10, -50, z_hmin - 10))
print(f"cutter_rear Y: [{cutter_rear.BoundBox.YMin:.2f}, {cutter_rear.BoundBox.YMax:.2f}]")

head_f = head_solid.cut(cutter_rear)
print(f"head_f after cut cutter_rear: Y=[{head_f.BoundBox.YMin:.4f}, {head_f.BoundBox.YMax:.4f}]")
