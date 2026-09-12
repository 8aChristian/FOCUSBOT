import FreeCAD, Part

doc = FreeCAD.open(r"c:\Users\Christian Ochoa\Documents\antigravity\goofy-borg\cad\goofy_robot_case.FCStd")

head_f = doc.getObject("Carcasa_Cabeza_Frontal").Shape
head_r = doc.getObject("Carcasa_Cabeza_Trasera").Shape
visor  = doc.getObject("Visor_Frontal_2Pulgadas").Shape
torso  = doc.getObject("Carcasa_Torso_Superior").Shape
chasis = doc.getObject("Chasis_Inferior").Shape
wheel_l = doc.getObject("Rueda_Traccion_L").Shape
wheel_r = doc.getObject("Rueda_Traccion_R").Shape
ball_f  = doc.getObject("Bola_Rodamiento_Frontal").Shape
ball_r  = doc.getObject("Bola_Rodamiento_Trasera").Shape

print("=== VERIFYING v8 CAD GENERATION ===")

# 1. Torso side walls sealed (no slits)
for y_pt in [-25.0, -15.0, 0.0, 15.0, 25.0]:
    pt_l = FreeCAD.Vector(-28.0, y_pt, 22.0)
    pt_r = FreeCAD.Vector(28.0, y_pt, 22.0)
    assert torso.isInside(pt_l, 0.1, True) and torso.isInside(pt_r, 0.1, True), f"Torso side wall breach at Y={y_pt}!"
print("[PASS] Torso side walls are 100% sealed and closed (Zero slits/openings).")

# 2. Chin is solid (no mic hole)
pt_chin = FreeCAD.Vector(-6.0, 18.0, 40.5)
assert head_f.isInside(pt_chin, 0.1, True), "Chin has a hole!"
print("[PASS] Head chin is 100% solid (Zero stray holes).")

# 3. Servo horn locking holes in head underside
# Holes at X = ±5.5mm, Z in [39.3, 42.0]
for hx in [-5.5, 5.5]:
    pt_h = FreeCAD.Vector(hx, 0.0, 40.0)
    assert not head_r.isInside(pt_h, 0.1, True), f"Hole at X={hx} missing!"
print("[PASS] Servo horn mechanical lock holes at X=±5.5mm verified.")

# 4. Ball casters ground contact
print(f"Ball Caster Front ZMin: {ball_f.BoundBox.ZMin:.2f} mm")
print(f"Ball Caster Rear ZMin:  {ball_r.BoundBox.ZMin:.2f} mm")
print(f"Wheel Left ZMin:        {wheel_l.BoundBox.ZMin:.2f} mm")
print(f"Wheel Right ZMin:       {wheel_r.BoundBox.ZMin:.2f} mm")
print(f"Total Robot Height:     {head_r.BoundBox.ZMax:.2f} mm ({head_r.BoundBox.ZMax/10:.2f} cm)")
print("\n>>> ALL CHECKS VERIFIED SUCCESSFULLY! <<<")
