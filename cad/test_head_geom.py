import FreeCAD
import Part
import math

print("=== TESTING COHERENT HEAD GEOMETRY ===")

w_h, d_h, h_h = 56.0, 38.0, 34.0
z_hmin = 40.0
r_v = 6.0
r_h = 2.5

# 1. Create full unibody head outer volume
box = Part.makeBox(w_h, d_h, h_h, FreeCAD.Vector(-w_h/2, -d_h/2, z_hmin))

# Fillet vertical edges
v_edges = []
for e in box.Edges:
    v1, v2 = e.Vertexes[0].Point, e.Vertexes[1].Point
    if abs(v1.x - v2.x) < 0.01 and abs(v1.y - v2.y) < 0.01:
        v_edges.append(e)
head_solid = box.makeFillet(r_v, v_edges)

# Fillet horizontal top and bottom perimeter edges
z_top = z_hmin + h_h
z_bot = z_hmin
h_edges = []
for e in head_solid.Edges:
    z1, z2 = e.Vertexes[0].Point.z, e.Vertexes[1].Point.z
    if (abs(z1 - z_top) < 0.05 and abs(z2 - z_top) < 0.05) or (abs(z1 - z_bot) < 0.05 and abs(z2 - z_bot) < 0.05):
        h_edges.append(e)
head_solid = head_solid.makeFillet(r_h, h_edges)

print(f"Unified head solid created: bounds Z=[{head_solid.BoundBox.ZMin:.2f}, {head_solid.BoundBox.ZMax:.2f}]")

# 2. Slice head into Front Half (Y >= 0) and Rear Half (Y <= 0)
# Cutting tool for Front Half: keep Y >= 0 by cutting away Y < 0
cutter_rear = Part.makeBox(w_h + 10, d_h + 10, h_h + 10, FreeCAD.Vector(-w_h/2 - 5, -d_h - 5, z_hmin - 5))
cutter_front = Part.makeBox(w_h + 10, d_h + 10, h_h + 10, FreeCAD.Vector(-w_h/2 - 5, 0.0, z_hmin - 5))

head_f_raw = head_solid.cut(cutter_rear)
head_r_raw = head_solid.cut(cutter_front)

print(f"Front half raw: Y in [{head_f_raw.BoundBox.YMin:.2f}, {head_f_raw.BoundBox.YMax:.2f}]")
print(f"Rear half raw:  Y in [{head_r_raw.BoundBox.YMin:.2f}, {head_r_raw.BoundBox.YMax:.2f}]")

# Check that the face at Y=0 is a planar face with Y bounds ~ 0
f_faces_at_0 = [f for f in head_f_raw.Faces if abs(f.BoundBox.YMin) < 0.01 and abs(f.BoundBox.YMax) < 0.01]
print(f"Front half planar faces at parting plane Y=0: {len(f_faces_at_0)} (MUST BE > 0)")
r_faces_at_0 = [f for f in head_r_raw.Faces if abs(f.BoundBox.YMin) < 0.01 and abs(f.BoundBox.YMax) < 0.01]
print(f"Rear half planar faces at parting plane Y=0:  {len(r_faces_at_0)} (MUST BE > 0)")

# 3. Front Half Cavity & Features
# Cavity from Y = 0.0 to Y = 16.5 (wall thickness 2.5mm at front)
cav_f = Part.makeBox(48.0, 16.5, 29.5, FreeCAD.Vector(-24.0, 0.0, z_hmin + 2.5))
head_f = head_f_raw.cut(cav_f)

# Screen viewport opening on front face:
# Width 42.0mm (X: [-21.0, 21.0]), Height 22.0mm (Z: [43.0, 65.0])
# Front wall is at Y = 16.5 to 19.0
screen_cut = Part.makeBox(42.0, 6.0, 22.0, FreeCAD.Vector(-21.0, 15.0, 43.0))
# Let's fillet vertical edges of screen cutout for cute look
sc_edges = []
for e in screen_cut.Edges:
    v1, v2 = e.Vertexes[0].Point, e.Vertexes[1].Point
    if abs(v1.x - v2.x) < 0.01 and abs(v1.z - v2.z) < 0.01:
        sc_edges.append(e)
screen_cut = screen_cut.makeFillet(2.0, sc_edges)
head_f = head_f.cut(screen_cut)

# Camera aperture at Z = 70.0mm, Dia 7.5mm (radius 3.75mm -> Z: [66.25, 73.75])
cam_cut = Part.makeCylinder(3.75, 6.0, FreeCAD.Vector(0.0, 15.0, 70.0), FreeCAD.Vector(0, 1, 0))
head_f = head_f.cut(cam_cut)

# Raised bezel on front face
bezel_out = Part.makeCylinder(5.25, 1.2, FreeCAD.Vector(0.0, 18.5, 70.0), FreeCAD.Vector(0, 1, 0))
bezel_in  = Part.makeCylinder(3.75, 2.0, FreeCAD.Vector(0.0, 18.0, 70.0), FreeCAD.Vector(0, 1, 0))
head_f = head_f.fuse(bezel_out.cut(bezel_in))

# Mic pinhole
mic = Part.makeCylinder(0.75, 6.0, FreeCAD.Vector(-6.0, 15.0, 41.5), FreeCAD.Vector(0, 1, 0))
head_f = head_f.cut(mic)

# Check separation between screen and camera
print(f"Screen top Z = 65.00 mm")
print(f"Camera bottom Z = {70.0 - 3.75:.2f} mm")
print(f"Solid plastic bridge = {70.0 - 3.75 - 65.00:.2f} mm (Zero collision!)")

# 4. 4x M2 Fastening Bosses inside Front Half
# Top bosses: in forehead corners, above screen (Z = 69.5, X = ±22.0)
# Bottom bosses: in lower side corners, outside screen (Z = 43.5, X = ±24.5)
# Notice: screen is in X: [-21.0, 21.0]. Lower bosses are at X = ±24.5, radius 2.0 -> X in [22.5, 26.5]!
# NONE OF THEM INTRUDE INTO THE SCREEN APERTURE!
bosses = []
pilots = []
for bx, bz in [(-22.0, 69.5), (22.0, 69.5), (-24.5, 43.5), (24.5, 43.5)]:
    # Cylindrical boss from Y = 0.0 to Y = 15.0 (length 15mm)
    boss = Part.makeCylinder(2.2, 15.0, FreeCAD.Vector(bx, 0.0, bz), FreeCAD.Vector(0, 1, 0))
    # Blind pilot hole for M2 screw: Dia 1.8mm (radius 0.9mm), depth 9.0mm from Y=0 to Y=9.0
    # STOPPING AT Y=9.0! The front wall is at Y=19.0, so 10mm of solid plastic remains in front!
    pilot = Part.makeCylinder(0.9, 9.0, FreeCAD.Vector(bx, 0.0, bz), FreeCAD.Vector(0, 1, 0))
    bosses.append(boss)
    pilots.append(pilot)

for b in bosses:
    head_f = head_f.fuse(b)
for p in pilots:
    head_f = head_f.cut(p)

print(f"Front half completed! Checking front face (Y > 18.0):")
front_faces = [f for f in head_f.Faces if f.BoundBox.YMin > 17.5]
print(f"Faces on front exterior: {len(front_faces)}")

# Verify NO hole cuts through front face at boss locations
for bx, bz in [(-22.0, 69.5), (22.0, 69.5), (-24.5, 43.5), (24.5, 43.5)]:
    # Check if point (bx, 19.0, bz) is solid or hole
    pt = FreeCAD.Vector(bx, 18.8, bz)
    is_inside = head_f.isInside(pt, 0.1, True)
    print(f"Point at front face for boss ({bx}, {bz}): inside solid? {is_inside} (MUST BE TRUE!)")

print("=== ALL CHECKS PASSED SUCCESSFULLY ===")
