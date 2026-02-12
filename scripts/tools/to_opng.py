#!/usr/bin/env python3
"""
Convert one or more Pixel+Intersection CSV artifacts (two lines per record, as written by Logger)
into an oPNG.txt-shaped artifact that Feeder+Writer can consume to render a PNG.

Usage:
  to_opng.py <out_oPNG.txt> <input1.txt> [input2.txt ...]

Notes:
- This is a structural pass-through: artifacts produced by Logger already use PrintPixel+PrintIntersection
  which matches what Writer expects (Pixel header + Intersection payload). We simply concatenate inputs
  in the given order. Use freeze_truth.sh first to de-duplicate if your source was affected by the older
  Feeder arg bug that doubled records.
"""
import sys

def main():
    if len(sys.argv) < 3:
        print("Usage: to_opng.py <out_oPNG.txt> <input1.txt> [input2.txt ...]", file=sys.stderr)
        sys.exit(2)
    out = sys.argv[1]
    ins = sys.argv[2:]
    with open(out, 'w') as o:
        for p in ins:
            with open(p, 'r') as f:
                for line in f:
                    o.write(line)
    print(f"wrote {out} from {len(ins)} input file(s)")

if __name__ == '__main__':
    main()
