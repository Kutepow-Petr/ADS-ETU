export class KMeansSegmentation {
    constructor(maxIterations = 10) {
        this.k;
        this.maxIterations = maxIterations;
        this.centroids = [];
        this.labels = [];
    }

    euclideanDistance(vec1, vec2) {
        let sum = 0;
        for (let i = 0; i < vec1.length; i++) {
            const diff = vec1[i] - vec2[i];
            sum += diff * diff;
        }
        return Math.sqrt(sum);
    }

    assignClusters(data, centroids) {
        const labels = new Array(data.length);
        for (let i = 0; i < data.length; i++) {
            let minDistance = Infinity;
            let closestCluster = 0;

            for (let j = 0; j < this.k; j++) {
                const distance = this.euclideanDistance(data[i], centroids[j]);
                if (distance < minDistance) {
                    minDistance = distance;
                    closestCluster = j;
                }
            }
            labels[i] = closestCluster;
        }
        return labels;
    }

    updateCentroids(data, labels) {
        const vectorLen = data[0].length;
        const sums = Array(this.k).fill().map(() => Array(vectorLen).fill(0));
        const counts = Array(this.k).fill(0);
        
        for (let i = 0; i < data.length; i++) {
            const clusterId = labels[i];
            const vector = data[i];
            for (let j = 0; j < vectorLen; j++) {
                sums[clusterId][j] += vector[j];
            }
            counts[clusterId]++;
        }
        
        const newCentroids = [];
        for (let i = 0; i < this.k; i++) {
            if (counts[i] > 0) {
                const newCentroid = Array(vectorLen).fill(0);
                for (let j = 0; j < vectorLen; j++) {
                    newCentroid[j] = sums[i][j] / counts[i];
                }
                newCentroids.push(newCentroid);
            } else {
                newCentroids.push([...this.centroids[i]]);
            }
        }
        return newCentroids;
    }

    hasConverged(oldCentroids, newCentroids, tolerance = 0.01) {
        for (let i = 0; i < this.k; i++) {
            const distance = this.euclideanDistance(oldCentroids[i], newCentroids[i]);
            if (distance > tolerance) return false;
        }
        return true;
    }

    fit(data, initialCentroids = null) {
        this.centroids = initialCentroids.map(c => [...c]);
        this.k = this.centroids.length;
        
        for (let iteration = 0; iteration < this.maxIterations; iteration++) {
            this.labels = this.assignClusters(data, this.centroids);
            const newCentroids = this.updateCentroids(data, this.labels);
            
            if (this.hasConverged(this.centroids, newCentroids)) {
                break;
            }
            this.centroids = newCentroids;
        }
        return this.labels;
    }
}