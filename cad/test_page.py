import FreeCAD
doc = FreeCAD.open(r"c:\Users\Christian Ochoa\Documents\antigravity\goofy-borg\cad\goofy_robot_case.FCStd")
page = doc.addObject('TechDraw::DrawPage', 'Page')
template = doc.addObject('TechDraw::DrawSVGTemplate', 'Template')
page.Template = template
doc.recompute()
with open(r"c:\Users\Christian Ochoa\Documents\antigravity\goofy-borg\cad\page_result.txt", "w") as f:
    f.write(str(dir(page)))
    if hasattr(page, 'PageResult'):
        f.write("\nPageResult length: " + str(len(page.PageResult)))
print("Saved page_result.txt")
