import { Canvas } from "./Canvas.js";
import { ImageProcessor } from "./ImageProcessor.js";
import { Panel } from "./Panel.js";

const dropArea = document.getElementById('drop-area');
const imgUploadBtn = document.getElementById('image-upload');
const saveBtn = document.getElementById('save-image');
const imagePanel = document.getElementById('image-panel');

let fileName;
const canvas = new Canvas('canvas');

const processor = new ImageProcessor(canvas);
const panel = new Panel(canvas, processor);

dropArea.addEventListener('click', () => {
    imgUploadBtn.click();
});
imgUploadBtn.addEventListener('change', (event) => {
    getImage(event.target.files[0]);
});
['dragenter', 'dragover', 'dragleave', 'drop'].forEach(eventName => {
    dropArea.addEventListener(eventName, preventDefaults, false);
});
['dragenter', 'dragover'].forEach(eventName => {
    dropArea.addEventListener(eventName, highlight, false);
});
['dragleave', 'drop'].forEach(eventName => {
    dropArea.addEventListener(eventName, unhighlight, false);
});
dropArea.addEventListener('drop', (event) => {
    getImage(event.dataTransfer.files[0]);
}, false);

function preventDefaults(e) {
    e.preventDefault();
    e.stopPropagation();
}

function highlight() {
    dropArea.classList.add('highlight');
}

function unhighlight() {
    dropArea.classList.remove('highlight');
}

function getImage(file) {
    const reader = new FileReader();
    reader.onload = (e) => {
        const img = new Image();

        img.onload = () => {
            canvas.element.width = img.width;
            canvas.element.height = img.height;
            canvas.drawImage(img);
            panel.origImg = canvas.getImageData();

            dropArea.classList.add('hidden');
            panel.element.classList.remove('hidden');
            imagePanel.classList.remove('hidden');
        };
        img.src = e.target.result;

        fileName = file.name.replace(/\.[^/.]+$/, "");
    };
    reader.readAsDataURL(file);
}

saveBtn.addEventListener('click', () => {
    const imgData = canvas.ctx.getImageData(0, 0, canvas.element.width, canvas.element.height);

    const tempCanvas = document.createElement('canvas');
    const tempCtx = tempCanvas.getContext('2d');
    tempCanvas.width = imgData.width;
    tempCanvas.height = imgData.height;
    tempCtx.putImageData(imgData, 0, 0);
    
    const link = document.createElement('a');
    link.download = fileName + '-edited.png';
    link.href = tempCanvas.toDataURL('image/png');
    link.click();
});