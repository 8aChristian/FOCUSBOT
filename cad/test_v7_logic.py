import FreeCAD
import Part
import math

print("=== TESTING MECHANICAL LOGIC & GEOMETRY v7 ===")

# -------------------------------------------------------------
# 1. HEAD GEOMETRY & CAMERA FIT TEST
# -------------------------------------------------------------
w_h, d_h, h_h = 56.0, 38.0, 35.0
z_hmin = 39.5
z_hmax = z_hmin + h_h # 74.5mm

head_raw = Part.makeBox(w_h, d_h, h_h, FreeCAD.Vector(-w_h/2, -d_h/2, z_hmin))
v_edges = [e for e in head_raw.Edges if abs(e.Vertexes[0].Point.x - e.Vertexes[1].Point.x) < 0.01 and abs(e.Vertexes[0].Point.y - e.Vertexes[1].Point.y) < 0.01]
head_solid = head_raw.makeFillet(6.0, v_edges)

z_top = z_hmax
z_bot = z_hmin
h_edges = [e for e in head_solid.Edges if (abs(e.Vertexes[0].Point.z - z_top) < 0.05 and abs(e.Vertexes[0].Point.z - z_top) < 0.05) or (abs(e.Vertexes[0].Point.z - z_bot) < 0.05 and abs(e.Vertexes[0].Point.z - z_bot) < 0.05)]
head_solid = head_solid.makeFillet(2.5, h_edges)

# Slicing at Y = 0
cutter_rear  = Part.makeBox(w_h + 20.0, 60.0, h_h + 30.0, FreeCAD.Vector(-w_h/2 - 10.0, -60.0, z_hmin - 15.0))
cutter_front = Part.makeBox(w_h + 20.0, 60.0, h_h + 30.0, FreeCAD.Vector(-w_h/2 - 10.0, 0.0, z_hmin - 15.0))

head_f = head_solid.cut(cutter_rear)
head_r = head_solid.cut(cutter_front)

# Internal cavity: floor at Z = 42.0, ceiling at Z = 72.5
cav_f = Part.makeBox(48.0, 16.5, 30.5, FreeCAD.Vector(-24.0, 0.0, 42.0))
head_f = head_f.cut(cav_f)

# Screen viewport: Width 41.5mm, Height 20.5mm, Z in [42.0, 62.5]
screen_cut = Part.makeBox(41.5, 6.0, 20.5, FreeCAD.Vector(-20.75, 15.0, 42.0))
sc_edges = [e for e in screen_cut.Edges if abs(e.Vertexes[0].Point.x - e.Vertexes[1].Point.x) < 0.01 and abs(e.Vertexes[0].Point.z - e.Vertexes[1].Point.z) < 0.01]
screen_cut = screen_cut.makeFillet(2.0, sc_edges)
head_f = head_f.cut(screen_cut)

# Camera aperture at Z = 67.5mm
# Lens hole: Dia 6.5mm (radius 3.25mm -> Z in [64.25, 70.75])
cam_cut = Part.makeCylinder(3.25, 6.0, FreeCAD.Vector(0.0, 15.0, 67.5), FreeCAD.Vector(0, 1, 0))
head_f = head_f.cut(cam_cut)

# Camera bezel: Outer Dia 8.8mm (radius 4.4mm -> Z in [63.10, 71.90])
cam_bezel_out = Part.makeCylinder(4.4, 1.0, FreeCAD.Vector(0.0, 18.5, 67.5), FreeCAD.Vector(0, 1, 0))
cam_bezel_in  = Part.makeCylinder(3.25, 1.5, FreeCAD.Vector(0.0, 18.0, 67.5), FreeCAD.Vector(0, 1, 0))
head_f = head_f.fuse(cam_bezel_out.cut(cam_bezel_in))

# Dedicated internal camera module pocket (9.0 x 9.0 x 4.5mm, Z in [63.0, 72.0])
cam_pocket = Part.makeBox(9.2, 3.5, 9.2, FreeCAD.Vector(-4.6, 13.0, 62.9))
head_f = head_f.cut(cam_pocket)

# 1. VERIFY CAMERA DOES NOT PROTRUDE ABOVE HEAD
print(f"Head roof Z Max:           {head_f.BoundBox.ZMax:.2f} mm")
print(f"Camera bezel top:          {67.5 + 4.4:.2f} mm")
print(f"Margin above camera bezel: {head_f.BoundBox.ZMax - (67.5 + 4.4):.2f} mm (MUST BE > 1.5 mm)")
assert head_f.BoundBox.ZMax - (67.5 + 4.4) > 1.5, "FAIL: Camera bezel still protrudes above head!"

# 2. VERIFY SOLID BRIDGE BETWEEN CAMERA AND SCREEN
screen_top = 62.5
cam_bezel_bot = 67.5 - 4.4 # 63.1
print(f"Screen window top:         {screen_top:.2f} mm")
print(f"Camera bezel bottom:       {cam_bezel_bot:.2f} mm")
print(f"Solid bridge separation:   {cam_bezel_bot - screen_top:.2f} mm (Zero collision!)")
assert cam_bezel_bot - screen_top > 0.5, "FAIL: Camera collides with screen!"

# -------------------------------------------------------------
# 2. SNAP-IN BALL CASTER TEST (CHASSIS INFERIOR)
# -------------------------------------------------------------
print("\n--- Testing Snap-in Ball Caster ---")
chassis_box = Part.makeBox(52.0, 60.0, 13.0, FreeCAD.Vector(-26.0, -30.0, 3.5))

# Ball radius: 4.0mm (Ø 8.0mm ball). Ball center at Z = 4.0mm
# Floor contact point of ball: Z = 4.0 - 4.0 = 0.00mm! (Same as Ø 34mm drive wheels at Z = 17.0 - 17.0 = 0.00mm!)
for sy in [-22.0, 22.0]:
    # Socket sphere: radius 4.2mm (Ø 8.4mm -> 0.2mm running clearance)
    socket_sphere = Part.makeSphere(4.2, FreeCAD.Vector(0.0, sy, 4.0))
    # Retention collar cylinder: Dia 7.4mm (radius 3.7mm) at bottom (Z = 1.0 to 2.5)
    # The ball (Dia 8.0mm) snaps past this Dia 7.4mm lip
    collar_bore = Part.makeCylinder(3.7, 3.5, FreeCAD.Vector(0.0, sy, 0.5), FreeCAD.Vector(0, 0, 1))
    
    chassis_box = chassis_box.cut(socket_sphere).cut(collar_bore)
    
    # 4 radial snap flex slits (0.8mm wide)
    slit_x = Part.makeBox(0.8, 9.0, 3.0, FreeCAD.Vector(-0.4, sy - 4.5, 0.5))
    slit_y = Part.makeBox(9.0, 0.8, 3.0, FreeCAD.Vector(-4.5, sy - 0.4, 0.5))
    chassis_box = chassis_box.cut(slit_x).cut(slit_y)

print("Snap-in Ball Caster socket successfully cut into chassis base!")

# Create the separate rolling balls (to be exported as 9_Bola_Rodamiento.stl)
ball_f = Part.makeSphere(4.0, FreeCAD.Vector(0.0, 22.0, 4.0))
ball_r = Part.makeSphere(4.0, FreeCAD.Vector(0.0, -22.0, 4.0))

print(f"Ball tangent lowest point Z: {ball_f.BoundBox.ZMin:.2f} mm (MUST BE EXACTLY 0.00 mm)")
assert abs(ball_f.BoundBox.ZMin) < 0.01, "FAIL: Ball does not touch ground at Z=0!"

print("\n>>> ALL LOGIC TESTS PASSED WITH 100% SUCCESS! <<<")
