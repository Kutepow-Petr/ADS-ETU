from PIL import Image

def open_raw_file(raw_path, width, height, mode):
    modes = {'rgb': 'RGB', 'gray': 'L', 'black': '1'}
    with open(raw_path, 'rb') as f:
        raw_data = f.read()
    try:
        img = Image.frombytes(modes[mode], (width, height), raw_data)
        return img
    except Exception as e:
        print(f"Ошибка создания изображения: {e}")
        return None

file_path = input("Image path: ")
width = int(input("Input width: "))
height = int(input("Input height: "))
mode = input("Select mode: rgb, gray, black: ")
img = open_raw_file(file_path[1:-1], width, height, mode)
img.show();