import sys
import os
import math

sys.path.insert(0, "C:/Users/Christian Ochoa/KiCAD-MCP-Server/python")

import pcbnew
from commands.component import ComponentCommands

def build_mainboard():
    board_path = r"c:\Users\Christian Ochoa\Documents\antigravity\goofy-borg\hardware\mainboard\goofy_mainboard.kicad_pcb"
    print(f"Creating Fresh Mainboard: {board_path}")
    board = pcbnew.BOARD()

    ds = board.GetDesignSettings()
    ds.m_MinThroughDrill = pcbnew.FromMM(0.20)
    ds.m_MinHoleSeparation = pcbnew.FromMM(0.25)
    ds.m_CopperEdgeClearance = pcbnew.FromMM(0.30)
    ds.m_SilkEdgeClearance = pcbnew.FromMM(0.05)
    ds.m_SilkClearance = pcbnew.FromMM(0.05)

    cx = 150.0
    w = 56.0
    x0 = cx - w / 2.0 # 122.0 mm
    x1 = cx + w / 2.0 # 178.0 mm
    
    # Board fully encloses ESP32 module (top at 72.75mm -> y0 = 71.0mm)
    y0 = 71.0 # Board top edge fully covers antenna
    y1 = 131.0 # Board bottom edge
    r = 3.0

    def add_edge_line(start_mm, end_mm):
        seg = pcbnew.PCB_SHAPE(board)
        seg.SetShape(pcbnew.SHAPE_T_SEGMENT)
        seg.SetLayer(pcbnew.Edge_Cuts)
        seg.SetStart(pcbnew.VECTOR2I(pcbnew.FromMM(start_mm[0]), pcbnew.FromMM(start_mm[1])))
        seg.SetEnd(pcbnew.VECTOR2I(pcbnew.FromMM(end_mm[0]), pcbnew.FromMM(end_mm[1])))
        seg.SetWidth(pcbnew.FromMM(0.15))
        board.Add(seg)

    def add_edge_arc(center_mm, start_mm, end_mm):
        arc = pcbnew.PCB_SHAPE(board)
        arc.SetShape(pcbnew.SHAPE_T_ARC)
        arc.SetLayer(pcbnew.Edge_Cuts)
        arc.SetCenter(pcbnew.VECTOR2I(pcbnew.FromMM(center_mm[0]), pcbnew.FromMM(center_mm[1])))
        arc.SetStart(pcbnew.VECTOR2I(pcbnew.FromMM(start_mm[0]), pcbnew.FromMM(start_mm[1])))
        arc.SetEnd(pcbnew.VECTOR2I(pcbnew.FromMM(end_mm[0]), pcbnew.FromMM(end_mm[1])))
        arc.SetWidth(pcbnew.FromMM(0.15))
        board.Add(arc)

    # Board Outline (Rounded Rectangle covering entire ESP32 module)
    add_edge_line((x0 + r, y0), (x1 - r, y0)) # Top
    add_edge_line((x1, y0 + r), (x1, y1 - r)) # Right
    add_edge_line((x1 - r, y1), (x0 + r, y1)) # Bottom
    add_edge_line((x0, y1 - r), (x0, y0 + r)) # Left

    add_edge_arc((x1 - r, y0 + r), (x1 - r, y0), (x1, y0 + r))
    add_edge_arc((x1 - r, y1 - r), (x1, y1 - r), (x1 - r, y1))
    add_edge_arc((x0 + r, y1 - r), (x0 + r, y1), (x0, y1 - r))
    add_edge_arc((x0 + r, y0 + r), (x0, y0 + r), (x0 + r, y0))

    def add_text(text_str, pos_mm, size_mm=0.8, layer=pcbnew.F_SilkS, bold=True):
        txt = pcbnew.PCB_TEXT(board)
        txt.SetText(text_str)
        txt.SetPosition(pcbnew.VECTOR2I(pcbnew.FromMM(pos_mm[0]), pcbnew.FromMM(pos_mm[1])))
        txt.SetLayer(layer)
        txt.SetTextSize(pcbnew.VECTOR2I(pcbnew.FromMM(size_mm), pcbnew.FromMM(size_mm)))
        txt.SetTextThickness(pcbnew.FromMM(0.15))
        txt.SetBold(bold)
        board.Add(txt)

    add_text("FOCUSROBOT", (x0 + 10.0, y0 + 3.0), 0.8)
    add_text("MAIN v1.0", (x0 + 10.0, y0 + 5.5), 0.8)
    add_text("L_MTR", (x0 + 6.0, 100.5), 0.8)
    add_text("R_MTR", (x1 - 6.0, 114.0), 0.8)
    add_text("SPK", (x1 - 6.0, 105.0), 0.8)
    add_text("BATT", (x0 + 6.0, y1 - 12.5), 0.8)

    cc = ComponentCommands(board)

    # U1 at (cx, 85.5), MH1/MH2 at Y=82.0mm (strictly outside U1 courtyard)
    components = [
        # Mounting holes M2
        {"ref": "MH1", "val": "M2", "id": "MountingHole_2.2mm_M2_Pad", "pos": (x0 + 4.0, 82.0), "rot": 0},
        {"ref": "MH2", "val": "M2", "id": "MountingHole_2.2mm_M2_Pad", "pos": (x1 - 4.0, 82.0), "rot": 0},
        {"ref": "MH3", "val": "M2", "id": "MountingHole_2.2mm_M2_Pad", "pos": (x0 + 4.0, y1 - 4.0), "rot": 0},
        {"ref": "MH4", "val": "M2", "id": "MountingHole_2.2mm_M2_Pad", "pos": (x1 - 4.0, y1 - 4.0), "rot": 0},

        # ESP32-S3 module fully on PCB
        {"ref": "U1", "val": "ESP32-S3-WROOM-1-N16R8", "id": "ESP32-S3-WROOM-1", "pos": (cx, 85.5), "rot": 0},

        # MCU Decoupling & Config (Left of U1)
        {"ref": "C1", "val": "10uF", "id": "C_0603_1608Metric", "pos": (cx - 12.5, 83.0), "rot": 0},
        {"ref": "C2", "val": "100nF", "id": "C_0603_1608Metric", "pos": (cx - 12.5, 86.0), "rot": 0},
        {"ref": "R1", "val": "10k", "id": "R_0603_1608Metric", "pos": (cx - 12.5, 89.0), "rot": 0},
        {"ref": "C3", "val": "1uF", "id": "C_0603_1608Metric", "pos": (cx - 12.5, 92.0), "rot": 0},

        # Right of U1: Servo Connector and Inter-Board FPC
        {"ref": "J_SERVO", "val": "Servo_Neck", "id": "PinHeader_1x03_P2.54mm_Vertical", "pos": (cx + 13.0, 84.5), "rot": 0},
        {"ref": "C_SERVO", "val": "100uF", "id": "CP_EIA-3528-21_Kemet-B", "pos": (cx + 13.0, 94.0), "rot": 0},
        {"ref": "J_FPC", "val": "FPC_16P_0.5mm", "id": "Hirose_FH12-16S-0.5SH_1x16-1MP_P0.50mm_Horizontal", "pos": (x1 - 4.5, 95.0), "rot": 270},

        # IMU MPU-6050 (Center of board)
        {"ref": "U8", "val": "MPU-6050", "id": "HVQFN-24-1EP_4x4mm_P0.5mm_EP2.5x2.5mm", "pos": (cx, 105.0), "rot": 0},
        {"ref": "R_SDA", "val": "4.7k", "id": "R_0603_1608Metric", "pos": (cx - 4.5, 101.5), "rot": 0},
        {"ref": "R_SCL", "val": "4.7k", "id": "R_0603_1608Metric", "pos": (cx + 4.5, 101.5), "rot": 0},

        # Motor Driver DRV8833 (Left zone)
        {"ref": "U6", "val": "DRV8833PWP", "id": "TSSOP-16_4.4x5mm_P0.65mm", "pos": (x0 + 14.5, 103.0), "rot": 0},
        {"ref": "C_VM", "val": "47uF_10V", "id": "CP_EIA-3528-21_Kemet-B", "pos": (x0 + 14.5, 96.0), "rot": 0},
        {"ref": "C6", "val": "100nF", "id": "C_0603_1608Metric", "pos": (x0 + 8.5, 98.0), "rot": 90},
        {"ref": "J_M1", "val": "Motor_Left", "id": "JST_SH_SM02B-SRSS-TB_1x02-1MP_P1.00mm_Horizontal", "pos": (x0 + 4.0, 104.0), "rot": 90},

        # Audio MAX98357A (Right zone)
        {"ref": "U7", "val": "MAX98357A", "id": "QFN-16-1EP_3x3mm_P0.5mm_EP1.45x1.45mm", "pos": (x1 - 14.0, 108.0), "rot": 0},
        {"ref": "C7", "val": "10uF", "id": "C_0603_1608Metric", "pos": (x1 - 14.0, 101.5), "rot": 0},
        {"ref": "C8", "val": "100nF", "id": "C_0603_1608Metric", "pos": (x1 - 8.5, 104.0), "rot": 90},
        {"ref": "R_GAIN", "val": "100k", "id": "R_0603_1608Metric", "pos": (x1 - 14.0, 113.5), "rot": 0},
        {"ref": "J_SPK", "val": "Speaker_1511", "id": "JST_SH_SM02B-SRSS-TB_1x02-1MP_P1.00mm_Horizontal", "pos": (x1 - 4.0, 108.0), "rot": 270},
        {"ref": "J_M2", "val": "Motor_Right", "id": "JST_SH_SM02B-SRSS-TB_1x02-1MP_P1.00mm_Horizontal", "pos": (x1 - 4.0, 117.0), "rot": 270},

        # Vibration Motor Driver
        {"ref": "Q2", "val": "2N7002", "id": "SOT-23", "pos": (cx - 7.5, 105.0), "rot": 0},
        {"ref": "D2", "val": "BAT54", "id": "D_SOD-323", "pos": (cx - 7.5, 109.0), "rot": 0},

        # Battery ADC Voltage Divider
        {"ref": "R_BAT1", "val": "100k_1%", "id": "R_0603_1608Metric", "pos": (cx + 3.0, 110.5), "rot": 0},
        {"ref": "R_BAT2", "val": "100k_1%", "id": "R_0603_1608Metric", "pos": (cx + 6.5, 110.5), "rot": 0},
        {"ref": "C_ADC", "val": "100nF", "id": "C_0603_1608Metric", "pos": (cx - 0.5, 110.5), "rot": 0},

        # Power Management Stage (Bottom edge)
        {"ref": "J_USB", "val": "USB-C-16P", "id": "USB_C_Receptacle_GCT_USB4105-xx-A_16P_TopMnt_Horizontal", "pos": (cx, y1 - 2.5), "rot": 0},
        {"ref": "R_CC1", "val": "5.1k", "id": "R_0603_1608Metric", "pos": (cx - 4.5, y1 - 10.0), "rot": 90},
        {"ref": "R_CC2", "val": "5.1k", "id": "R_0603_1608Metric", "pos": (cx + 4.5, y1 - 10.0), "rot": 90},
        {"ref": "U2", "val": "TP4056", "id": "SOIC-8_3.9x4.9mm_P1.27mm", "pos": (cx - 12.0, y1 - 12.0), "rot": 0},
        {"ref": "Q1", "val": "AO3401A", "id": "SOT-23", "pos": (cx - 2.5, y1 - 15.0), "rot": 0},
        {"ref": "R_GATE", "val": "100k", "id": "R_0603_1608Metric", "pos": (cx + 1.5, y1 - 15.0), "rot": 90},
        {"ref": "D1", "val": "SS14", "id": "D_SMA", "pos": (cx + 7.5, y1 - 15.0), "rot": 0},
        {"ref": "U5", "val": "ME6211C33M5G", "id": "SOT-23-5", "pos": (cx + 12.0, y1 - 8.5), "rot": 0},

        # Battery Protection & Switch (Bottom Left)
        {"ref": "J_BATT", "val": "LiPo_Batt", "id": "JST_PH_S2B-PH-SM4-TB_1x02-1MP_P2.00mm_Horizontal", "pos": (x0 + 5.5, y1 - 18.0), "rot": 90},
        {"ref": "U3", "val": "DW01A", "id": "SOT-23-6", "pos": (x0 + 13.5, y1 - 18.0), "rot": 0},
        {"ref": "U4", "val": "FS8205A", "id": "TSSOP-8_4.4x3mm_P0.65mm", "pos": (x0 + 19.5, y1 - 18.0), "rot": 0},
        {"ref": "SW1", "val": "Power_Switch", "id": "SW_SPDT_PCM12", "pos": (x0 + 13.0, y1 - 3.8), "rot": 180},
    ]

    placed = 0
    for comp in components:
        res = cc.place_component({
            "componentId": comp["id"],
            "reference": comp["ref"],
            "value": comp["val"],
            "position": {"x": comp["pos"][0], "y": comp["pos"][1], "unit": "mm"},
            "rotation": comp["rot"],
            "layer": "F.Cu"
        })
        if res.get("success"):
            placed += 1
        else:
            print(f"Failed to place {comp['ref']}: {res.get('errorDetails', res.get('message'))}")

    for fp in board.GetFootprints():
        ref = fp.GetReference()
        ref_item = fp.Reference()
        ref_item.SetTextSize(pcbnew.VECTOR2I(pcbnew.FromMM(0.8), pcbnew.FromMM(0.8)))
        if ref in ["SW1", "J_BATT", "J_FPC", "MH1", "MH2", "MH3", "MH4", "D1", "D2", "R_GATE", "C_ADC", "R_CC1", "R_CC2", "J_M1", "J_M2", "J_SPK", "J_USB", "R_BAT1", "R_BAT2"]:
            ref_item.SetVisible(False)

    print(f"Mainboard placed {placed} of {len(components)} components.")
    pcbnew.SaveBoard(board_path, board)
    print(f"Mainboard saved: {board_path}")

def build_headboard():
    board_path = r"c:\Users\Christian Ochoa\Documents\antigravity\goofy-borg\hardware\headboard\goofy_headboard.kicad_pcb"
    print(f"\nCreating Fresh Headboard: {board_path}")
    board = pcbnew.BOARD()

    ds = board.GetDesignSettings()
    ds.m_CopperEdgeClearance = pcbnew.FromMM(0.30)
    ds.m_SilkEdgeClearance = pcbnew.FromMM(0.05)
    ds.m_SilkClearance = pcbnew.FromMM(0.05)

    cx, cy = 150.0, 105.0
    w, h = 46.0, 30.0
    r = 2.5

    x0 = cx - w / 2.0 # 127.0 mm
    x1 = cx + w / 2.0 # 173.0 mm
    y0 = cy - h / 2.0 # 90.0 mm
    y1 = cy + h / 2.0 # 120.0 mm

    def add_edge_line(start_mm, end_mm):
        seg = pcbnew.PCB_SHAPE(board)
        seg.SetShape(pcbnew.SHAPE_T_SEGMENT)
        seg.SetLayer(pcbnew.Edge_Cuts)
        seg.SetStart(pcbnew.VECTOR2I(pcbnew.FromMM(start_mm[0]), pcbnew.FromMM(start_mm[1])))
        seg.SetEnd(pcbnew.VECTOR2I(pcbnew.FromMM(end_mm[0]), pcbnew.FromMM(end_mm[1])))
        seg.SetWidth(pcbnew.FromMM(0.15))
        board.Add(seg)

    def add_edge_arc(center_mm, start_mm, end_mm):
        arc = pcbnew.PCB_SHAPE(board)
        arc.SetShape(pcbnew.SHAPE_T_ARC)
        arc.SetLayer(pcbnew.Edge_Cuts)
        arc.SetCenter(pcbnew.VECTOR2I(pcbnew.FromMM(center_mm[0]), pcbnew.FromMM(center_mm[1])))
        arc.SetStart(pcbnew.VECTOR2I(pcbnew.FromMM(start_mm[0]), pcbnew.FromMM(start_mm[1])))
        arc.SetEnd(pcbnew.VECTOR2I(pcbnew.FromMM(end_mm[0]), pcbnew.FromMM(end_mm[1])))
        arc.SetWidth(pcbnew.FromMM(0.15))
        board.Add(arc)

    add_edge_line((x0 + r, y0), (x1 - r, y0))
    add_edge_line((x1, y0 + r), (x1, y1 - r))
    add_edge_line((x1 - r, y1), (x0 + r, y1))
    add_edge_line((x0, y1 - r), (x0, y0 + r))

    add_edge_arc((x1 - r, y0 + r), (x1 - r, y0), (x1, y0 + r))
    add_edge_arc((x1 - r, y1 - r), (x1, y1 - r), (x1 - r, y1))
    add_edge_arc((x0 + r, y1 - r), (x0 + r, y1), (x0, y1 - r))
    add_edge_arc((x0 + r, y0 + r), (x0, y0 + r), (x0 + r, y0))

    def add_text(text_str, pos_mm, size_mm=0.8, layer=pcbnew.F_SilkS, bold=True):
        txt = pcbnew.PCB_TEXT(board)
        txt.SetText(text_str)
        txt.SetPosition(pcbnew.VECTOR2I(pcbnew.FromMM(pos_mm[0]), pcbnew.FromMM(pos_mm[1])))
        txt.SetLayer(layer)
        txt.SetTextSize(pcbnew.VECTOR2I(pcbnew.FromMM(size_mm), pcbnew.FromMM(size_mm)))
        txt.SetTextThickness(pcbnew.FromMM(0.15))
        txt.SetBold(bold)
        board.Add(txt)

    add_text("FOCUSROBOT HEAD v1.0", (cx, cy + 7.5), 0.8)
    add_text("CAM TOP", (cx, y0 + 2.5), 0.8)

    cc = ComponentCommands(board)

    head_components = [
        {"ref": "MH1", "val": "M2", "id": "MountingHole_2.2mm_M2_Pad", "pos": (x0 + 3.0, y0 + 3.0), "rot": 0},
        {"ref": "MH2", "val": "M2", "id": "MountingHole_2.2mm_M2_Pad", "pos": (x1 - 3.0, y0 + 3.0), "rot": 0},
        {"ref": "MH3", "val": "M2", "id": "MountingHole_2.2mm_M2_Pad", "pos": (x0 + 3.0, y1 - 3.0), "rot": 0},
        {"ref": "MH4", "val": "M2", "id": "MountingHole_2.2mm_M2_Pad", "pos": (x1 - 3.0, y1 - 3.0), "rot": 0},

        {"ref": "J_CAM", "val": "OV2640_FPC24", "id": "Hirose_FH12-24S-0.5SH_1x24-1MP_P0.50mm_Horizontal", "pos": (cx, y0 + 6.0), "rot": 180},
        {"ref": "C_CAM1", "val": "10uF", "id": "C_0603_1608Metric", "pos": (cx - 12.0, y0 + 6.0), "rot": 0},
        {"ref": "C_CAM2", "val": "100nF", "id": "C_0603_1608Metric", "pos": (cx + 12.0, y0 + 6.0), "rot": 0},

        {"ref": "DISP1", "val": "OLED_SSD1306_I2C", "id": "PinHeader_1x04_P2.54mm_Vertical", "pos": (cx, cy - 2.0), "rot": 0},
        {"ref": "C_OLED", "val": "100nF", "id": "C_0603_1608Metric", "pos": (cx - 8.0, cy - 2.0), "rot": 90},

        {"ref": "MIC1", "val": "INMP441_MIC", "id": "Knowles_LGA-6_4.72x3.76mm", "pos": (cx - 6.0, y1 - 4.5), "rot": 0},
        {"ref": "C_MIC", "val": "100nF", "id": "C_0603_1608Metric", "pos": (cx - 11.0, y1 - 4.5), "rot": 0},

        {"ref": "J_FPC_HEAD", "val": "FPC_16P_0.5mm", "id": "Hirose_FH12-16S-0.5SH_1x16-1MP_P0.50mm_Horizontal", "pos": (x1 - 4.5, cy + 2.0), "rot": 270},
    ]

    placed = 0
    for comp in head_components:
        res = cc.place_component({
            "componentId": comp["id"],
            "reference": comp["ref"],
            "value": comp["val"],
            "position": {"x": comp["pos"][0], "y": comp["pos"][1], "unit": "mm"},
            "rotation": comp["rot"],
            "layer": "F.Cu"
        })
        if res.get("success"):
            placed += 1
        else:
            print(f"Failed to place {comp['ref']}: {res.get('errorDetails', res.get('message'))}")

    for fp in board.GetFootprints():
        ref = fp.GetReference()
        ref_item = fp.Reference()
        ref_item.SetTextSize(pcbnew.VECTOR2I(pcbnew.FromMM(0.8), pcbnew.FromMM(0.8)))
        if ref in ["MH1", "MH2", "MH3", "MH4", "J_CAM", "DISP1", "MIC1", "C_MIC"]:
            ref_item.SetVisible(False)

    print(f"Headboard placed {placed} of {len(head_components)} components.")
    pcbnew.SaveBoard(board_path, board)
    print(f"Headboard saved: {board_path}")

if __name__ == "__main__":
    mode = sys.argv[1] if len(sys.argv) > 1 else "all"
    if mode in ["main", "all"]:
        build_mainboard()
    if mode in ["head", "all"]:
        build_headboard()
