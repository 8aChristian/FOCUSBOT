import os, sys, math
import FreeCAD
import Part

print(">>> [CASE PERFECTION ENGINE v12.0 - ROBOTICS ENGINEER] Generating Production-Grade FocusBot Case...", flush=True)

base_dir = os.path.dirname(os.path.abspath(__file__))
out_dir = base_dir
stl_dir = os.path.join(out_dir, "stl")
os.makedirs(stl_dir, exist_ok=True)

doc = FreeCAD.newDocument("FocusBot_Case_v12_0")

COLOR_BODY_CREAM    = (0.95, 0.94, 0.91, 0.0)
COLOR_VISOR_BLACK   = (0.08, 0.08, 0.10, 0.0)
COLOR_CYAN_GLOW     = (0.00, 0.78, 1.00, 0.0)
COLOR_TIRE_RUBBER   = (0.13, 0.13, 0.14, 0.0)
COLOR_NECK_DARK     = (0.15, 0.15, 0.18, 0.0)
COLOR_PCB_SOLDER    = (0.06, 0.42, 0.18, 0.0)
COLOR_GOLD_LIPO     = (0.92, 0.78, 0.12, 0.0)
COLOR_STEEL_METAL   = (0.78, 0.78, 0.80, 0.0)
COLOR_SERVO_BLUE    = (0.12, 0.38, 0.85, 0.0)

created_objects = {}

def add_part(shape, name, label=None, color=COLOR_BODY_CREAM):
    obj = doc.addObject("Part::Feature", name)
    obj.Shape = shape
    if label:
        obj.Label = label
    if hasattr(obj, "ViewObject") and obj.ViewObject:
        obj.ViewObject.ShapeColor = color[:3]
        if len(color) > 3:
            obj.ViewObject.Transparency = int(color[3] * 100)
    created_objects[name] = obj
    return obj

# ==============================================================================
# DIMENSIONES MAESTRAS Y CINETICA
# ==============================================================================
# Suelo: Z = 0.00mm
# Torso: Z in [3.5, 41.0] (Altura 37.5mm)
#   - Chasis Base: Z in [3.5, 24.5] (Bañera monocasco con motores, batería, PCB)
#   - Tapa Superior: Z in [24.5, 41.0] (Cúpula de cierre con tornamesa de cuello)
# Cuello: Z in [37.5, 41.0] (Altura 3.5mm)
# Cabeza: Z in [41.0, 76.0] (Altura 35.0mm) + Orejas hasta Z = 80.0mm
# Altura Total: 80.00mm exacto dentro del standard de 8.0cm

w_t, d_t, h_t = 80.0, 77.0, 37.5
z_tmin = 3.5
z_tmax = z_tmin + h_t # 41.0mm

w_h, d_h, h_h = 57.0, 39.0, 35.0
z_hmin = 41.0
z_hmax = z_hmin + h_h # 76.0mm

# ==============================================================================
# 1. CARCASA CABEZA FRONTAL (CUNA LCD, CAMARA COAXIAL, GUIAS PCB, COPA TORNAMESA)
# ==============================================================================
print("1. Engineering Carcasa Cabeza Frontal...", flush=True)

head_box = Part.makeBox(w_h, d_h, h_h, FreeCAD.Vector(-w_h/2, -d_h/2, z_hmin))
tv_edges = [e for e in head_box.Edges if abs(e.Vertexes[0].Point.x - e.Vertexes[1].Point.x) < 0.01 and abs(e.Vertexes[0].Point.y - e.Vertexes[1].Point.y) < 0.01]
head_solid = head_box.makeFillet(4.2, tv_edges)

z_top, z_bot = z_hmax, z_hmin
h_edges = [e for e in head_solid.Edges if (abs(e.Vertexes[0].Point.z - z_top) < 0.05) or (abs(e.Vertexes[0].Point.z - z_bot) < 0.05)]
head_solid = head_solid.makeFillet(2.0, h_edges)

cutter_rear  = Part.makeBox(w_h + 20.0, 60.0, h_h + 30.0, FreeCAD.Vector(-w_h/2 - 10.0, -60.0, z_hmin - 15.0))
cutter_front = Part.makeBox(w_h + 20.0, 60.0, h_h + 30.0, FreeCAD.Vector(-w_h/2 - 10.0, 0.0, z_hmin - 15.0))

head_f_raw = head_solid.cut(cutter_rear)
head_r_raw = head_solid.cut(cutter_front)

head_f = head_f_raw

# Cavidad interna frontal continua (47.6mm de ancho: paso libre para insertar LCD 45mm y PCB 46mm desde atras)
cav_f = Part.makeBox(47.6, 16.5, 31.0, FreeCAD.Vector(-23.8, 0.0, 43.0))
c_edges = [e for e in cav_f.Edges if abs(e.Vertexes[0].Point.x - e.Vertexes[1].Point.x) < 0.01 and abs(e.Vertexes[0].Point.y - e.Vertexes[1].Point.y) < 0.01]
cav_f = cav_f.makeFillet(2.0, c_edges)
head_f = head_f.cut(cav_f)

# Ventana activa frontal para Pantalla 2.0" ST7789V integrada directamente en la carcasa (43.5 x 23.0mm, R=2.0mm)
# Estructura monocoque 100% unificada: Cero piezas separadas, cero holguras perimetrales
scr_win_box = Part.makeBox(43.5, 8.0, 23.0, FreeCAD.Vector(-21.75, 14.0, 44.75))
sw_edges = [e for e in scr_win_box.Edges if abs(e.Vertexes[0].Point.x - e.Vertexes[1].Point.x) < 0.01 and abs(e.Vertexes[0].Point.z - e.Vertexes[1].Point.z) < 0.01]
scr_win_solid = scr_win_box.makeFillet(2.0, sw_edges)
head_f = head_f.cut(scr_win_solid)

# Pinhole coaxial para Camara OV2640 a Z = 71.0mm
cam_aperture = Part.makeCylinder(1.3, 8.0, FreeCAD.Vector(0.0, 14.0, 71.0), FreeCAD.Vector(0, 1, 0))
head_f = head_f.cut(cam_aperture)

# Cuna interna para modulo LCD ST7789V 2.0" Bare Panel (45.0 x 2.0 x 24.5mm en Z in [44.5, 69.0])
lcd_pocket = Part.makeBox(45.0, 2.0, 24.5, FreeCAD.Vector(-22.5, 15.2, 44.5))
head_f = head_f.cut(lcd_pocket)

# Cuna interna para Camara OV2640 (Z en [69.2, 75.2] - ARRIBA de LCD, CERO COLISION)
cam_pocket = Part.makeBox(8.5, 3.0, 6.5, FreeCAD.Vector(-4.25, 13.5, 69.2))
head_f = head_f.cut(cam_pocket)

# Ranuras guia para Headboard PCB (ranuras limpias de 1.5x1.8mm en la pared lateral, Y = 11.5mm)
rail_l = Part.makeBox(1.5, 1.8, 29.5, FreeCAD.Vector(-24.5, 10.6, 43.5))
rail_r = Part.makeBox(1.5, 1.8, 29.5, FreeCAD.Vector(23.0, 10.6, 43.5))
head_f = head_f.cut(rail_l).cut(rail_r)

# Taladros roscados M2 100% integrados en paredes sólidas frontales (X = ±25.2, Z = 45.5 & 71.5)
# ¡Cero tubos flotantes, cero gussets, cero pegotes! Perforación directa en pared lateral sólida
h_boss_coords = [(-25.2, 71.5), (25.2, 71.5), (-25.2, 45.5), (25.2, 45.5)]
for bx, bz in h_boss_coords:
    pilot = Part.makeCylinder(0.85, 6.0, FreeCAD.Vector(bx, 0.0, bz), FreeCAD.Vector(0, 1, 0))
    head_f = head_f.cut(pilot)

# Copa hembra de tornamesa en cuello inferior (Dia 18.6mm, Altura 3.5mm, Z in [37.5, 41.0])
neck_cup_f = Part.makeCylinder(9.3, 3.5, FreeCAD.Vector(0.0, 0.0, 37.5), FreeCAD.Vector(0, 0, 1))
conduit_f  = Part.makeCylinder(4.5, 8.0, FreeCAD.Vector(0.0, 0.0, 37.5), FreeCAD.Vector(0, 0, 1))
head_f = head_f.cut(neck_cup_f).cut(conduit_f)

add_part(head_f, "Carcasa_Cabeza_Frontal", "1_Carcasa_Cabeza_Frontal", COLOR_BODY_CREAM)

# ==============================================================================
# 2. CARCASA CABEZA TRASERA (OREJAS, REJILLA PARLANTE, CUNA SERVO, TORNILLOS VISIBLES)
# ==============================================================================
print("2. Engineering Carcasa Cabeza Trasera...", flush=True)

head_r = head_r_raw

def make_cute_ear(center_x, is_left=True):
    sign = -1.0 if is_left else 1.0
    b_x, b_y, b_z = center_x, -4.0, 76.0
    p1 = FreeCAD.Vector(b_x - sign * 5.5, b_y - 2.5, b_z)
    p2 = FreeCAD.Vector(b_x + sign * 6.5, b_y - 2.5, b_z)
    p3 = FreeCAD.Vector(b_x - sign * 2.0, b_y + 3.5, b_z)
    w1 = Part.Wire([Part.makeLine(p1, p2), Part.makeLine(p2, p3), Part.makeLine(p3, p1)])
    
    m_x, m_y, m_z = center_x + sign * 1.5, -6.0, 78.5
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
    st_x, st_y, st_z = center_x + sign * 2.0, -5.8, 79.0
    sw2 = Part.Wire([Part.makeCircle(0.8, FreeCAD.Vector(st_x, st_y, st_z), FreeCAD.Vector(sign * 0.3, -0.2, 1.0))])
    scoop = Part.makeLoft([sw1, sw2], True)
    return ear_solid.cut(scoop)

ear_l = make_cute_ear(-18.0, is_left=True)
ear_r = make_cute_ear(18.0, is_left=False)
head_r = head_r.fuse(ear_l).fuse(ear_r)

# Cavidad interna trasera: 38.0mm de ancho (holgura limpia para servo SG90 y speaker 1511)
# Deja paredes laterales 100% continuas y sólidas de 9.5mm donde se alojan los tornillos pasantes
cav_r = Part.makeBox(38.0, 16.5, 31.0, FreeCAD.Vector(-19.0, -16.5, 43.0))
cr_edges = [e for e in cav_r.Edges if abs(e.Vertexes[0].Point.x - e.Vertexes[1].Point.x) < 0.01 and abs(e.Vertexes[0].Point.y - e.Vertexes[1].Point.y) < 0.01]
cav_r = cav_r.makeFillet(2.5, cr_edges)
head_r = head_r.cut(cav_r)

# Cuna integrada para Parlante 1511 (sin clips externos tipo pegote)
spk_pocket = Part.makeBox(15.4, 3.8, 11.4, FreeCAD.Vector(-7.7, -18.5, 52.3))
head_r = head_r.cut(spk_pocket)

# Rejilla acustica trasera para parlante 1511 a Z = 58.0mm
spk_cz = 58.0
holes = [Part.makeCylinder(0.9, 6.0, FreeCAD.Vector(0.0, -21.0, spk_cz), FreeCAD.Vector(0, 1, 0))]
for i in range(6):
    ang = i * 2.0 * math.pi / 6.0
    holes.append(Part.makeCylinder(0.8, 6.0, FreeCAD.Vector(3.8*math.cos(ang), -21.0, spk_cz + 3.8*math.sin(ang)), FreeCAD.Vector(0, 1, 0)))
for i in range(12):
    ang = i * 2.0 * math.pi / 12.0
    holes.append(Part.makeCylinder(0.75, 6.0, FreeCAD.Vector(7.2*math.cos(ang), -21.0, spk_cz + 7.2*math.sin(ang)), FreeCAD.Vector(0, 1, 0)))
for i in range(18):
    ang = i * 2.0 * math.pi / 18.0
    holes.append(Part.makeCylinder(0.7, 6.0, FreeCAD.Vector(10.5*math.cos(ang), -21.0, spk_cz + 10.5*math.sin(ang)), FreeCAD.Vector(0, 1, 0)))
head_r = head_r.cut(Part.makeCompound(holes))

# Alojamiento de precision para Servo SG90 (cuerpo, aletas y torre de engranaje)
s_pocket = Part.makeBox(23.6, 13.0, 23.5, FreeCAD.Vector(-11.8, -12.5, 44.0))
s_flange_pocket = Part.makeBox(33.0, 13.0, 3.0, FreeCAD.Vector(-16.5, -12.5, 59.5))
s_tower_cut = Part.makeCylinder(6.5, 5.0, FreeCAD.Vector(0.0, -6.0, 40.0), FreeCAD.Vector(0, 0, 1))
head_r = head_r.cut(s_pocket).cut(s_flange_pocket).cut(s_tower_cut)

# Taladros piloto para fijacion de aletas del servo M2
for sx in [-14.0, 14.0]:
    s_hole = Part.makeCylinder(0.9, 10.0, FreeCAD.Vector(sx, -6.0, 56.0), FreeCAD.Vector(0, 0, 1))
    head_r = head_r.cut(s_hole)

# Taladros pasantes y avellanados M2 a través de las paredes sólidas traseras
# ¡Cero tubos flotantes, cero gussets, cero escalones! Perforaciones cilíndricas 100% limpias
for bx, bz in h_boss_coords:
    thru = Part.makeCylinder(1.15, 25.0, FreeCAD.Vector(bx, -21.0, bz), FreeCAD.Vector(0, 1, 0))
    cb   = Part.makeCylinder(2.0, 3.0, FreeCAD.Vector(bx, -20.5, bz), FreeCAD.Vector(0, 1, 0))
    head_r = head_r.cut(thru).cut(cb)

# Copa hembra de tornamesa en cabeza trasera
neck_cup_r = Part.makeCylinder(9.3, 3.5, FreeCAD.Vector(0.0, 0.0, 37.5), FreeCAD.Vector(0, 0, 1))
conduit_r  = Part.makeCylinder(4.5, 8.0, FreeCAD.Vector(0.0, 0.0, 37.5), FreeCAD.Vector(0, 0, 1))
head_r = head_r.cut(neck_cup_r).cut(conduit_r)

add_part(head_r, "Carcasa_Cabeza_Trasera", "2_Carcasa_Cabeza_Trasera", COLOR_BODY_CREAM)

# ==============================================================================
# 3 & 4. CARCASA TORSO (TOP-DOWN ASSEMBLY: CHASIS BASE INFERIOR + TAPA SUPERIOR)
# ==============================================================================
print("3. Engineering Carcasa Torso (Chasis Base + Tapa Superior)...", flush=True)

torso_box = Part.makeBox(w_t, d_t, h_t, FreeCAD.Vector(-w_t/2, -d_t/2, z_tmin))
tv_edges = [e for e in torso_box.Edges if abs(e.Vertexes[0].Point.x - e.Vertexes[1].Point.x) < 0.01 and abs(e.Vertexes[0].Point.y - e.Vertexes[1].Point.y) < 0.01]
torso_solid = torso_box.makeFillet(6.0, tv_edges)

th_edges = [e for e in torso_solid.Edges if abs(e.Vertexes[0].Point.z - z_tmax) < 0.05 or abs(e.Vertexes[0].Point.z - z_tmin) < 0.05]
torso_solid = torso_solid.makeFillet(2.5, th_edges)

# Anillo Macho de Tornamesa de Cuello: Dia 18.0mm, Altura 3.5mm de Z=37.5 a 41.0mm
neck_ring = Part.makeCylinder(9.0, 3.5, FreeCAD.Vector(0.0, 0.0, 37.5), FreeCAD.Vector(0, 0, 1))
torso_solid = torso_solid.fuse(neck_ring)

# Alojamiento para Servo Horn fijo en el cuello: 15.2 x 4.2 x 2.0mm
horn_pocket = Part.makeBox(15.2, 4.2, 2.0, FreeCAD.Vector(-7.6, -2.1, 39.0))
torso_solid = torso_solid.cut(horn_pocket)

for hx in [-5.5, 5.5]:
    h_hole = Part.makeCylinder(0.85, 6.0, FreeCAD.Vector(hx, 0.0, 36.0), FreeCAD.Vector(0, 0, 1))
    torso_solid = torso_solid.cut(h_hole)

neck_conduit = Part.makeCylinder(4.5, 12.0, FreeCAD.Vector(0.0, 0.0, 34.0), FreeCAD.Vector(0, 0, 1))
tower_relief = Part.makeCylinder(6.5, 3.0, FreeCAD.Vector(0.0, -6.0, 39.5), FreeCAD.Vector(0, 0, 1))
torso_solid = torso_solid.cut(neck_conduit).cut(tower_relief)

# Cajeados de Rueda (Guardabarros) optimizados: R = 19.5mm (2.5mm holgura radial), ancho 11.5mm con 1.5mm de holgura lateral simetrica
recess_l = Part.makeCylinder(19.5, 11.5, FreeCAD.Vector(-42.5, 0.0, 17.0), FreeCAD.Vector(1, 0, 0))
recess_r = Part.makeCylinder(19.5, 11.5, FreeCAD.Vector(31.0, 0.0, 17.0), FreeCAD.Vector(1, 0, 0))
torso_solid = torso_solid.cut(recess_l).cut(recess_r)

# Orificio casquillo eje motor N20 (Dia 3.4mm)
axle_hole = Part.makeCylinder(1.7, 85.0, FreeCAD.Vector(-42.0, 0.0, 17.0), FreeCAD.Vector(1, 0, 0))
torso_solid = torso_solid.cut(axle_hole)

# Alojamientos de Bolas Rodamiento Frontal y Trasera
for y_c in [29.0, -29.0]:
    sock = Part.makeSphere(4.2, FreeCAD.Vector(0.0, y_c, 4.0))
    col  = Part.makeCylinder(3.7, 4.0, FreeCAD.Vector(0.0, y_c, 0.5), FreeCAD.Vector(0, 0, 1))
    s_x  = Part.makeBox(0.8, 9.0, 3.0, FreeCAD.Vector(-0.4, y_c - 4.5, 0.5))
    s_y  = Part.makeBox(9.0, 0.8, 3.0, FreeCAD.Vector(-4.5, y_c - 0.4, 0.5))
    caster_cut = sock.fuse(col).fuse(s_x).fuse(s_y)
    torso_solid = torso_solid.cut(caster_cut)

# Difusor de luz de pecho (Frontal)
bar_slot = Part.makeBox(18.0, 6.0, 3.2, FreeCAD.Vector(-9.0, 34.0, 25.5))
pinhole = Part.makeCylinder(0.5, 6.0, FreeCAD.Vector(0.0, 34.0, 22.0), FreeCAD.Vector(0, 1, 0))
torso_solid = torso_solid.cut(bar_slot).cut(pinhole)

# Aberturas traseras para USB-C y switch SW1
usb_throat = Part.makeBox(11.0, 8.0, 5.0, FreeCAD.Vector(-5.5, -40.0, 24.2))
sw_slot   = Part.makeBox(9.5, 8.0, 5.0, FreeCAD.Vector(-20.0, -40.0, 24.5))
torso_solid = torso_solid.cut(usb_throat).cut(sw_slot)

# BIPARTICIÓN HORIZONTAL DFA (TOP-DOWN):
# Partición a Z = 24.5mm.
# Chasis Base: Z in [3.5, 24.5mm] -> aloja motores, batería, casters y soporta la PCB a Z=23.5mm
# Tapa Superior: Z in [24.5, 41.0mm] -> cúpula de cierre con tornamesa de cuello
z_split = 24.5
cutter_top = Part.makeBox(w_t + 20.0, d_t + 20.0, 25.0, FreeCAD.Vector(-w_t/2 - 10.0, -d_t/2 - 10.0, z_split))
cutter_bot = Part.makeBox(w_t + 20.0, d_t + 20.0, 25.0, FreeCAD.Vector(-w_t/2 - 10.0, -d_t/2 - 10.0, z_split - 25.0))

torso_tapa = torso_solid.cut(cutter_bot)
torso_base = torso_solid.cut(cutter_top)

# Cavidades internas Chasis Base (65.5 x 71.5mm: holgura perimetral limpia de 0.75mm para PCB 64x70mm)
cav_base = Part.makeBox(65.5, 71.5, 20.0, FreeCAD.Vector(-32.75, -35.75, 5.5))
cb_edges = [e for e in cav_base.Edges if abs(e.Vertexes[0].Point.x - e.Vertexes[1].Point.x) < 0.01 and abs(e.Vertexes[0].Point.y - e.Vertexes[1].Point.y) < 0.01]
cav_base = cav_base.makeFillet(2.5, cb_edges)
torso_base = torso_base.cut(cav_base)

# Cavidades internas Tapa Superior
cav_tapa = Part.makeBox(65.5, 71.5, 14.0, FreeCAD.Vector(-32.75, -35.75, 24.5))
ct_edges = [e for e in cav_tapa.Edges if abs(e.Vertexes[0].Point.x - e.Vertexes[1].Point.x) < 0.01 and abs(e.Vertexes[0].Point.y - e.Vertexes[1].Point.y) < 0.01]
cav_tapa = cav_tapa.makeFillet(2.5, ct_edges)
torso_tapa = torso_tapa.cut(cav_tapa)

# Cunas / Saddles N20 en Chasis Base
saddle_l = Part.makeBox(24.5, 13.0, 6.5, FreeCAD.Vector(-28.5, -6.5, 5.5))
saddle_r = Part.makeBox(24.5, 13.0, 6.5, FreeCAD.Vector(4.0, -6.5, 5.5))
torso_base = torso_base.fuse(saddle_l).fuse(saddle_r)

# Standoffs de fijacion unificada Mainboard PCB + Cierre Tapa (MH1-MH4 a ±26.0, ±28.0)
# Cero colisiones con PCB, cero pilares seccionados: Sujecion simultanea de PCB y Tapa con 4 tornillos M2
standoff_coords = [(-26.0, 28.0), (26.0, 28.0), (-26.0, -28.0), (26.0, -28.0)]
for sx, sy in standoff_coords:
    p = Part.makeCylinder(2.5, 18.5, FreeCAD.Vector(sx, sy, 5.0), FreeCAD.Vector(0, 0, 1))
    pilot = Part.makeCylinder(0.85, 10.0, FreeCAD.Vector(sx, sy, 14.0), FreeCAD.Vector(0, 0, 1))
    torso_base = torso_base.fuse(p).cut(pilot)

for sx, sy in standoff_coords:
    p_tapa = Part.makeCylinder(2.4, 15.9, FreeCAD.Vector(sx, sy, 25.1), FreeCAD.Vector(0, 0, 1))
    thru = Part.makeCylinder(1.15, 20.0, FreeCAD.Vector(sx, sy, 24.0), FreeCAD.Vector(0, 0, 1))
    cb = Part.makeCylinder(2.0, 3.0, FreeCAD.Vector(sx, sy, 38.5), FreeCAD.Vector(0, 0, 1))
    torso_tapa = torso_tapa.fuse(p_tapa).cut(thru).cut(cb)

# Bahía LiPo 1S en suelo del Chasis Base
lipo_bay = Part.makeBox(40.0, 21.0, 7.5, FreeCAD.Vector(-20.0, -24.0, 4.5))
torso_base = torso_base.cut(lipo_bay)

add_part(torso_base, "Carcasa_Torso_Chasis", "3_Carcasa_Torso_Chasis", COLOR_BODY_CREAM)
add_part(torso_tapa,   "Carcasa_Torso_Tapa",   "4_Carcasa_Torso_Tapa",   COLOR_BODY_CREAM)

# ==============================================================================
# 5. RUEDAS DE TRACCION (LISAS, CONTACTO SUELO Z = 0.00mm, SIN RECTANGULOS)
# ==============================================================================
print("5. Engineering Smooth Traction Wheels...", flush=True)

def make_smooth_wheel(is_left=True):
    sign = -1.0 if is_left else 1.0
    w_width = 8.5
    x_inner = sign * 32.5
    x_outer = sign * (32.5 + w_width)
    x_min, x_max = min(x_inner, x_outer), max(x_inner, x_outer)
    
    # Neumático cilíndrico liso: R = 17.00mm exacto -> diámetro 34.0mm, contacto Z = 0.00mm
    tire = Part.makeCylinder(17.00, w_width, FreeCAD.Vector(x_min, 0.0, 17.0), FreeCAD.Vector(1, 0, 0))
    
    # Bisel / Chamfer suave en hombros del neumático (estilo RC Buggy, 0.8mm) para rodar suavemente sin enganches
    tire_edges = [e for e in tire.Edges if hasattr(e, 'Curve') and isinstance(e.Curve, Part.Circle) and abs(e.Curve.Radius - 17.0) < 0.1]
    if tire_edges:
        tire = tire.makeChamfer(0.8, tire_edges)
    
    # Garganta anular para Aro Cyan en cara exterior: R in [9.8, 12.5], profundidad 2.0mm
    x_groove_start = x_outer - sign * 2.0
    x_g_min = min(x_outer, x_groove_start)
    g_out = Part.makeCylinder(12.5, 2.2, FreeCAD.Vector(x_g_min, 0.0, 17.0), FreeCAD.Vector(1, 0, 0))
    g_in  = Part.makeCylinder(9.8,  2.4, FreeCAD.Vector(x_g_min - 0.1, 0.0, 17.0), FreeCAD.Vector(1, 0, 0))
    ring_groove = g_out.cut(g_in)
    tire = tire.cut(ring_groove)
    
    # Detalle decorativo central en buje (rebaje de 0.4mm a R=3.2)
    x_c_min = min(x_outer, x_outer - sign * 0.4)
    c_recess = Part.makeCylinder(3.2, 0.4, FreeCAD.Vector(x_c_min, 0.0, 17.0), FreeCAD.Vector(1, 0, 0))
    tire = tire.cut(c_recess)
    
    # Casquillo interior de apoyo con reductora N20: Dia 5.5mm, longitud 0.6mm
    collar = Part.makeCylinder(2.75, 0.6, FreeCAD.Vector(x_inner if is_left else x_inner - 0.6, 0.0, 17.0), FreeCAD.Vector(1, 0, 0))
    tire = tire.fuse(collar)
    
    # Taladro eje D-shaft a traves de casquillo y buje macizo (profundidad 7.2mm)
    bore_dir = FreeCAD.Vector(-1, 0, 0) if is_left else FreeCAD.Vector(1, 0, 0)
    x_start = x_inner - sign * 0.65
    bore = Part.makeCylinder(1.525, 7.2, FreeCAD.Vector(x_start, 0.0, 17.0), bore_dir)
    x_box_min = min(x_start, x_start + bore_dir.x * 7.2)
    flat = Part.makeBox(7.5, 4.0, 2.0, FreeCAD.Vector(x_box_min, -2.0, 18.0))
    d_bore = bore.cut(flat)
    tire = tire.cut(d_bore)
    
    return tire

wheel_left  = make_smooth_wheel(is_left=True)
wheel_right = make_smooth_wheel(is_left=False)
add_part(wheel_left,  "Rueda_Traccion_L", "7_Rueda_Traccion_L", COLOR_TIRE_RUBBER)
add_part(wheel_right, "Rueda_Traccion_R", "8_Rueda_Traccion_R", COLOR_TIRE_RUBBER)

def make_cyan_ring(is_left=True):
    sign = -1.0 if is_left else 1.0
    x_pos = sign * 39.3 if not is_left else sign * 40.7
    r_out = Part.makeCylinder(12.2, 1.4, FreeCAD.Vector(x_pos, 0.0, 17.0), FreeCAD.Vector(1, 0, 0))
    r_in  = Part.makeCylinder(10.0, 1.6, FreeCAD.Vector(x_pos - 0.1, 0.0, 17.0), FreeCAD.Vector(1, 0, 0))
    return r_out.cut(r_in)

add_part(make_cyan_ring(is_left=True),  "Aro_Cyan_Rueda_L", "Aro_Cyan_L", COLOR_CYAN_GLOW)
add_part(make_cyan_ring(is_left=False), "Aro_Cyan_Rueda_R", "Aro_Cyan_R", COLOR_CYAN_GLOW)

# Bolas rodamiento omnidireccional (Dia 8.0mm, Centro Z = 4.00mm -> Contacto Z = 0.00mm)
ball_front = Part.makeSphere(4.0, FreeCAD.Vector(0.0, 29.0, 4.0))
ball_rear  = Part.makeSphere(4.0, FreeCAD.Vector(0.0, -29.0, 4.0))
add_part(ball_front, "Bola_Rodamiento_Frontal", "9_Bola_Frontal", COLOR_STEEL_METAL)
add_part(ball_rear,  "Bola_Rodamiento_Trasera", "10_Bola_Trasera", COLOR_STEEL_METAL)

# Difusor de luz de pecho
diffuser = Part.makeBox(17.6, 2.0, 2.8, FreeCAD.Vector(-8.8, 36.5, 25.7))
d_edges = [e for e in diffuser.Edges if abs(e.Vertexes[0].Point.y - e.Vertexes[1].Point.y) < 0.01]
diffuser = diffuser.makeFillet(0.8, d_edges)
add_part(diffuser, "Difusor_Luz_Pecho", "6_Difusor_Luz_Pecho", COLOR_CYAN_GLOW)

# ==============================================================================
# 6. MODELADO DE COMPONENTES INTERNOS EXACTOS (FITS REALES)
# ==============================================================================
print("6. Modeling Internal Components with Real Mechatronic Fits...", flush=True)

# Motores N20 con eje D-shaft estandar (Radio 1.48mm, rebaje plano a Z=18.0mm)
def make_motor_n20(is_left=True):
    if is_left:
        body = Part.makeBox(24.0, 12.0, 10.0, FreeCAD.Vector(-28.5, -6.0, 12.0))
        shaft = Part.makeCylinder(1.48, 8.8, FreeCAD.Vector(-28.5, 0.0, 17.0), FreeCAD.Vector(-1, 0, 0))
        d_cut = Part.makeBox(9.0, 4.0, 2.0, FreeCAD.Vector(-37.5, -2.0, 18.0))
        shaft = shaft.cut(d_cut)
    else:
        body = Part.makeBox(24.0, 12.0, 10.0, FreeCAD.Vector(4.5, -6.0, 12.0))
        shaft = Part.makeCylinder(1.48, 8.8, FreeCAD.Vector(28.5, 0.0, 17.0), FreeCAD.Vector(1, 0, 0))
        d_cut = Part.makeBox(9.0, 4.0, 2.0, FreeCAD.Vector(28.5, -2.0, 18.0))
        shaft = shaft.cut(d_cut)
    return body.fuse(shaft)

motor_l = make_motor_n20(is_left=True)
motor_r = make_motor_n20(is_left=False)
add_part(motor_l, "Interno_Motor_N20_L", "Motor_N20_L", COLOR_STEEL_METAL)
add_part(motor_r, "Interno_Motor_N20_R", "Motor_N20_R", COLOR_STEEL_METAL)

# Bateria LiPo 1S 3.7V 500mAh: 38.0 x 20.0 x 7.0mm a Z in [4.5, 11.5]
lipo = Part.makeBox(38.0, 20.0, 7.0, FreeCAD.Vector(-19.0, -23.5, 4.5))
add_part(lipo, "Interno_Bateria_LiPo", "Bateria_LiPo_500mAh", COLOR_GOLD_LIPO)

# Mainboard PCB: 64.0 x 70.0 x 1.6mm solida, instalada en Z = 23.5mm
mainboard = Part.makeBox(64.0, 70.0, 1.6, FreeCAD.Vector(-32.0, -35.0, 23.5))
mb_edges = [e for e in mainboard.Edges if abs(e.Vertexes[0].Point.x - e.Vertexes[1].Point.x) < 0.01 and abs(e.Vertexes[0].Point.y - e.Vertexes[1].Point.y) < 0.01]
mainboard = mainboard.makeFillet(3.0, mb_edges)

for hx, hy in [(-26.0, 28.0), (26.0, 28.0), (-26.0, -28.0), (26.0, -28.0)]:
    m_hole = Part.makeCylinder(1.25, 4.0, FreeCAD.Vector(hx, hy, 22.5), FreeCAD.Vector(0, 0, 1))
    mainboard = mainboard.cut(m_hole)

# Conducto hueco pasacables en PCB
fpc_hole = Part.makeCylinder(4.5, 4.0, FreeCAD.Vector(0.0, 0.0, 22.5), FreeCAD.Vector(0, 0, 1))
mainboard = mainboard.cut(fpc_hole)
add_part(mainboard, "Interno_Mainboard_PCB", "Mainboard_PCB_v9", COLOR_PCB_SOLDER)

# Headboard PCB: 46.0 x 1.6 x 30.0mm vertical en Y in [11.5, 13.1], Z in [43.5, 73.5]
headboard = Part.makeBox(46.0, 1.6, 30.0, FreeCAD.Vector(-23.0, 11.5, 43.5))
hb_edges = [e for e in headboard.Edges if abs(e.Vertexes[0].Point.x - e.Vertexes[1].Point.x) < 0.01 and abs(e.Vertexes[0].Point.z - e.Vertexes[1].Point.z) < 0.01]
headboard = headboard.makeFillet(1.5, hb_edges)

for hx, hz in [(-20.0, 70.5), (20.0, 70.5), (-20.0, 46.5), (20.0, 46.5)]:
    h_hole = Part.makeCylinder(1.25, 4.0, FreeCAD.Vector(hx, 10.5, hz), FreeCAD.Vector(0, 1, 0))
    headboard = headboard.cut(h_hole)
add_part(headboard, "Interno_Headboard_PCB", "Headboard_PCB_v9", COLOR_PCB_SOLDER)

# Receptaculo USB-C Tipo C 16P montado en Mainboard
usb_c = Part.makeBox(9.0, 7.5, 3.2, FreeCAD.Vector(-4.5, -38.5, 25.1))
add_part(usb_c, "Interno_Conector_USBC", "Conector_USBC", COLOR_STEEL_METAL)

# Interruptor Deslizante SW1 montado en Mainboard
sw1 = Part.makeBox(8.5, 4.0, 4.0, FreeCAD.Vector(-19.25, -37.2, 25.1))
add_part(sw1, "Interno_Switch_SW1", "Switch_SW1", COLOR_STEEL_METAL)

# Servo SG90 instalado boca abajo dentro de la cabeza trasera
s_body = Part.makeBox(22.8, 12.0, 22.5, FreeCAD.Vector(-11.4, -12.0, 44.5))
s_flange = Part.makeBox(32.4, 12.0, 2.0, FreeCAD.Vector(-16.2, -12.0, 60.0))
s_tower = Part.makeCylinder(6.0, 4.0, FreeCAD.Vector(0.0, -6.0, 40.5), FreeCAD.Vector(0, 0, 1))
s_spline = Part.makeCylinder(2.4, 3.5, FreeCAD.Vector(0.0, 0.0, 37.0), FreeCAD.Vector(0, 0, 1))
servo = s_body.fuse(s_flange).fuse(s_tower).fuse(s_spline)
add_part(servo, "Interno_Servo_SG90", "Servo_SG90_Pan", COLOR_SERVO_BLUE)

# Pantalla LCD ST7789V 2.0" Panoramica bare panel enrasada en su cuna frontal (44.5 x 1.6 x 24.0mm en Z in [44.75, 68.75])
lcd = Part.makeBox(44.5, 1.6, 24.0, FreeCAD.Vector(-22.25, 15.3, 44.75))
add_part(lcd, "Interno_Display_LCD20", "Display_LCD_ST7789", COLOR_VISOR_BLACK)

# Camara OV2640 con Lente Coaxial Stealth a Z = 71.0mm (Z in [69.3, 75.1] - ARRIBA de LCD)
cam_body = Part.makeBox(8.0, 2.5, 5.8, FreeCAD.Vector(-4.0, 13.8, 69.3))
cam_lens = Part.makeCylinder(1.2, 3.5, FreeCAD.Vector(0.0, 15.5, 71.0), FreeCAD.Vector(0, 1, 0))
cam = cam_body.fuse(cam_lens)
add_part(cam, "Interno_Camara_OV2640", "Camara_OV2640", COLOR_VISOR_BLACK)

# Parlante 1511 montado en cuna trasera (15x11x3.5mm)
speaker = Part.makeBox(15.0, 3.5, 11.0, FreeCAD.Vector(-7.5, -18.3, 52.5))
add_part(speaker, "Interno_Parlante_1511", "Parlante_1511", COLOR_STEEL_METAL)

# ==============================================================================
# 7. GUARDAR DOCUMENTO NATIVO Y EXPORTAR STL
# ==============================================================================
print("7. Saving Native CAD and Exporting 3D Printable STL Files...", flush=True)

fcstd_file = os.path.join(out_dir, "focusbot_case.FCStd")
doc.saveAs(fcstd_file)
print(f"[OK] Saved Native FreeCAD Document: {fcstd_file}", flush=True)

step_file = os.path.join(out_dir, "focusbot_case.step")
all_shapes = [obj.Shape for obj in doc.Objects if hasattr(obj, "Shape")]
Part.export(all_shapes, step_file)
print(f"[OK] Saved Master STEP File: {step_file}", flush=True)

stl_export_map = {
    "01_Carcasa_Cabeza_Frontal.stl": doc.getObject("Carcasa_Cabeza_Frontal"),
    "02_Carcasa_Cabeza_Trasera.stl": doc.getObject("Carcasa_Cabeza_Trasera"),
    "03_Carcasa_Torso_Chasis.stl":   doc.getObject("Carcasa_Torso_Chasis"),
    "04_Carcasa_Torso_Tapa.stl":     doc.getObject("Carcasa_Torso_Tapa"),
    "06_Difusor_Luz_Pecho.stl":      doc.getObject("Difusor_Luz_Pecho"),
    "07_Rueda_Traccion_L.stl":       doc.getObject("Rueda_Traccion_L"),
    "08_Rueda_Traccion_R.stl":       doc.getObject("Rueda_Traccion_R"),
    "09_Aro_Cyan_Rueda_L.stl":       doc.getObject("Aro_Cyan_Rueda_L"),
    "10_Aro_Cyan_Rueda_R.stl":       doc.getObject("Aro_Cyan_Rueda_R"),
}

import MeshPart
for filename, part_obj in stl_export_map.items():
    if part_obj and hasattr(part_obj, "Shape") and part_obj.Shape.Volume > 0:
        out_stl = os.path.join(stl_dir, filename)
        m = MeshPart.meshFromShape(part_obj.Shape, 0.05, 0.35)
        m.write(out_stl)

print(">>> [CASE PERFECTION ENGINE v12.0] Complete Mechatronic Enclosure Exported Successfully!", flush=True)
