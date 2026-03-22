from PIL import Image
import os

img_path = input('Image path: ')
img = Image.open(img_path[1:-1])
img_name = os.path.splitext(os.path.basename(img_path))[0]

# Для 3 байт на пиксель
rgb_img = img.convert('RGB')
rgb_img.tobytes()
with open(f'../files/{img_name}_rgb.raw', 'wb') as f:
    f.write(rgb_img.tobytes())
rgb_img.show()

# Для 1 байта на пиксель
gray_img = img.convert('L')
with open(f'../files/{img_name}_gray.raw', 'wb') as f:
    f.write(gray_img.tobytes())
gray_img.show()

# Для 1 бита на пиксель
black_img = img.convert('1')
with open(f'../files/{img_name}_black.raw', 'wb') as f:
    f.write(black_img.tobytes())
black_img.show()