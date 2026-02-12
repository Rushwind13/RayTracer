#!/usr/bin/env python3
"""
Sanitize a Logger-formatted artifact (two-line Pixel+Intersection records):
- Replace non-numeric tokens (e.g., nan, inf, -inf, empty) with 0
- Clamp extreme magnitudes to a safe finite range to avoid stof/stol overflows

Usage: sanitize_artifact.py <in.txt> <out.txt>
"""
import sys, math

MAX_MAG = 1e12

def sanitize_token(tok: str) -> str:
    t = tok.strip()
    if not t:
        return "0"
    # Normalize common non-finite tokens
    lt = t.lower()
    if lt in ("nan", "+nan", "-nan", "inf", "+inf", "-inf"):
        return "0"
    # Attempt float conversion; fall back to 0
    try:
        v = float(t)
    except Exception:
        return "0"
    # Clamp to safe magnitude
    if math.isfinite(v):
        if v > MAX_MAG:
            v = MAX_MAG
        elif v < -MAX_MAG:
            v = -MAX_MAG
    else:
        v = 0.0
    # Preserve integer-looking values without trailing .0 to keep downstream stol happy
    if abs(v - int(v)) < 1e-12:
        return str(int(v))
    return ("%g" % v)

def sanitize_file(in_path: str, out_path: str):
    with open(in_path, 'r') as fin, open(out_path, 'w') as fout:
        for line in fin:
            # Keep empty lines as-is
            if not line.strip():
                fout.write("\n")
                continue
            toks = line.rstrip("\n").split(',')
            stoks = [sanitize_token(t) for t in toks]
            fout.write(','.join(stoks) + "\n")

def main():
    if len(sys.argv) != 3:
        print("Usage: sanitize_artifact.py <in.txt> <out.txt>", file=sys.stderr)
        return 2
    sanitize_file(sys.argv[1], sys.argv[2])
    return 0

if __name__ == '__main__':
    sys.exit(main())
