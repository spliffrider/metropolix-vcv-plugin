"""Compare Rack front-buffer proofs after test-ui.ps1 -Polish (requires Pillow)."""
from pathlib import Path
from PIL import Image, ImageChops, ImageDraw

folder = Path(__file__).resolve().parents[1] / 'tmp/polish-qa'
sheet = Image.new('RGB', (1280, 620), (25, 30, 32))
draw = ImageDraw.Draw(sheet)
lenses = []
for i, name in enumerate(['bloom-off', 'bloom-on', 'dark-room']):
    frame = Image.open(folder / (name + '.png')).convert('RGB')
    crop = frame.crop((465, 100, 795, 275))
    lenses.append(crop)
    sheet.paste(crop, (15 + i * 420, 32))
    draw.text((15 + i * 420, 12), name, fill='white')
assert ImageChops.difference(lenses[0], lenses[1]).getbbox(), 'Bloom had no visible effect'

for i, zoom in enumerate([75, 100, 125, 150, 200]):
    frame = Image.open(folder / f'zoom-{zoom:03}.png').convert('RGB')
    x, y, w, h = map(float, (folder / f'zoom-{zoom:03}.txt').read_text().split())
    crop = frame.crop((round(x), round(y), round(x + w), round(y + h)))
    # Require the actual blue OLED pixels so an empty rack cannot pass inspection.
    assert sum(b > r + 20 and g > r + 10 for r, g, b in
               [crop.getpixel((cx, cy)) for cy in range(crop.height) for cx in range(crop.width)]) > 20
    sheet.paste(crop.resize((228, 122), Image.Resampling.NEAREST), (15 + i * 252, 250))
    draw.text((15 + i * 252, 230), f'{zoom}% zoom', fill='white')

glyphs = Image.open(folder / 'lettering.pgm').convert('RGB')
sheet.paste(glyphs.resize((384, 192), Image.Resampling.NEAREST), (15, 402))
draw.text((15, 382), 'Native OLED glyphs', fill='white')
sheet.save(folder / 'comparison.png')
print('PASS: visible Bloom difference and populated OLED at all five zooms')
