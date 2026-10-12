"""Arrange native OLED captures from test-ui.ps1 -Display for visual inspection."""
from pathlib import Path
from PIL import Image, ImageDraw

root = Path(__file__).resolve().parents[1]
folder = root / 'tmp/display-qa'
files = sorted(folder.glob('*.pgm'))
sheet = Image.new('RGB', (840, ((len(files) + 2) // 3) * 170), (25, 30, 32))
draw = ImageDraw.Draw(sheet)
for index, path in enumerate(files):
    x, y = 12 + (index % 3) * 280, 26 + (index // 3) * 170
    sheet.paste(Image.open(path).convert('RGB').resize((256, 128), Image.Resampling.NEAREST), (x, y))
    draw.text((x, y - 18), path.stem, fill='white')
sheet.save(folder / 'display-contact-sheet.png')
