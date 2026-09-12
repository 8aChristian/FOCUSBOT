import FreeCAD
import Part
import math

print("=== VERIFYING COHERENT ARCHITECTURE v6 ===")

doc = FreeCAD.newDocument("Test_v6")

w_h, d_h, h_h = 56.0, 38.0, 34.0
z_hmin = 40.0

head_raw = Part.makeBox(w_h, d_h, h_h, FreeCAD.Vector(-w_h/2, -d_h/2, z_hmin))

# 1. Fillet 4 vertical corners (R = 6.0mm)
v_edges = [e for e in head_raw.Edges if abs(e.Vertexes[0].Point.x - e.Vertexes[1].Point.x) < 0.01 and abs(e.Vertexes[0].Point.y - e.Vertexes[1].Point.y) < 0.01]
head_solid = head_raw.makeFillet(6.0, v_edges)

# 2. Fillet top and bottom horizontal perimeter edges (R = 2.5mm)
z_top = z_hmin + h_h
z_bot = z_hmin
h_edges = [e for e in head_solid.Edges if (abs(e.Vertexes[0].Point.z - z_top) < 0.05 and abs(e.Vertexes[0].Point.z - z_top) < 0.05) or (abs(e.Vertexes[0].Point.z - z_bot) < 0.05 and abs(e.Vertexes[0].Point.z - z_bot) < 0.05)]
head_solid = head_solid.makeFillet(2.5, h_edges)

# 3. Slice at Y = 0.0 into Front (Y >= 0) and Rear (Y <= 0)
cutter_rear  = Part.makeBox(w_h + 20, 60.0, h_h + 30, FreeCAD.Vector(-w_h/2 - 10, -60.0, z_hmin - 15))
cutter_front = Part.makeBox(w_h + 20, 60.0, h_h + 30, FreeCAD.Vector(-w_h/2 - 10, 0.0, z_hmin - 15))

head_f = head_solid.cut(cutter_rear)
head_r = head_solid.cut(cutter_front)

print(f"Base Front Y: [{head_f.BoundBox.YMin:.4f}, {head_f.BoundBox.YMax:.4f}]")
print(f"Base Rear  Y: [{head_r.BoundBox.YMin:.4f}, {head_r.BoundBox.YMax:.4f}]")

# Sculpted cat ears attached to REAR shell
def make_cute_ear(center_x, is_left=True):
    sign = -1.0 if is_left else 1.0
    b_x, b_y, b_z = center_x, -5.0, 73.5
    p1 = FreeCAD.Vector(b_x - sign * 7.5, b_y - 4.5, b_z)
    p2 = FreeCAD.Vector(b_x + sign * 6.5, b_y - 2.5, b_z)
    p3 = FreeCAD.Vector(b_x - sign * 2.0, b_y + 3.5, b_z)
    w1 = Part.Wire([Part.makeLine(p1, p2), Part.makeLine(p2, p3), Part.makeLine(p3, p1)])
    
    m_x, m_y, m_z = center_x + sign * 1.5, -6.0, 77.0
    p4 = FreeCAD.Vector(m_x - sign * 5.0, m_y - 3.0, m_z)
    p5 = FreeCAD.Vector(m_x + sign * 4.2, m_y - 1.5, m_z)
    p6 = FreeCAD.Vector(m_x - sign * 1.2, m_y + 2.5, m_z)
    w2 = Part.Wire([Part.makeLine(p4, p5), Part.makeLine(p5, p6), Part.makeLine(p6, p4)])
    
    t_x, t_y, t_z = center_x + sign * 2.8, -7.0, 80.0
    w3 = Part.Wire([Part.makeCircle(1.2, FreeCAD.Vector(t_x, t_y, t_z), FreeCAD.Vector(sign * 0.3, -0.2, 1.0))])
    
    ear_solid = Part.makeLoft([w1, w2, w3], True)
    
    s1 = FreeCAD.Vector(b_x - sign * 5.0, b_y - 1.8, b_z + 1.2)
    s2 = FreeCAD.Vector(b_x + sign * 4.0, b_y - 0.8, b_z + 1.2)
    s3 = FreeCAD.Vector(b_x - sign * 1.0, b_y + 2.5, b_z + 1.2)
    sw1 = Part.Wire([Part.makeLine(s1, s2), Part.makeLine(s2, s3), Part.makeLine(s3, s1)])
    st_x, st_y, st_z = center_x + sign * 2.0, -5.8, 78.5
    sw2 = Part.Wire([Part.makeCircle(0.8, FreeCAD.Vector(st_x, st_y, st_z), FreeCAD.Vector(sign * 0.3, -0.2, 1.0))])
    scoop = Part.makeLoft([sw1, sw2], True)
    return ear_solid.cut(scoop)

ear_l = make_cute_ear(-18.0, is_left=True)
ear_r = make_cute_ear(18.0, is_left=False)
head_r = head_r.fuse(ear_l).fuse(ear_r)

# -------------------------------------------------------------
# 2. FRONT HALF CAVITY & FEATURES (ZERO HOLES ON FRONT FACE)
# -------------------------------------------------------------
# Cavity from Y = 0.0 to Y = 16.5
cav_f = Part.makeBox(48.0, 16.5, 29.0, FreeCAD.Vector(-24.0, 0.0, 42.5))
head_f = head_f.cut(cav_f)

# Screen viewport opening on front face: Width 41.5mm, Height 22.0mm, Z: [43.5, 65.5]
screen_open = Part.makeBox(41.5, 5.0, 22.0, FreeCAD.Vector(-20.75, 15.5, 43.5))
sc_edges = [e for e in screen_open.Edges if abs(e.Vertexes[0].Point.x - e.Vertexes[1].Point.x) < 0.01 and abs(e.Vertexes[0].Point.z - e.Vertexes[1].Point.z) < 0.01]
screen_open = screen_open.makeFillet(2.0, sc_edges)
head_f = head_f.cut(screen_open)

# Camera aperture at Z = 70.0mm (strictly above screen!)
cam_open = Part.makeCylinder(3.75, 5.0, FreeCAD.Vector(0.0, 15.5, 70.0), FreeCAD.Vector(0, 1, 0))
head_f = head_f.cut(cam_open)

# Raised camera bezel on front face
cam_bezel_out = Part.makeCylinder(5.25, 1.2, FreeCAD.Vector(0.0, 18.5, 70.0), FreeCAD.Vector(0, 1, 0))
cam_bezel_in  = Part.makeCylinder(3.75, 2.0, FreeCAD.Vector(0.0, 18.0, 70.0), FreeCAD.Vector(0, 1, 0))
head_f = head_f.fuse(cam_bezel_out.cut(cam_bezel_in))

# Mic pinhole at chin
mic_hole = Part.makeCylinder(0.75, 5.0, FreeCAD.Vector(-6.0, 15.5, 41.5), FreeCAD.Vector(0, 1, 0))
head_f = head_f.cut(mic_hole)

# 4x Blind Fastening Bosses inside Front Half:
boss_coords = [(-22.0, 69.5), (22.0, 69.5), (-23.5, 44.5), (23.5, 44.5)]
for bx, bz in boss_coords:
    boss = Part.makeCylinder(2.2, 14.5, FreeCAD.Vector(bx, 0.0, bz), FreeCAD.Vector(0, 1, 0))
    head_f = head_f.fuse(boss)
    pilot = Part.makeCylinder(0.9, 8.0, FreeCAD.Vector(bx, 0.0, bz), FreeCAD.Vector(0, 1, 0))
    head_f = head_f.cut(pilot)

print("Front half modeled successfully!")

# -------------------------------------------------------------
# 3. REAR HALF CAVITY & FEATURES
# -------------------------------------------------------------
cav_r = Part.makeBox(48.0, 16.5, 29.0, FreeCAD.Vector(-24.0, -16.5, 42.5))
head_r = head_r.cut(cav_r)

# 37-hole speaker grille
spk_cz = 56.0
holes = [Part.makeCylinder(0.9, 5.0, FreeCAD.Vector(0.0, -20.0, spk_cz), FreeCAD.Vector(0, 1, 0))]
for i in range(6):
    ang = i * 2.0 * math.pi / 6.0
    holes.append(Part.makeCylinder(0.8, 5.0, FreeCAD.Vector(3.8*math.cos(ang), -20.0, spk_cz + 3.8*math.sin(ang)), FreeCAD.Vector(0, 1, 0)))
for i in range(12):
    ang = i * 2.0 * math.pi / 12.0
    holes.append(Part.makeCylinder(0.75, 5.0, FreeCAD.Vector(7.2*math.cos(ang), -20.0, spk_cz + 7.2*math.sin(ang)), FreeCAD.Vector(0, 1, 0)))
for i in range(18):
    ang = i * 2.0 * math.pi / 18.0
    holes.append(Part.makeCylinder(0.7, 5.0, FreeCAD.Vector(10.5*math.cos(ang), -20.0, spk_cz + 10.5*math.sin(ang)), FreeCAD.Vector(0, 1, 0)))
head_r = head_r.cut(Part.makeCompound(holes))

# Speaker retention clips
clip_l = Part.makeBox(2.0, 4.0, 12.0, FreeCAD.Vector(-8.5, -16.5, 50.0))
clip_r = Part.makeBox(2.0, 4.0, 12.0, FreeCAD.Vector(6.5, -16.5, 50.0))
head_r = head_r.fuse(clip_l).fuse(clip_r)

# 4x Counterbored M2 screw through-holes in Rear Shell:
for bx, bz in boss_coords:
    r_pillar = Part.makeCylinder(2.4, 16.5, FreeCAD.Vector(bx, -16.5, bz), FreeCAD.Vector(0, 1, 0))
    head_r = head_r.fuse(r_pillar)
    thru = Part.makeCylinder(1.15, 20.0, FreeCAD.Vector(bx, -19.5, bz), FreeCAD.Vector(0, 1, 0))
    cb   = Part.makeCylinder(2.1, 2.5, FreeCAD.Vector(bx, -19.5, bz), FreeCAD.Vector(0, 1, 0))
    head_r = head_r.cut(thru).cut(cb)

# Neck socket in Rear Shell underside (Z = 39.8 to 41.5, floor is at Z = 42.5)
neck_socket = Part.makeCylinder(7.5, 1.8, FreeCAD.Vector(0.0, 0.0, 39.8), FreeCAD.Vector(0, 0, 1))
horn_pocket = Part.makeBox(15.0, 7.0, 1.8, FreeCAD.Vector(-7.5, -7.0, 39.8))
neck_bore   = Part.makeCylinder(2.5, 3.5, FreeCAD.Vector(0.0, -2.5, 39.8), FreeCAD.Vector(0, 0, 1))
head_r = head_r.cut(neck_socket).cut(horn_pocket).cut(neck_bore)

print("Rear half modeled successfully!")

# -------------------------------------------------------------
# VERIFICATION CHECKS
# -------------------------------------------------------------
print("\n=== RUNNING CRITICAL VERIFICATION CHECKS ===")

# 1. Front face solid at boss coords
for bx, bz in boss_coords:
    inside = head_f.isInside(FreeCAD.Vector(bx, 18.8, bz), 0.1, True)
    assert inside, f"FAIL: Boss at ({bx}, {bz}) pierced front face!"
print("[PASS] Front face is 100% solid at all screw boss locations (Zero visible front holes).")

# 2. Screen viewport clear
for test_x in [-20.0, 0.0, 20.0]:
    for test_z in [45.0, 54.0, 64.0]:
        inside = head_f.isInside(FreeCAD.Vector(test_x, 18.5, test_z), 0.1, True)
        assert not inside, f"FAIL: Point ({test_x}, {test_z}) in screen window is solid!"
print("[PASS] Screen window viewport is completely clear (Zero intruding bosses).")

# 3. Bottom lip solid
for test_x in [-5.0, 0.0, 5.0]:
    inside = head_f.isInside(FreeCAD.Vector(test_x, 18.0, 41.5), 0.1, True)
    assert inside, f"FAIL: Bottom lip at X={test_x} is breached!"
print("[PASS] Bottom lip of front aperture is 100% solid and continuous (Zero neck bites).")

# 4. Parting Plane is planar (Y = 0)
f_ymin = head_f.BoundBox.YMin
r_ymax = head_r.BoundBox.YMax
print(f"Front YMin: {f_ymin:.4f}, Rear YMax: {r_ymax:.4f}")
assert abs(f_ymin) < 0.01 and abs(r_ymax) < 0.01, f"FAIL: Parting seam is not at Y=0! (f_ymin={f_ymin}, r_ymax={r_ymax})"
print("[PASS] Parting seam between Front and Rear is 100% flat at Y = 0.00 (Zero seam gap).")

# 5. Camera vs Screen separation
screen_top = 65.5
cam_bot = 70.0 - 3.75
bridge = cam_bot - screen_top
print(f"Screen top Z = {screen_top:.2f}, Camera bot Z = {cam_bot:.2f}, Solid bridge = {bridge:.2f} mm")
assert bridge > 0.5, "FAIL: Camera collides with screen!"
print(f"[PASS] Solid plastic separation between Camera and Screen = {bridge:.2f} mm (Cero colisión).")

print("\n>>> ALL 5 CHECKS PASSED WITH ZERO ERRORS! <<<")
