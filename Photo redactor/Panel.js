export class Panel {
    constructor(canvas, processor) {
        this.canvas = canvas;
        this.processor = processor;
        this.origImg = null;

        this.modes = {
            adjust: document.getElementById('adjust-menu'),
            filters: document.getElementById('filters-menu'),
        };

        this.element = document.getElementById('panel');

        this.undoBtn = document.getElementById('undo');
        this.redoBtn = document.getElementById('redo');
        this.showOrigBtn = document.getElementById('show-original');
        this.resetBtn = document.getElementById('reset');

        this.sensitivityContainer = document.getElementById('sensitivity-container');
        this.sensitivitySlider = document.getElementById('sensitivity');
        this.sensitivityValue = document.getElementById('sensitivity-value');

        this.pipetteContainer = document.getElementById('pipette');
        this.pipetteColor = document.getElementById('pipette-color');
        this.pipetteValue = document.getElementById('pipette-value');

        this.frame = document.getElementById('frame');

        this.cropBtn = document.getElementById('crop');
        this.adjustBtn = document.getElementById('adjust');
        this.filtersBtn = document.getElementById('filters');
        this.chromaKeyBtn = document.getElementById('chroma-key');
        this.removeBgBtn = document.getElementById('remove-background');

        this.brightnessBtn = document.getElementById('brightness');
        this.contrastBtn = document.getElementById('contrast');
        this.saturationBtn = document.getElementById('saturation');
        this.sharpenBtn = document.getElementById('sharpen');
        this.temperatureBtn = document.getElementById('temperature');
        this.noiseReductionBtn = document.getElementById('noise-reduction');
        
        this.negativeBtn = document.getElementById('negative');
        this.blurBtn = document.getElementById('blur');
        this.motionBlurVertBtn = document.getElementById("motion-blur-vertical");
        this.motionBlurHorizBtn = document.getElementById("motion-blur-horizontal");

        this.filters = [
            'portrait', 'landscape', 'autumn', 'winter', 'night',
            'vintage', 'retro', 'cinematic', 'hollywood', 'dramatic',
            'natural', 'cool', 'warm', 'sharp', 'soft',
            'pastel', 'fresh', 'food', 'cyberpunk', 'neon',
            'sepia', 'bw'
        ]
        this.filterBtns = [];
        for (const filter of this.filters) {
            const btn = document.getElementById(filter);
            if (btn) {
                this.filterBtns[filter] = btn;
            }
        }

        this.currentMenu = null;
        this.currentAction = null;
        this.hideAllMenus();
        this.#initEventListeners();
    }

    #initEventListeners() {
        this.undoBtn.addEventListener('click', () => {
            this.canvas.undo();
            this.processor.saveLastImgData();
        });

        this.redoBtn.addEventListener('click', () => {
            this.canvas.redo();
            this.processor.saveLastImgData();
        });

        this.showOrigBtn.addEventListener('mousedown', () => {
            this.curImg = this.canvas.getImageData();
            this.canvas.putImageData(this.origImg, false);
            this.showOrigUpHandler = this.handleShowOrigUp.bind(this);
            window.addEventListener('mouseup', this.showOrigUpHandler);
        });

        this.resetBtn.addEventListener('click', () => {
            if (confirm('Сбросить все изменения?')) {
                this.canvas.putImageData(this.origImg);
                this.processor.saveLastImgData();
            }
        });

        this.cropBtn.addEventListener('click', () => {
            this.hideAllMenus();
            this.processor.saveLastImgData();
            this.showColorPicker(false);
            this.showSensitivity(true, 50);
            this.showFrame(true);
            this.currentAction = 'crop';
        })

        this.adjustBtn.addEventListener('click', () => {
            this.showMenu('adjust');
            this.showSensitivity(false);
            this.showColorPicker(false);
            this.showFrame(false);
        });

        this.filtersBtn.addEventListener('click', () => {
            this.showMenu('filters');
            this.showSensitivity(false);
            this.showColorPicker(false);
            this.showFrame(false);
        });

        this.chromaKeyBtn.addEventListener('click', () => {
            this.hideAllMenus();
            this.processor.saveLastImgData();
            this.showFrame(false);
            this.showSensitivity(true, 10);
            this.showColorPicker(true);
            this.currentAction = 'chromaKey';
        });

        this.removeBgBtn.addEventListener('click', () => {
            this.hideAllMenus();
            this.showColorPicker(false);
            this.showSensitivity(true, 10);
            this.showFrame(true);
            this.currentAction = 'removeBackground';
        });

        this.brightnessBtn.addEventListener('click', () => {
            this.processor.saveLastImgData();
            this.showSensitivity(true, 50);
            this.currentAction = 'brightness';
        });

        this.contrastBtn.addEventListener('click', () => {
            this.processor.saveLastImgData();
            this.showSensitivity(true, 50);
            this.currentAction = 'contrast';
        });

        this.saturationBtn.addEventListener('click', () => {
            this.processor.saveLastImgData();
            this.showSensitivity(true, 50);
            this.currentAction = 'saturation';
        });

        this.sharpenBtn.addEventListener('click', () => {
            this.processor.saveLastImgData();
            this.showSensitivity(true, 0);
            this.currentAction = 'sharpen';
        });

        this.temperatureBtn.addEventListener('click', () => {
            this.processor.saveLastImgData();
            this.showSensitivity(true, 50);
            this.currentAction = 'temperature';
        });

        this.noiseReductionBtn.addEventListener('click', () => {
            this.processor.saveLastImgData();
            this.showSensitivity(true, 0);
            this.currentAction = 'noiseReduction';
        });

        for (const [filterName, btn] of Object.entries(this.filterBtns)) {
            btn.addEventListener('click', () => {
                this.processor.saveLastImgData();
                this.showSensitivity(true, 50);
                this.currentAction = filterName;
                this.processor.applyFilter(filterName, this.sensitivitySlider.value);
            });
        }

        this.negativeBtn.addEventListener('click', () => {
            this.processor.saveLastImgData();
            this.showSensitivity(false);
            this.processor.adjustNegative();
        });

        this.blurBtn.addEventListener('click', () => {
            this.processor.saveLastImgData();
            this.showSensitivity(true, 0);
            this.currentAction = 'blur';
        });

        this.motionBlurVertBtn.addEventListener('click', () => {
            this.processor.saveLastImgData();
            this.showSensitivity(true, 0);
            this.currentAction = 'motionBlurVertical';
        })

        this.motionBlurHorizBtn.addEventListener('click', () => {
            this.processor.saveLastImgData();
            this.showSensitivity(true, 0);
            this.currentAction = 'motionBlurHorizontal';
        })

        this.sensitivitySlider.addEventListener('change', (e) => {
            const value = e.target.value;
            this.sensitivityValue.textContent = value;
            this.applyCurrentAction(value);
        });

        this.sensitivitySlider.addEventListener('input', (e) => {
            const value = e.target.value;
            this.sensitivityValue.textContent = value;
        });
    }

    applyCurrentAction(value) {
        if (this.filters.includes(this.currentAction)) {
            this.processor.applyFilter(this.currentAction, value);
        } else {
            switch (this.currentAction) {
            case 'crop':
                this.processor.rotateImage(value);
                break;
            case 'brightness':
                this.processor.adjustBrightness(value);
                break;
            case 'contrast':
                this.processor.adjustContrast(value);
                break;
            case 'saturation':
                this.processor.adjustSaturation(value);
                break;
            case 'sharpen':
                this.processor.adjustSharpness(value);
                break;
            case 'temperature':
                this.processor.adjustTemperature(value);
                break;
            case 'noiseReduction':
                this.processor.reduceNoise(value);
                break;
            case 'blur':
                this.processor.applyBlur(value);
                break;
            case 'motionBlurVertical':
                this.processor.applyMotionBlurVertical(value);
                break;
            case 'motionBlurHorizontal':
                this.processor.applyMotionBlurHorizontal(value);
                break;
            case 'chromaKey':
                this.processor.applyChromaKey(value, this.pipetteColor.style.backgroundColor);
                break;
            case 'removeBackground':
                this.processor.removeBackground(this.selectedArea, value);
                break;
            }
        }
    }

    hideAllMenus() {
        Object.values(this.modes).forEach(menu => {
            if (menu) menu.classList.add('hidden');
        });
        this.sensitivityContainer.classList.add('hidden');
        this.pipetteContainer.classList.add('hidden');
        this.frame.classList.add('hidden');
        this.currentMenu = null;
        this.currentAction = null;
    }

    showMenu(menuName) {
        this.hideAllMenus();
        if (this.modes[menuName]) {
            this.modes[menuName].classList.remove('hidden');
            this.currentMenu = menuName;
        }
    }

    showSensitivity(show, value = null) {
        if (show) {
            this.sensitivityContainer.classList.remove('hidden');
            this.sensitivitySlider.value = value;
            this.sensitivityValue.textContent = value;
        } else {
            this.sensitivityContainer.classList.add('hidden');
        }
    }

    showColorPicker(show) {
        if (show) {
            this.canvas.element.style.cursor = 'crosshair';
            this.pipetteContainer.classList.remove('hidden');
            this.pipetteClickHandler = this.handlePipetteClick.bind(this);
            this.canvas.element.addEventListener('click', this.pipetteClickHandler);
        } else {
            this.pipetteContainer.classList.add('hidden');
            this.canvas.element.removeEventListener('click', this.pipetteClickHandler);
            this.pipetteClickHandler = null;
            if (this.isColorSelected) {
                this.isColorSelected = false;
                this.canvas.element.removeEventListener('mousemove', this.pipetteMoveHandler);
                this.pipetteMoveHandler = null;
            }
            this.canvas.element.style.cursor = null;
        }
    }

    handlePipetteClick(event) {
        if (this.isColorSelected) {
            this.isColorSelected = false;
            this.canvas.element.removeEventListener('mousemove', this.pipetteMoveHandler);
            this.pipetteMoveHandler = null;
            this.processor.applyChromaKey(this.sensitivitySlider.value, this.pipetteColor.style.backgroundColor);
        } else {
            this.isColorSelected = true;
            const cursorPos = this.getCursorPosition(event);
            if (cursorPos) {
                const pixelPos = this.getPixelPosition(cursorPos);
                const hexColor = this.getPixelHexColor(pixelPos.x, pixelPos.y);
                this.pipetteColor.style.backgroundColor = hexColor;
                this.pipetteValue.textContent = hexColor;
                this.pipetteMoveHandler = this.handlePipetteMove.bind(this);
                this.canvas.element.addEventListener('mousemove', this.pipetteMoveHandler);
            }
        }
    }

    handlePipetteMove(event) {
        const cursorPos = this.getCursorPosition(event);
        if (cursorPos) {
            const pixelPos = this.getPixelPosition(cursorPos);
            const hexColor = this.getPixelHexColor(pixelPos.x, pixelPos.y);
            this.pipetteColor.style.backgroundColor = hexColor;
            this.pipetteValue.textContent = hexColor;
        }
    }

    showFrame(show) {
        if (show) {
            this.canvas.element.style.cursor = 'crosshair';
            this.frameDownHandler = this.handleFrameDown.bind(this);
            window.addEventListener('mousedown', this.frameDownHandler);
            this.frameUpHandler = this.handleFrameUp.bind(this);
            window.addEventListener('mouseup', this.frameUpHandler);
        } else {
            this.canvas.element.style.cursor = null;
            window.removeEventListener('mousedown', this.frameDownHandler);
            window.removeEventListener('mouseup', this.frameUpHandler);
        }
    }

    handleFrameDown(event) {
        this.cursorPos = [null, null];
        this.cursorPos[0] = this.getCursorPosition(event);
        if (this.cursorPos[0]) {
            this.frame.classList.remove('hidden');
            this.frame.style.left = this.cursorPos[0].x + 'px';
            this.frame.style.top = this.cursorPos[0].y + 'px';
            this.frame.style.width = '0px';
            this.frame.style.height = '0px';
            this.frameMoveHandler = this.handleFrameMove.bind(this);
            window.addEventListener('mousemove', this.frameMoveHandler);
        }
    }

    handleFrameMove(event) {
        this.cursorPos[1] = this.getCursorPosition(event);
        if (this.cursorPos[1]) {
            const width = Math.abs(this.cursorPos[1].x - this.cursorPos[0].x);
            const height = Math.abs(this.cursorPos[1].y - this.cursorPos[0].y);
            const left = Math.min(this.cursorPos[0].x, this.cursorPos[1].x);
            const top = Math.min(this.cursorPos[0].y, this.cursorPos[1].y);
            this.frame.style.left = left + 'px';
            this.frame.style.top = top + 'px';
            this.frame.style.width = width + 'px';
            this.frame.style.height = height + 'px';
        }
    }

    handleFrameUp(event) {
        this.frame.classList.add('hidden');
        window.removeEventListener('mousemove', this.frameMoveHandler);
        if (this.cursorPos && this.cursorPos[0] && this.cursorPos[1]) {
            const pixelPos = [];
            pixelPos[0] = this.getPixelPosition(this.cursorPos[0]);
            pixelPos[1] = this.getPixelPosition(this.cursorPos[1]);
            this.selectedArea = {
                start: {
                    x: Math.min(pixelPos[0].x, pixelPos[1].x),
                    y: Math.min(pixelPos[0].y, pixelPos[1].y)
                },
                end: {
                    x: Math.max(pixelPos[0].x, pixelPos[1].x),
                    y: Math.max(pixelPos[0].y, pixelPos[1].y)
                }
            };
            switch (this.currentAction) {
                case 'removeBackground':
                    this.processor.saveLastImgData();
                    this.processor.removeBackground(this.selectedArea, this.sensitivitySlider.value);
                    break;
                case 'crop':
                    this.processor.saveLastImgData();
                    this.processor.cropImage(this.selectedArea);
                    break;
            }
        }
    }

    handleShowOrigUp(event) {
        this.canvas.putImageData(this.curImg, false);
        window.removeEventListener('mouseup', this.showOrigUpHandler);
    }

    getCursorPosition(event) {
        const x = event.clientX;
        const y = event.clientY;
        const rect = this.canvas.element.getBoundingClientRect();
        if (x < rect.left || x > rect.right ||
            y < rect.top || y > rect.bottom) {
            return null;
        }
        return { x, y };
    }

    getPixelPosition(cursorPos) {
        const canvas = this.processor.canvas.element;
        const rect = canvas.getBoundingClientRect();
        const scaleX = canvas.width / rect.width;
        const scaleY = canvas.height / rect.height;
        let x = (cursorPos.x - rect.left) * scaleX;
        let y = (cursorPos.y - rect.top) * scaleY;
        x = Math.min(Math.max(0, Math.floor(x)), canvas.width - 1);
        y = Math.min(Math.max(0, Math.floor(y)), canvas.height - 1);
        return { x, y };
    }

    getPixelHexColor(x, y) {
        const ctx = this.processor.canvas.ctx;
        const pixel = ctx.getImageData(x, y, 1, 1).data;
        return '#' + [pixel[0], pixel[1], pixel[2]].map(x => {
            const hex = x.toString(16);
            return hex.length === 1 ? '0' + hex : hex;
        }).join('');
    }
}