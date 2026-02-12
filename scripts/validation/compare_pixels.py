#!/usr/bin/env python3
"""
Compare PixelFactory/output.txt pixel lines against data/pixels.txt pixel lines
for a 150x100 image. Ignores every second line (Intersection). Compares origin
and direction (primaryRay) with a small tolerance. Prints a summary and returns
0 on match, 1 otherwise.
"""
import sys, os, math

def parse_pixel_line(line):
	parts = line.strip().split(',')
	# Try to parse both known CSV schemas:
	# 1) PrintPixel schema (current): x,y,oid,pr.dir[4],pr.origin[4],pr.length,type,r.dir[4],...
	# 2) Legacy ref schema (data/pixels.txt): x,y,pr.dir[4],pr.origin[4],...
	if len(parts) < 12:
		return None
	try:
		x = float(parts[0]); y = float(parts[1])
		# Heuristic: if token[2] looks like an integer oid and token[12] looks like integer 'type', treat as PrintPixel
		def is_intish(s):
			try:
				v = float(s)
				return abs(v - int(v)) < 1e-9
			except Exception:
				return False
		if is_intish(parts[2]) and is_intish(parts[12]):
			dx, dy, dz, _dw = map(float, parts[3:7])
			ox, oy, oz, _ow = map(float, parts[7:11])
		else:
			dx, dy, dz, _dw = map(float, parts[2:6])
			ox, oy, oz, _ow = map(float, parts[6:10])
		return (x, y, ox, oy, oz, dx, dy, dz)
	except Exception:
		return None

def collect_pixels(path):
	out = []
	with open(path, 'r') as f:
		line_idx = 0
		for line in f:
			if line_idx % 2 == 0:  # pixel
				px = parse_pixel_line(line)
				if px is not None:
					out.append(px)
			line_idx += 1
	return out

def main():
	root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
	pf_path = os.path.join(root, 'PixelFactory', 'output.txt')
	ref_path = os.path.join(root, 'data', 'pixels.txt')
	if not os.path.isfile(pf_path):
		print(f"Missing {pf_path}. Run PixelFactory first.")
		return 2
	if not os.path.isfile(ref_path):
		print(f"Missing {ref_path}")
		return 2

	a = collect_pixels(pf_path)
	b = collect_pixels(ref_path)
	if len(a) != len(b):
		print(f"Count mismatch: PixelFactory={len(a)} ref={len(b)}")
		return 1

	tol = 1e-4
	mismatches = 0
	for i, (pa, pb) in enumerate(zip(a,b)):
		xa, ya, oxa, oya, oza, dxa, dya, dza = pa
		xb, yb, oxb, oyb, ozb, dxb, dyb, dzb = pb
		if abs(xa-xb) > 1e-3 or abs(ya-yb) > 1e-3:
			mismatches += 1; continue
		if abs(oxa-oxb) > tol or abs(oya-oyb) > tol or abs(oza-ozb) > tol:
			mismatches += 1; continue
		if abs(dxa-dxb) > tol or abs(dya-dyb) > tol or abs(dza-dzb) > tol:
			mismatches += 1; continue

	if mismatches == 0:
		print(f"MATCH: {len(a)} pixels match origin+direction within tol={tol}")
		return 0
	else:
		# Show first few diffs for debugging
		shown = 0
		print(f"MISMATCH: {mismatches} of {len(a)} differ (tol={tol})")
		for i, (pa, pb) in enumerate(zip(a,b)):
			xa, ya, oxa, oya, oza, dxa, dya, dza = pa
			xb, yb, oxb, oyb, ozb, dxb, dyb, dzb = pb
			if (abs(oxa-oxb) > tol or abs(oya-oyb) > tol or abs(oza-ozb) > tol or
				abs(dxa-dxb) > tol or abs(dya-dyb) > tol or abs(dza-dzb) > tol):
				print(f"idx={i} xy=({xa},{ya}) PF_o=({oxa:.6f},{oya:.6f},{oza:.6f}) PF_d=({dxa:.6f},{dya:.6f},{dza:.6f})"
					  f" vs REF_o=({oxb:.6f},{oyb:.6f},{ozb:.6f}) REF_d=({dxb:.6f},{dyb:.6f},{dzb:.6f})")
				shown += 1
				if shown >= 3:
					break
		return 1

if __name__ == '__main__':
	sys.exit(main())
