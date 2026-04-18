from PIL import Image
import os

def open_raw_file(raw_path, width, height, mode):
    with open(raw_path, 'rb') as f:
        raw_data = f.read()
    try:
        img = Image.frombytes(mode, (int(width), int(height)), raw_data)
        return img
    except Exception as e:
        print(e)
        return None

def main():
    img_path = input("Image path: ").strip()
    img_path = img_path.strip('"').strip("'")
    img_name = os.path.splitext(os.path.basename(img_path))[0]
    data = img_name.split('_')
    scale = data[1].split('x')
    img = open_raw_file(img_path, scale[0], scale[1], 'RGB')
    try:
        img.show()
    except Exception as e:
        print(e)
        return

if __name__ == "__main__":
    main()