import FreeCAD

doc = FreeCAD.open(r"c:\Users\Christian Ochoa\Documents\antigravity\goofy-borg\cad\goofy_robot_case.FCStd")

def check_bounds(name):
    o = doc.getObject(name)
    bb = o.Shape.BoundBox
    print(f"{name:27s}: X=[{bb.XMin:6.2f}, {bb.XMax:6.2f}], Y=[{bb.YMin:6.2f}, {bb.YMax:6.2f}], Z=[{bb.ZMin:6.2f}, {bb.ZMax:6.2f}] mm")
    return bb

print("--- HEAD AND 2.0 LCD ---")
check_bounds("Cabeza_Monolitica")
check_bounds("Visor_Frontal_2Pulgadas")
check_bounds("Interno_Pantalla_LCD_2_0")
check_bounds("Interno_Headboard_PCB")
check_bounds("Interno_Camara_OV2640")

print("\n--- TORSO, SERVO AND MAINBOARD ---")
check_bounds("Torso_Superior")
check_bounds("Chasis_Inferior")
check_bounds("Interno_Servo_Cuello_SG90")
check_bounds("Interno_Mainboard_PCB")

print("\n--- TOTAL ROBOT ENVELOPE ---")
all_objs = [doc.getObject(n) for n in ["Cabeza_Monolitica", "Torso_Superior", "Chasis_Inferior", "Rueda_Traccion_L", "Rueda_Traccion_R"]]
total_zmin = min(o.Shape.BoundBox.ZMin for o in all_objs)
total_zmax = max(o.Shape.BoundBox.ZMax for o in all_objs)
total_xmin = min(o.Shape.BoundBox.XMin for o in all_objs)
total_xmax = max(o.Shape.BoundBox.XMax for o in all_objs)
total_ymin = min(o.Shape.BoundBox.YMin for o in all_objs)
total_ymax = max(o.Shape.BoundBox.YMax for o in all_objs)

print(f"Total Envelope: Width={total_xmax - total_xmin:.1f} mm, Depth={total_ymax - total_ymin:.1f} mm, Height={total_zmax - total_zmin:.1f} mm")
print(f"Z range: [{total_zmin:.2f}, {total_zmax:.2f}] mm")
