#!/usr/bin/env python3
"""
Scan Logger-formatted per-object artifacts (oIntersectResult.<obj>.txt) and
report how many intersections (gothit=True) are present per file.

Usage:
  analyze_intersections.py [dir]

Defaults to runs/truth/objects.
"""
import os
import sys
from glob import glob

def count_hits(path):
	total = 0
	hits = 0
	finite = 0
	with open(path, 'r') as f:
		while True:
			px = f.readline()
			if not px:
				break
			i = f.readline()
			if not i:
				break
			total += 1
			parts = i.strip().split(',')
			if not parts:
				continue
			try:
				gothit = int(parts[0])
			except Exception:
				gothit = 0
			try:
				d0 = float(parts[-2])
				d1 = float(parts[-1])
			except Exception:
				d0 = d1 = 1e9
			if gothit == 1:
				hits += 1
			if d0 < 1e9 or d1 < 1e9:
				finite += 1
	return total, hits, finite

def main():
	root = sys.argv[1] if len(sys.argv) > 1 else os.path.join(os.path.dirname(__file__), '..', 'runs', 'truth', 'objects')
	root = os.path.abspath(root)
	files = sorted(glob(os.path.join(root, 'oIntersectResult.*.txt')))
	if not files:
		print(f"No per-object files found under {root}")
		sys.exit(1)
	grand_total = grand_hits = grand_finite = 0
	for fp in files:
		total, hits, finite = count_hits(fp)
		grand_total += total
		grand_hits += hits
		grand_finite += finite
		obj = os.path.basename(fp).replace('oIntersectResult.', '').replace('.txt', '')
		rate = (hits / total * 100.0) if total else 0.0
		print(f"{obj:16s} total={total:6d} hits={hits:6d} ({rate:5.1f}%) finite_dist={finite:6d}")
	if len(files) > 1:
		rate = (grand_hits / grand_total * 100.0) if grand_total else 0.0
		print(f"{'TOTAL':16s} total={grand_total:6d} hits={grand_hits:6d} ({rate:5.1f}%) finite_dist={grand_finite:6d}")

if __name__ == '__main__':
	main()
