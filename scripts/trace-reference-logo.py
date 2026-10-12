"""Reproduce the supplied manual's logo as a code-native SVG outline.

No image synthesis: follows the original bitmap's dark/red boundaries.
The original extracted image remains beside the vector for provenance.
"""
from pathlib import Path
from PIL import Image

root = Path(__file__).resolve().parents[1]
source = Image.open(root / 'res/intellijel-manual-logo.png').convert('RGB')
w, h = source.size
paths = []
for color in ('#231f20', '#ef2845'):
    mask = set()
    for y in range(h):
        for x in range(w):
            r,g,b = source.getpixel((x,y))
            if (color == '#231f20' and max(r,g,b) < 128) or (color == '#ef2845' and r > 140 and g < 120):
                mask.add((x,y))
    edges = {}
    def edge(a,b):
        edges.setdefault(a, []).append(b)
    for x,y in mask:
        if (x,y-1) not in mask: edge((x,y),(x+1,y))
        if (x+1,y) not in mask: edge((x+1,y),(x+1,y+1))
        if (x,y+1) not in mask: edge((x+1,y+1),(x,y+1))
        if (x-1,y) not in mask: edge((x,y+1),(x,y))
    contours=[]
    while edges:
        start=min(edges); p=start; points=[start]
        while True:
            ends=edges[p]; q=ends.pop()
            if not ends: del edges[p]
            p=q
            if p==start: break
            points.append(p)
        # Retain corners, omitting redundant collinear pixel-edge vertices.
        corners=[]
        for i,p in enumerate(points):
            a=points[i-1]; b=points[(i+1)%len(points)]
            if (p[0]-a[0])*(b[1]-p[1]) != (p[1]-a[1])*(b[0]-p[0]): corners.append(p)
        if corners:
            contours.append('M'+' L'.join(f'{x},{y}' for x,y in corners)+' Z')
    paths.append(f'<path fill="{color}" d="{" ".join(contours)}"/>')
(root / 'res/intellijel.svg').write_text(f'<svg xmlns="http://www.w3.org/2000/svg" width="610" height="172" viewBox="0 0 610 172">{"".join(paths)}</svg>\n')
print('Saved vector outline of the supplied manual logo.')
