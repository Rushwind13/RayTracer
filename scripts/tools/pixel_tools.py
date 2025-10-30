#!/usr/bin/env python3
import sys, json, os, math

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
WORLD = os.path.join(ROOT, 'bin', 'World.json')
PF_OUT = os.path.join(ROOT, 'PixelFactory', 'output.txt')

def norm3(v):
    x,y,z = v
    l = math.sqrt(x*x+y*y+z*z)
    if l == 0: return (0.0,0.0,0.0)
    return (x/l, y/l, z/l)

def write_pixel_line(f, x, y, eye, dir3):
    # Build a PrintPixel-compatible line with defaults
    dx,dy,dz = dir3
    # primaryRay
    f.write(f"{x},{y},")                 # px.x, px.y
    f.write(f"-1,")                       # oid
    f.write(f"{dx},{dy},{dz},0,")        # primaryRay.direction (w=0)
    f.write(f"{eye[0]},{eye[1]},{eye[2]},0,") # primaryRay.origin (w=0)
    f.write(f"1,")                        # primaryRay.length
    f.write(f"0,")                        # type iPrimary
    # r (copy primary)
    f.write(f"{dx},{dy},{dz},0,")
    f.write(f"{eye[0]},{eye[1]},{eye[2]},0,")
    f.write(f"1,")
    # normal (0), position (0)
    f.write("0,0,0,0,")
    f.write("0,0,0,0,")
    # distance, lid, NdotL, depth, weight, color(rgba), gothit
    f.write("1e9,0,0,0,1,0,0,0,0,0\n")

def cmd_extract_pf(out_path, x, y):
    x = float(x); y = float(y)
    with open(PF_OUT, 'r') as fin, open(out_path, 'w') as out:
        idx = 0
        while True:
            px = fin.readline()
            if not px: break
            li = fin.readline()  # blank intersection
            p = px.strip().split(',')
            if len(p) < 2: continue
            if abs(float(p[0]) - x) < 1e-3 and abs(float(p[1]) - y) < 1e-3:
                out.write(px)
                out.write(li if li is not None else "\n")
                print(f"wrote {out_path} for ({x},{y})")
                return 0
            idx += 1
    print("pixel not found", file=sys.stderr)
    return 1

def cmd_to_object(out_path, obj_name):
    with open(WORLD, 'r') as f:
        world = json.load(f)
    eye = world['camera']['eye']
    obj = world['objects'][obj_name]
    center = obj.get('center', [0.0,0.0,0.0])
    dir3 = norm3((center[0]-eye[0], center[1]-eye[1], center[2]-eye[2]))
    with open(out_path, 'w') as out:
        write_pixel_line(out, 75.0, 50.0, eye, dir3)
        out.write("\n")  # blank intersection
    print(f"wrote {out_path} for ray eye->{obj_name}")
    return 0

def main():
    if len(sys.argv) < 2:
        print("Usage: pixel_tools.py extract_pf <out.csv> <x> <y> | to_object <out.csv> <object>")
        return 2
    cmd = sys.argv[1]
    if cmd == 'extract_pf' and len(sys.argv) == 5:
        return cmd_extract_pf(sys.argv[2], sys.argv[3], sys.argv[4])
    if cmd == 'to_object' and len(sys.argv) == 4:
        return cmd_to_object(sys.argv[2], sys.argv[3])
    print("Usage: pixel_tools.py extract_pf <out.csv> <x> <y> | to_object <out.csv> <object>")
    return 2

if __name__ == '__main__':
    sys.exit(main())
