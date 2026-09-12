import FreeCAD, Part

doc = FreeCAD.open(r"c:\Users\Christian Ochoa\Documents\antigravity\goofy-borg\cad\goofy_robot_case.FCStd")

head_f = doc.getObject("Carcasa_Cabeza_Frontal").Shape
head_r = doc.getObject("Carcasa_Cabeza_Trasera").Shape
torso  = doc.getObject("Carcasa_Torso_Superior").Shape
chasis = doc.getObject("Chasis_Inferior").Shape

print(f"Head Front Y bounds: [{head_f.BoundBox.YMin:.2f}, {head_f.BoundBox.YMax:.2f}]")
print(f"Head Rear  Y bounds: [{head_r.BoundBox.YMin:.2f}, {head_r.BoundBox.YMax:.2f}]")
print(f"Torso Y bounds:      [{torso.BoundBox.YMin:.2f}, {torso.BoundBox.YMax:.2f}]")

# Where is the screen opening? (should be on the FRONT face)
# In head_f, screen_cut was at Y in [15.0, 21.0] -> +Y is the FRONT of the head!
# Where is the speaker grille?
# In head_r, speaker holes were at Y = -21.0 -> -Y is the REAR of the head!

# Now where is the Torso USB-C port?
# usb_pocket was at Y = -33.5 -> -Y is the REAR of the torso!
# Where is the Torso Cat Logo?
# logo was at Y = 32.2 -> +Y is the FRONT of the torso!

# Wait, then why in the screenshot does the user see BOTH the speaker grille on the head AND the cat logo on the torso at the SAME time?!
# Look at the screenshot:
# In the head, we see the speaker grille (which is at -Y).
# In the torso, we see the cat logo (which is at +Y).
# HOW CAN YOU SEE BOTH AT THE SAME TIME UNLESS ONE IS ON -Y AND ONE IS ON +Y?!
# Let's check: Did the cut for logo or bar cut through the back, or is the user looking through transparent/wireframe, or was torso cut at -Y?
print("\nChecking Torso cuts:")
for y_test in [-32.0, 32.0]:
    # Check if logo is at y_test
    print(f"Torso faces near Y={y_test}:")
    faces = [f for f in torso.Faces if abs(f.BoundBox.YMin - y_test) < 1.5]
    print(f"  Count: {len(faces)}")
