#!/usr/bin/env python3
import sys, re
from collections import Counter

def parse_xy(line):
    # Expect CSV starting with x,y,... possibly with spaces
    try:
        parts = line.strip().split(',')
        x = int(parts[0])
        y = int(parts[1])
        return x, y
    except Exception:
        return None

def load_xy(path):
    seen = set()
    maxx = -1
    maxy = -1
    with open(path, 'r') as f:
        for line in f:
            if not line.strip():
                continue
            xy = parse_xy(line)
            if xy is None:
                continue
            x, y = xy
            seen.add((x, y))
            if x > maxx: maxx = x
            if y > maxy: maxy = y
    return seen, maxx, maxy

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('Usage: analyze_png_log.py <oPNG.txt> [max_x max_y]')
        sys.exit(2)
    path = sys.argv[1]
    seen, maxx, maxy = load_xy(path)
    if len(sys.argv) >= 4:
        maxx = int(sys.argv[2])
        maxy = int(sys.argv[3])
    total = (maxx + 1) * (maxy + 1) if maxx >= 0 and maxy >= 0 else 0
    missing = []
    if total > 0:
        for y in range(maxy + 1):
            for x in range(maxx + 1):
                if (x, y) not in seen:
                    missing.append((x, y))
    print(f'Parsed {len(seen)} unique pixels; grid approx {maxx+1}x{maxy+1} => total {total}; missing {len(missing)}')
    if missing:
        print('First 20 missing:', missing[:20])
