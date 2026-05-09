export class Canvas {
    constructor(canvasName) {
        this.element = document.getElementById(canvasName);
        this.ctx = this.element.getContext('2d', { alpha: true });
        this.undoStack = [];
        this.redoStack = [];
        this.maxHistory = 20;
    }
    
    #saveState() {
        const imgData = this.getImageData();
        const copy = this.#copyImgData(imgData);
        this.undoStack.push(copy);

        if (this.undoStack.length > this.maxHistory) {
            this.undoStack.shift();
        }

        this.redoStack = [];
    }
    
    undo() {
        if (this.undoStack.length > 1) {
            const cur = this.getImageData();
            const curCopy = this.#copyImgData(cur);
            this.redoStack.push(curCopy);

            const prev = this.undoStack[this.undoStack.length - 2];
            this.putImageData(prev, false);

            this.undoStack.pop();
        }
    }
    
    redo() {
        if (this.redoStack.length > 0) {
            const cur = this.getImageData();
            const curCopy = this.#copyImgData(cur);
            this.undoStack.push(curCopy);
            
            const next = this.redoStack.pop();
            this.putImageData(next, false);
        }
    }
    
    drawImage(img) {
        this.ctx.drawImage(img, 0, 0);
        this.#saveState();
    }
    
    putImageData(imgData, saveToHistory = true) {
        if (this.element.width !== imgData.width || this.element.height !== imgData.height) {
            this.element.width = imgData.width;
            this.element.height = imgData.height;
        }
        this.ctx.putImageData(imgData, 0, 0);
        
        if (saveToHistory) {
            this.#saveState();
        }
    }
    
    getImageData() {
        return this.ctx.getImageData(0, 0, this.element.width, this.element.height);
    }

    #copyImgData(imgData) {
        return new ImageData(
            new Uint8ClampedArray(imgData.data),
            imgData.width,
            imgData.height
        );
    }
}