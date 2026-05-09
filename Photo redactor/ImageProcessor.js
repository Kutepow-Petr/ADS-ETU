import { KMeansSegmentation } from "./KMeansSegmentation.js";

export class ImageProcessor {
    constructor(canvas) {
        this.canvas = canvas;
        this.lastImgData = null;
    }

    saveLastImgData() {
        const currImgData = this.canvas.getImageData();
        this.lastImgData = new ImageData(
            new Uint8ClampedArray(currImgData.data),
            currImgData.width,
            currImgData.height
        );
    }

    #copyLastImgData() {
        const imgData = new ImageData(
            new Uint8ClampedArray(this.lastImgData.data),
            this.lastImgData.width,
            this.lastImgData.height
        );
        return imgData;
    }

    adjustBrightness(value) {
        const imgData = this.#copyLastImgData();
        const data = imgData.data;
        const delta = Math.floor((value - 50) * 2);

        for (let i = 0; i < data.length; i += 4) {
            data[i] = Math.min(255, Math.max(0, data[i] + delta));
            data[i + 1] = Math.min(255, Math.max(0, data[i + 1] + delta));
            data[i + 2] = Math.min(255, Math.max(0, data[i + 2] + delta));
        }
        this.canvas.putImageData(imgData);
    }

    adjustContrast(value) {
        const imgData = this.#copyLastImgData();
        const data = imgData.data;
        const strength = 1 + ((value - 50) / 70);

        for (let i = 0; i < data.length; i += 4) {
            data[i] = Math.min(255, Math.max(0, (data[i] - 128) * strength + 128));
            data[i + 1] = Math.min(255, Math.max(0, (data[i + 1] - 128) * strength + 128));
            data[i + 2] = Math.min(255, Math.max(0, (data[i + 2] - 128) * strength + 128));
        }
        this.canvas.putImageData(imgData);
    }

    COLOR_MATRICES = {
        portrait: [
            1.1, 0.0, 0.0, 5,
            0.0, 1.05, 0.0, 3,
            0.0, 0.0, 0.95, 2,
        ],
        landscape: [
            1.1, 0.0, 0.0, 0,
            0.0, 1.15, 0.0, 0,
            0.0, 0.0, 1.2, 0,
        ],
        autumn: [
            1.2, 0.1, 0.0, 5,
            0.0, 1.0, 0.0, 0,
            0.0, 0.0, 0.7, 5,
        ],
        winter: [
            0.9, 0.0, 0.1, 10,
            0.0, 1.05, 0.05, 5,
            0.1, 0.0, 1.2, 15,
        ],
        night: [
            0.7, 0.0, 0.3, -20,
            0.0, 0.8, 0.1, -15,
            0.2, 0.0, 1.1, -10,
        ],
        vintage: [
            0.9, 0.1, 0.0, 10,
            0.0, 0.85, 0.0, 5,
            0.0, 0.0, 0.7, 15,
        ],
        retro: [
            1.2, -0.1, 0.0, 0,
            0.0, 0.9, 0.0, 0,
            0.0, 0.0, 0.8, 5,
        ],
        cinematic: [
            1.15, 0.0, 0.0, -5,
            0.0, 1.0, 0.0, 0,
            0.0, 0.0, 0.9, 5,
        ],
        hollywood: [
            1.2, 0.0, 0.1, 0,
            0.0, 1.05, 0.0, 0,
            -0.1, 0.0, 1.1, 10,
        ],
        dramatic: [
            1.3, 0.0, 0.0, -15,
            0.0, 1.2, 0.0, -10,
            0.0, 0.0, 1.1, -5,
        ],
        natural: [
            1.05, 0.02, 0.0, 2,
            0.0, 1.05, 0.0, 2,
            0.02, 0.0, 1.02, 2,
        ],
        cool: [
            0.8, 0.0, 0.2, 0,
            0.0, 1.0, 0.0, 0,
            0.1, 0.0, 1.2, 5,
        ],
        warm: [
            1.2, 0.0, 0.0, 5,
            0.0, 1.0, 0.0, 2,
            0.0, 0.0, 0.8, 0,
        ],
        sharp: [
            1.2, 0.0, 0.0, -5,
            0.0, 1.2, 0.0, -5,
            0.0, 0.0, 1.2, -5,
        ],
        soft: [
            0.95, 0.0, 0.0, 5,
            0.0, 0.95, 0.0, 5,
            0.0, 0.0, 0.95, 5,
        ],
        pastel: [
            1.1, 0.0, 0.0, 10,
            0.0, 1.1, 0.0, 10,
            0.0, 0.0, 1.1, 15,
        ],
        fresh: [
            1.15, 0.0, 0.0, 3,
            0.0, 1.15, 0.0, 3,
            0.0, 0.0, 1.1, 5,
        ],
        food: [
            1.1, 0.1, 0.0, 5,
            0.0, 1.0, 0.0, 0,
            0.0, 0.0, 0.8, 5,
        ],
        cyberpunk: [
            1.2, 0.0, 0.3, -10,
            0.0, 0.7, 0.0, 0,
            0.2, 0.0, 1.4, 10,
        ],
        neon: [
            1.3, 0.0, 0.5, 5,
            0.0, 0.8, 0.0, 0,
            0.5, 0.0, 1.4, -5,
        ],
        sepia: [
            0.393, 0.769, 0.189, 0,
            0.349, 0.686, 0.168, 0,
            0.272, 0.534, 0.131, 0,
        ],
        bw: [
            0.299, 0.587, 0.114, 0,
            0.299, 0.587, 0.114, 0,
            0.299, 0.587, 0.114, 0,
        ],
        negative: [
            -1, 0, 0, 255,
            0, -1, 0, 255,
            0, 0, -1, 255,
        ],
    };

    applyFilter(filterName, value) {
        const matrix = this.scaleColorMatrix(this.COLOR_MATRICES[filterName], value);
        this.#applyColorMatrix(matrix);
    }

    scaleColorMatrix(targetMatrix, value) {
        const strength = value / 100;
        const E = [1,0,0,0, 0,1,0,0, 0,0,1,0];
        
        const matrix = E.map((v, i) => 
            E[i] + (targetMatrix[i] - E[i]) * strength
        );
        return matrix;
    }

    #applyColorMatrix(m) {
        const imgData = this.#copyLastImgData();
        const data = imgData.data;
        
        for (let i = 0; i < data.length; i += 4) {
            const r = data[i];
            const g = data[i + 1];
            const b = data[i + 2];
            
            const newR = m[0] * r + m[1] * g + m[2] * b + m[3];
            const newG = m[4] * r + m[5] * g + m[6] * b + m[7];
            const newB = m[8] * r + m[9] * g + m[10] * b + m[11];
            
            data[i] = Math.min(255, Math.max(0, newR));
            data[i + 1] = Math.min(255, Math.max(0, newG));
            data[i + 2] = Math.min(255, Math.max(0, newB));
        }
        this.canvas.putImageData(imgData);
    }

    adjustSaturation(value) {
        const strength = 1 - value / 50;
        const rw = 0.299;
        const gw = 0.587;
        const bw = 0.114;
        const matrix = [
            rw * strength + (strength + 1), gw * strength, bw * strength, 0,
            rw * strength, gw * strength + (strength + 1), bw * strength, 0,
            rw * strength, gw * strength, bw * strength + (strength + 1), 0
        ];
        this.#applyColorMatrix(matrix);
    }

    adjustTemperature(value) {
        let temp = (value - 50) / 50;
        let r = 1, g = 1, b = 1;
        
        if (temp > 0) {
            r = 1 + temp * 0.5;
            g = 1 + temp * 0.2;
            b = 1 - temp * 0.3;
        } else if (temp < 0) {
            const cold = -temp;
            r = 1 - cold * 0.3;
            g = 1 + cold * 0.1;
            b = 1 + cold * 0.5;
        }
        
        const matrix = [
            r, 0, 0, 0,
            0, g, 0, 0,
            0, 0, b, 0
        ];
        this.#applyColorMatrix(matrix);
    }

    adjustNegative() {
        this.#applyColorMatrix(this.COLOR_MATRICES['negative']);
    }

    adjustSharpness(value) {
        const strength = value / 50;
        const center = 1 + strength * 4;
        const edge = -strength;

        const kernel = [
            0, edge, 0,
            edge, center, edge,
            0, edge, 0
        ];
        this.#convolution(kernel);
    }

    applyBlur(value) {
        const kSize = 1 + Math.round(value / 10);
        const kSize2 = kSize * kSize;

        const kernel = Array(kSize2).fill(1 / kSize2);

        this.#convolution(kernel);
    }

    applyMotionBlurHorizontal(value) {
        const kSize = 1 + Math.round(value / 10);

        const kernel = Array(kSize * kSize).fill(0);
        for (let i = 0; i < kSize; i++) {
            kernel[i] = 1 / kSize;
        }
        this.#convolution(kernel);
    }

    applyMotionBlurVertical(value) {
        const kSize = 1 + Math.round(value / 10);

        const kernel = Array(kSize * kSize).fill(0);
        for (let i = 0; i < kSize * kSize; i+=kSize) {
            kernel[i] = 1 / kSize;
        }
        this.#convolution(kernel);
    }

    #convolution(kernel) {
        const imgData = this.#copyLastImgData();
        const data = imgData.data;
        const width = imgData.width;
        const height = imgData.height;
        const kSize = Math.sqrt(kernel.length);
        const offset = Math.floor(kSize / 2);
        const upLim = offset;
        const lowLim = height - offset - 1;
        const leftLim = offset;
        const rightLim = width - offset - 1;

        const newImgData = new ImageData(width, height);
        const newData = newImgData.data;

        for (let y = 0; y < height; y++) {
            for (let x = 0; x < width; x++) {
                let r = 0, g = 0, b = 0;

                for (let dy = 0; dy < kSize; dy++) {
                    for (let dx = 0; dx < kSize; dx++) {
                        let pX = x + dx;
                        let pY = y + dy;

                        if (pX < leftLim) pX = leftLim;
                        if (pX > rightLim) pX = rightLim;
                        if (pY < upLim) pY = upLim;
                        if (pY > lowLim) pY = lowLim;

                        const i = (pY * width + pX) * 4;
                        const weight = kernel[dy * kSize + dx];

                        r += data[i] * weight;
                        g += data[i + 1] * weight;
                        b += data[i + 2] * weight;
                    }
                }
                const i = (y * width + x) * 4;
                newData[i] = Math.min(255, Math.max(0, r));
                newData[i + 1] = Math.min(255, Math.max(0, g));
                newData[i + 2] = Math.min(255, Math.max(0, b));
                newData[i + 3] = data[i + 3];
            }
        }
        this.canvas.putImageData(newImgData);
    }

    reduceNoise(value) {
        const imgData = this.#copyLastImgData();
        const data = imgData.data;

        const radius = Math.max(1, Math.floor(value / 10));
        const width = imgData.width;
        const height = imgData.height;
        const newImgData = new ImageData(width, height);
        const newData = newImgData.data;
        
        for (let y = radius; y < height - radius; y++) {
            for (let x = radius; x < width - radius; x++) {
                let rVals = [], gVals = [], bVals = [];
                
                for (let dy = -radius; dy <= radius; dy++) {
                    for (let dx = -radius; dx <= radius; dx++) {
                        const idx = ((y + dy) * width + (x + dx)) * 4;
                        rVals.push(data[idx]);
                        gVals.push(data[idx + 1]);
                        bVals.push(data[idx + 2]);
                    }
                }
                
                rVals = this.#sort(rVals);
                gVals = this.#sort(gVals);
                bVals = this.#sort(bVals);
                
                const mid = Math.floor(rVals.length / 2);
                const resultIdx = (y * width + x) * 4;
                
                newData[resultIdx] = rVals[mid];
                newData[resultIdx + 1] = gVals[mid];
                newData[resultIdx + 2] = bVals[mid];
                newData[resultIdx + 3] = data[resultIdx + 3];
            }
        }
        this.canvas.putImageData(newImgData);
    }

    #sort(arr) {
        const k = Math.max(...arr);
        const n = arr.length;
        const c = Array(k + 1).fill(0);
        for (const num of arr) {
            c[num] += 1;
        }
        for (let i = 1; i < k + 1; i++) {
            c[i] += c[i - 1];
        }
        const b = Array(n).fill(0);
        for (let i = n - 1; i > -1; i--) {
            const num = arr[i];
            const pos = c[num] - 1;
            b[pos] = num;
            c[num] -= 1;
        }
        return b;
    }

    applyChromaKey(value, hexColor) {
        const imgData = this.#copyLastImgData();
        const data = imgData.data;

        const match = hexColor.match(/\d+/g);
        const targetR = parseInt(match[0]);
        const targetG = parseInt(match[1]);
        const targetB = parseInt(match[2]);
        const limit = value * 2.55;

        for (let i = 0; i < data.length; i += 4) {
            const r = data[i];
            const g = data[i + 1];
            const b = data[i + 2];

            const distance = Math.hypot(r - targetR, g - targetG, b - targetB);

            if (distance < limit) {
                data[i + 3] = 0;
            }
        }
        this.canvas.putImageData(imgData);
    }

    removeBackground(area, value) {
        const imgData = this.#copyLastImgData();
        const data = imgData.data;
        const width = this.lastImgData.width;
        const height = this.lastImgData.height;

        const matrix = [];
        const objSize = (area.end.x - area.start.x) * (area.end.y - area.start.y);
        const bgSize = width * height - objSize;
        let bgColorsMap = new Map();
        let objColorsMap = new Map();
        const quantCoef = Math.floor((value + 1) / 10);

        for (let y = 0; y < height; y++) {
            for (let x = 0; x < width; x++) {
                const pixelPos = (y * width + x) * 4;

                let color = {
                    r: data[pixelPos],
                    g: data[pixelPos + 1],
                    b: data[pixelPos + 2],
                }
                color = this.#rgbToYCbCr(color);

                const vector = [
                    color.cb,
                    color.cr,
                    Math.round(x / width * (1.5 * value)),
                    Math.round(y / height * (1.5 * value))
                ]
                matrix.push(vector);

                const key = String(Math.round(color.cb / quantCoef) * quantCoef).padStart(3, '0')
                    + String(Math.round(color.cr / quantCoef) * quantCoef).padStart(3, '0');

                if (area.start.x < x && x < area.end.x && area.start.y < y && y < area.end.y) {
                    if (!objColorsMap.has(key)) {
                        objColorsMap.set(key, [1, vector[2], vector[3]]);
                    } else {
                        const value = objColorsMap.get(key);
                        objColorsMap.set(key, [
                            value[0] + 1,
                            value[1] + vector[2],
                            value[2] + vector[3]
                        ]);
                    }
                }
                else {
                    if (!bgColorsMap.has(key)) {
                        bgColorsMap.set(key, [1, vector[2], vector[3]]);
                    } else {
                        const value = bgColorsMap.get(key);
                        bgColorsMap.set(key, [
                            value[0] + 1,
                            value[1] + vector[2],
                            value[2] + vector[3]
                        ]);
                    }
                }
            }
        }

        let bgSortedColors = [...bgColorsMap.entries()].sort((a, b) => b[1][0] - a[1][0]);
        let objSortedColors = [...objColorsMap.entries()].sort((a, b) => b[1][0] - a[1][0]);

        let countObjCentroids = 0;
        let countBgCentroids = 0;
        const centroids = [];

        for (const el of bgSortedColors) {
            const count = el[1][0];
            const freq = count / bgSize;
            if (freq >= 0.05) {
                const color = el[0];
                const cb = parseInt(color.substr(0, 3));
                const cr = parseInt(color.substr(3, 3));

                centroids.push([
                    cb,
                    cr,
                    el[1][1] / count,
                    el[1][2] / count
                ]);
                countBgCentroids++;
                if (countBgCentroids >= 5) { break; }
            }
        }

        for (const el of objSortedColors) {
            const count = el[1][0];
            const freq = count / objSize;

            if (freq >= 0.05) {
                const color = el[0];
                const cb = parseInt(color.substr(0, 3));
                const cr = parseInt(color.substr(3, 3));

                if (centroids.some(vector => {
                    return vector[0] === cb && vector[1] === cr;
                })) { continue; }

                centroids.push([
                    cb,
                    cr,
                    el[1][1] / count,
                    el[1][2] / count
                ]);
                countObjCentroids++;
                if (countObjCentroids >= 10) { break; }
            }
        }

        const kMeans = new KMeansSegmentation(10);
        const labels = kMeans.fit(matrix, centroids);

        for (let i = 0; i < data.length; i += 4) {
            if (labels[i / 4] < countBgCentroids) {
                data[i + 3] = 0;
            }
        }

        this.canvas.putImageData(imgData);
    }

    #rgbToYCbCr(color) {
        const y = 0.299 * color.r + 0.587 * color.g + 0.114 * color.b;
        const cb = 128 - 0.168736 * color.r - 0.331264 * color.g + 0.5 * color.b;
        const cr = 128 + 0.5 * color.r - 0.418688 * color.g - 0.081312 * color.b;
        return {
            y : Math.round(y),
            cb : Math.round(cb),
            cr : Math.round(cr)
        };
    }

    #yCbCrToRgb(color) {
        let r = y + 1.402 * (color.cr - 128);
        let g = y - 0.344136 * (color.cb - 128) - 0.714136 * (color.cr - 128);
        let b = y + 1.772 * (color.cb - 128);

        return {
            r: Math.min(255, Math.max(0, Math.round(r))),
            g: Math.min(255, Math.max(0, Math.round(g))),
            b: Math.min(255, Math.max(0, Math.round(b)))
        };
    }

    cropImage(area) {
        const newWidth = area.end.x - area.start.x;
        const newHeight = area.end.y - area.start.y;
        const left = area.start.x;
        const top = area.start.y;

        const oldImgData = this.#copyLastImgData();
        const oldData = oldImgData.data;

        const newImdData = new ImageData(newWidth, newHeight);
        const newData = newImdData.data;

        for (let y = 0; y < newHeight; y++) {
            for (let x = 0; x < newWidth; x++) {
                const oldPos = ((y + top) * oldImgData.width + (x + left)) * 4;
                const newPos = (y * newWidth + x) * 4;

                newData[newPos] = oldData[oldPos];
                newData[newPos + 1] = oldData[oldPos + 1];
                newData[newPos + 2] = oldData[oldPos + 2];
                newData[newPos + 3] = oldData[oldPos + 3];
            }
        }
        this.canvas.putImageData(newImdData);
    }

    rotateImage(value) {
        const angle = (value - 50) * 0.02 * Math.PI;
        const cos = Math.cos(angle);
        const sin = Math.sin(angle);
        const absCos = Math.abs(cos);
        const absSin = Math.abs(sin);
        
        const oldImgData = this.#copyLastImgData();
        const oldWidth = oldImgData.width;
        const oldHeight = oldImgData.height;
        const oldData = oldImgData.data;
        const oldCenter = {
            x: oldWidth / 2,
            y: oldHeight / 2
        };
        
        const newWidth = Math.ceil(oldWidth * absCos + oldHeight * absSin);
        const newHeight = Math.ceil(oldWidth * absSin + oldHeight * absCos);

        const scale = Math.max(
            (newWidth * absCos + newHeight * absSin) / oldWidth,
            (newWidth * absSin + newHeight * absCos) / oldHeight
        );
        
        const newImgData = new ImageData(newWidth, newHeight);
        const newData = newImgData.data;
        const newCenter = {
            x: newWidth / 2,
            y: newHeight / 2
        };
        
        for (let y = 0; y < newHeight; y++) {
            for (let x = 0; x < newWidth; x++) {
                const nx = x - newCenter.x;
                const ny = y - newCenter.y;
                let ox = Math.round((nx * cos + ny * sin) / scale + oldCenter.x);
                let oy = Math.round((-nx * sin + ny * cos) / scale + oldCenter.y);
                
                if (ox >= 0 && ox < oldWidth && oy >= 0 && oy < oldHeight) {
                    const oldIdx = (oy * oldWidth + ox) * 4;
                    const newIdx = (y * newWidth + x) * 4;
                    
                    newData[newIdx] = oldData[oldIdx];
                    newData[newIdx + 1] = oldData[oldIdx + 1];
                    newData[newIdx + 2] = oldData[oldIdx + 2];
                    newData[newIdx + 3] = oldData[oldIdx + 3];
                }
            }
        }
        this.canvas.putImageData(newImgData);
    }
}