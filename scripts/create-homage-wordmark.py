"""Build the requested 'sensiburner <3 intellijel' panel wordmark as SVG.

Shared letters follow the supplied manual logo; new letters use matching
heavy geometric strokes. No installed font is needed to render the result.
"""
from pathlib import Path
import xml.etree.ElementTree as ET
from PIL import Image

root = Path(__file__).resolve().parents[1]
source = Image.open(root / 'res/intellijel-manual-logo.png').convert('RGB')
ink, red = '#231f20', '#ef2845'


def outline(crop, color):
    mask = set()
    for y in range(crop.height):
        for x in range(crop.width):
            r, g, b = crop.getpixel((x, y))
            if (color == ink and max(r, g, b) < 128) or (color == red and r > 140 and g < 120):
                mask.add((x, y))
    edges = {}
    def edge(a, b):
        edges.setdefault(a, []).append(b)
    for x, y in sorted(mask):
        if (x, y-1) not in mask: edge((x, y), (x+1, y))
        if (x+1, y) not in mask: edge((x+1, y), (x+1, y+1))
        if (x, y+1) not in mask: edge((x+1, y+1), (x, y+1))
        if (x-1, y) not in mask: edge((x, y+1), (x, y))
    contours = []
    while edges:
        start = min(edges)
        p, points = start, [start]
        while True:
            ends = edges[p]
            q = ends.pop()
            if not ends: del edges[p]
            p = q
            if p == start: break
            points.append(p)
        corners = []
        for i, p in enumerate(points):
            a, b = points[i-1], points[(i+1) % len(points)]
            if (p[0]-a[0])*(b[1]-p[1]) != (p[1]-a[1])*(b[0]-p[0]): corners.append(p)
        if corners: contours.append('M'+' L'.join(f'{x},{y}' for x, y in corners)+' Z')
    return f'<path fill="{color}" d="{" ".join(contours)}"/>'


glyphs = {}
for letter, left, right in (('e', 203, 302), ('n', 44, 137), ('i', 0, 35)):
    crop = source.crop((left, 0, right, 172))
    glyphs[letter] = (right-left, ''.join(outline(crop, c) for c in (ink, red)))
glyphs['u'] = (93, '<g transform="translate(93 179) rotate(180)">'+glyphs['n'][1]+'</g>')
# Match the original's 27-unit stems, round bowls, x-height and ascender.
paths = {
    's': (92, 'M89 70H64C63 62 57 58 46 58C36 58 29 62 29 68C29 75 37 77 54 80C81 85 93 92 93 109C93 131 75 141 46 141C18 141 0 129 0 108H27C28 119 35 123 47 123C59 123 66 119 66 113C66 106 58 104 40 100C14 95 2 88 2 71C2 50 20 39 47 39C73 39 89 50 89 70Z'),
    'b': (98, 'M0 6H28V53C35 44 45 39 58 39C84 39 98 60 98 90C98 120 84 141 57 141C44 141 34 136 27 127V138H0Z M27 90C27 108 34 119 47 119C62 119 70 108 70 90C70 72 62 61 47 61C34 61 27 72 27 90Z'),
    'r': (64, 'M0 42H27V60C33 46 44 39 58 39C60 39 62 39 64 40V66C60 65 57 65 53 65C36 65 28 76 28 97V138H0Z'),
}
for letter, (width, path) in paths.items():
    glyphs[letter] = (width, f'<path fill="{ink}" fill-rule="evenodd" d="{path}"/>')

parts, x = [], 0
for letter in 'sensiburner':
    width, drawing = glyphs[letter]
    parts.append(f'<g transform="translate({x} 0)">{drawing}</g>')
    x += width+6
x += 23
# Literal <3, as requested, in the same red as the dotted i/j accents.
parts.append(f'<g transform="translate({x} 0)" fill="{red}"><path d="M46 54V76L20 91L46 106V128L0 100V82Z"/><path transform="translate(0 -13)" d="M65 54H85C108 54 119 65 119 81C119 91 114 98 105 101C116 104 122 112 122 123C122 141 109 151 86 151H64V132H85C96 132 102 128 102 121C102 114 96 110 86 110H72V92H85C95 92 100 88 100 81C100 75 95 72 85 72H65Z"/></g>')
x += 122+29
original = ET.parse(root / 'res/intellijel.svg').getroot()
parts.append(f'<g transform="translate({x} 0)">'+''.join(ET.tostring(p, encoding='unicode').replace('ns0:', '').replace(':ns0', '') for p in original)+'</g>')
width = x+610
svg = f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="172" viewBox="0 0 {width} 172"><title>sensiburner &lt;3 intellijel — independent homage</title>{"".join(parts)}</svg>\n'
(root / 'res/sensiburner-intellijel.svg').write_text(svg, encoding='utf-8')
print(f'Saved {width} x 172 SVG wordmark.')
