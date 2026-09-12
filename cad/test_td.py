import TechDraw
with open(r"c:\Users\Christian Ochoa\Documents\antigravity\goofy-borg\cad\td_methods.txt", "w") as f:
    f.write("\n".join(dir(TechDraw)))
print("Saved td_methods.txt")
