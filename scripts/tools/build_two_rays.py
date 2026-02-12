#!/usr/bin/env python3
import json, math, os, sys

# Build a two-record CSV (Pixel+Intersection per record) for a given object:
# 1) Ray through center-of-frame pixel (x=75,y=50)
# 2) Ray from eye toward the object's world-space center
# Output written to the provided path.

def normalize(v):
    l = math.sqrt(sum(c*c for c in v))
    return [c/l for c in v] if l else [0,0,0]

def cross(a,b):
    return [a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0]]

def ray_through_pixel(cam, x, y):
    eye = cam['eye']; look = cam['lookAt']; up = cam['up']
    w = int(cam['width']); h = int(cam['height']); fovy = float(cam['fovy'])
    f = normalize([look[i]-eye[i] for i in range(3)])
    r = normalize(cross(f, up))
    u = cross(r, f)
    # NDC (pixel centers): [0,w)->[-1,1], [0,h)->[-1,1]
    ndc_x = ( (x + 0.5) / w ) * 2.0 - 1.0
    ndc_y = ( (y + 0.5) / h ) * 2.0 - 1.0
    aspect = w / h
    half = math.tan(math.radians(fovy) * 0.5)
    dir_cam = [ndc_x * aspect * half, -ndc_y * half, -1.0]
    # Transform cam-space dir into world using basis (r,u,-f)
    dir_world = [
        dir_cam[0]*r[0] + dir_cam[1]*u[0] - dir_cam[2]*f[0],
        dir_cam[0]*r[1] + dir_cam[1]*u[1] - dir_cam[2]*f[1],
        dir_cam[0]*r[2] + dir_cam[1]*u[2] - dir_cam[2]*f[2],
    ]
    dir_world = normalize(dir_world)
    return eye + [1.0], dir_world + [0.0]

def ray_toward_point(cam, pt):
    eye = cam['eye']
    dir_world = normalize([pt[i]-eye[i] for i in range(3)])
    return eye + [1.0], dir_world + [0.0]

def write_pair(f, x, y, o, d):
    # Construct Pixel CSV per PrintPixel order with minimal fields
    # x,y, oid, primaryRay.dir(4), primaryRay.orig(4), primaryRay.length,
    # type, r.dir(4), r.orig(4), r.length, normal(4), position(4), distance,
    # lid, NdotL, depth, weight, color.rgba, gothit
    pr_len = 1.0
    r_len = 1.0
    normal = [0,0,0,0]; pos=[0,0,0,0]; dist=1e9; lid=0; ndotl=0; depth=0; weight=1.0; color=[0,0,0,0]
    fields = [x, y, -1,
              d[0], d[1], d[2], d[3], o[0], o[1], o[2], o[3], pr_len,
              0, d[0], d[1], d[2], d[3], o[0], o[1], o[2], o[3], r_len,
              normal[0],normal[1],normal[2],normal[3], pos[0],pos[1],pos[2],pos[3], dist,
              lid, ndotl, depth, weight, color[0],color[1],color[2],color[3], 0]
    f.write(','.join(str(v) for v in fields) + '\n')
    # Minimal Intersection CSV per PrintIntersection with default miss
    i_fields = [0, 0, -1, 0,0,0,0, 0,0,0,0, 1e9, 1e9]
    f.write(','.join(str(v) for v in i_fields) + '\n')

def main():
    if len(sys.argv) < 4:
        print('Usage: build_two_rays.py <World.json> <objectName> <out.csv>')
        sys.exit(2)
    wjson = sys.argv[1]; obj = sys.argv[2]; out = sys.argv[3]
    with open(wjson) as fh:
        W = json.load(fh)
    cam = W['camera']
    objs = W['objects']
    if obj not in objs:
        print(f'Object {obj} not found in World.json', file=sys.stderr)
        sys.exit(1)
    center = objs[obj].get('center', [0,0,0])
    # 1) center-of-frame ray
    o1, d1 = ray_through_pixel(cam, 75, 50)
    # 2) eye->object center
    o2, d2 = ray_toward_point(cam, center)
    with open(out, 'w') as f:
        write_pair(f, 75, 50, o1, d1)
        write_pair(f, -1, -1, o2, d2)
    print(f'wrote {out} (object={obj})')

if __name__ == '__main__':
    main()
