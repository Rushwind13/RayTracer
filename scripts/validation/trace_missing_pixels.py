#!/usr/bin/env python3
import sys

def load_xy(path):
    s = set()
    try:
        with open(path, 'r') as f:
            for line in f:
                if not line.strip():
                    continue
                parts = line.split(',')
                try:
                    x = int(parts[0].strip())
                    y = int(parts[1].strip())
                except Exception:
                    continue
                s.add((x, y))
    except FileNotFoundError:
        pass
    return s

if __name__ == '__main__':
    if len(sys.argv) < 4:
        print('Usage: trace_missing_pixels.py <oPNG.txt> <oDEPTH.txt> <oCOLOR.txt> [oShader.txt] [oIntersectResult.txt]')
        sys.exit(2)
    paths = sys.argv[1:]
    labels = ['PNG', 'DEPTH', 'COLOR', 'SHADER', 'INTERSECT']
    sets = []
    for i, p in enumerate(paths):
        s = load_xy(p)
        print(f'Loaded {len(s)} coords from {labels[i]}: {p}')
        sets.append(s)
    while len(sets) < 5:
        sets.append(set())
    png, depth, color, shader, inter = sets
    missing_from_png = (depth | color | shader | inter) - png
    print(f'Missing from PNG but present upstream: {len(missing_from_png)}')
    # categorize
    in_depth_only = [xy for xy in missing_from_png if xy in depth and xy not in color]
    in_color_only = [xy for xy in missing_from_png if xy in color and xy not in depth]
    in_both = [xy for xy in missing_from_png if xy in depth and xy in color]
    print(f'  present in DEPTH only: {len(in_depth_only)}')
    print(f'  present in COLOR only: {len(in_color_only)}')
    print(f'  present in both DEPTH and COLOR: {len(in_both)}')
    if in_depth_only[:10]:
        print('  examples (DEPTH only):', in_depth_only[:10])
    if in_color_only[:10]:
        print('  examples (COLOR only):', in_color_only[:10])
    if in_both[:10]:
        print('  examples (both):', in_both[:10])
