with open(r'hardware/headboard/goofy_headboard.kicad_pcb', 'r', encoding='utf-8') as f:
    text = f.read()

import re
matches = re.findall(r'\(footprint "([^"]+)"\s+\(layer "[^"]+"\)\s+\(uuid "[^"]+"\)\s+\(at ([0-9.-]+) ([0-9.-]+).*?\(property "Reference" "([^"]+)"', text, re.DOTALL)
for fp, x, y, ref in matches:
    print(f"{ref} ({fp}): at ({x}, {y})")

edge_cuts = re.findall(r'\(gr_(?:line|arc|rect)\s+.*?\s+\(layer "Edge\.Cuts"\)', text)
print(f"\nEdge cuts segments: {len(edge_cuts)}")

# Find all gr_ lines
gr_lines = re.findall(r'\(start ([0-9.-]+) ([0-9.-]+)\) \(end ([0-9.-]+) ([0-9.-]+)\).*?\(layer "Edge\.Cuts"\)', text)
xs, ys = [], []
for x1, y1, x2, y2 in gr_lines:
    xs.extend([float(x1), float(x2)])
    ys.extend([float(y1), float(y2)])
if xs and ys:
    print(f"Board outline bounds: X: [{min(xs):.2f}, {max(xs):.2f}], Y: [{min(ys):.2f}, {max(ys):.2f}]")
    print(f"Board size: {max(xs)-min(xs):.2f} x {max(ys)-min(ys):.2f} mm")
    print(f"Board center: ({(min(xs)+max(xs))/2:.2f}, {(min(ys)+max(ys))/2:.2f})")
