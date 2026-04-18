import keyboard
from PIL import Image
import os

def convert_rgb (img):
    rgb_img = img.convert('RGB')
    rgb_img.tobytes()
    return rgb_img

def convert_gray (img):
    gray_img = img.convert('L')
    rgb_img = gray_img.convert('RGB')
    rgb_img.tobytes()
    return rgb_img

def convert_bw_no_dither (img):
    gray_img = img.convert('L')
    bw_no_dither = gray_img.point(lambda x: 255 if x > 128 else 0, mode='L')
    rgb_img = bw_no_dither.convert('RGB')
    rgb_img.tobytes()
    return rgb_img

def convert_bw_dither (img):
    bw_1bit = img.convert('1')
    rgb_img = bw_1bit.convert('RGB')
    rgb_img.tobytes()
    return rgb_img

def ask(question):
    print(question)
    print("\tENTER: yes, ESC: no")
    
    while True:
        event = keyboard.read_event(suppress=True)
        if event.event_type == keyboard.KEY_DOWN:
            if event.name == 'enter':
                return True
            elif event.name == 'esc':
                return False

def main():
    img_path = input("Image path: ").strip()
    img_path = img_path.strip('"').strip("'")
    try:
        img = Image.open(img_path)
    except Exception as e:
        print(e)
        return
    img_name = os.path.splitext(os.path.basename(img_path))[0]
    results = {
        'rgb': None,
        'gray': None,
        'bw-no-dither': None,
        'bw-dither': None
    }

    if ask("Convert to RGB?"):
        results['rgb'] = convert_rgb(img)
    if ask("Convert to gray?"):
        results['gray'] = convert_gray(img)
    if ask("Convert to black-white (with dither)?"):
        results['bw-no-dither'] = convert_bw_no_dither(img)
    if ask("Convert to black-white (without dither)?"):
        results['bw-dither'] = convert_bw_dither(img)
    for key, result in results.items():
        if result is not None:
            try:
                with open(f"../test_data/{img_name}_{key}.raw", "wb") as f:
                    f.write(result.tobytes())
                    result.show()
            except Exception as e:
                print(e)
                return
                
if __name__ == "__main__":
    main()