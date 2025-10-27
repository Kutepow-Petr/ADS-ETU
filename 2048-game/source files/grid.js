import { Cell } from "./cell.js";

export class Grid {
    gridSize
    cells = []
    cellsGroupedByColumn = []
    cellsGroupedByLine = []
    cellsGroupedByReversedColumn = []
    cellsGroupedByReversedLine = []

    constructor(gridElement, gridSize) {
        this.gridSize = gridSize
        gridElement.style.setProperty("--grid-size", gridSize)

        for (let i = 0; i < this.gridSize; i++) {
            for (let j = 0; j < this.gridSize; j++) {
                this.cells.push(new Cell(gridElement, j, i))
            }
        }

        this.groupCellByColumn()
        this.groupCellByLine()
        this.groupCellByReversedColumn()
        this.groupCellByReversedLine()
    }

    getRandomEmptyCell() {
        const emptyCells = this.cells.filter(cell => cell.isEmpty())
        const randomIndex = Math.floor(Math.random() * emptyCells.length)
        return emptyCells[randomIndex]
    }

    groupCellByColumn() {
        for (let i = 0; i < this.gridSize; i++) {
            const column = []
            for (let j = 0; j < this.gridSize; j++) {
                column.push(this.cells[this.gridSize * j + i])
            }
            this.cellsGroupedByColumn.push(column)
        }
    }
    groupCellByLine() {
        for (let i = 0; i < this.gridSize; i++) {
            const line = []
            for (let j = 0; j < this.gridSize; j++) {
                line.push(this.cells[this.gridSize * i + j])
            }
            this.cellsGroupedByLine.push(line)
        }
    }
    groupCellByReversedColumn() {
        for (let i = 0; i < this.gridSize; i++) {
            const reversedColumn = []
            for (let j = this.gridSize - 1; j >= 0; j--) {
                reversedColumn.push(this.cells[this.gridSize * j + i])
            }
            this.cellsGroupedByReversedColumn.push(reversedColumn)
        }
    }
    groupCellByReversedLine() {
        for (let i = 0; i < this.gridSize; i++) {
            const reversedLine = []
            for (let j = this.gridSize - 1; j >= 0; j--) {
                reversedLine.push(this.cells[this.gridSize * i + j])
            }
            this.cellsGroupedByReversedLine.push(reversedLine)
        }
    }

    findMaxValue() {
        let maxValue = 0
        this.cells.forEach(cell => {
            if (cell.linkedTile) {
                maxValue = maxValue < cell.linkedTile.value ? cell.linkedTile.value : maxValue
            }
        })
        return maxValue
    }

    moveUpTiles(oldGrid) {
        for (let i = 0; i < oldGrid.gridSize; i++) {
            for (let j = 0; j < oldGrid.gridSize; j++) {
                if (oldGrid.cells[i * oldGrid.gridSize + j].linkedTile) {
                    this.cells[i * this.gridSize + (j + 1)].linkTile(oldGrid.cells[i * oldGrid.gridSize + j].linkedTile)
                }
            }
        }
    }

    moveDownTiles(oldGrid) {
        for (let i = 0; i < oldGrid.gridSize; i++) {
            for (let j = 0; j < oldGrid.gridSize; j++) {
                if (oldGrid.cells[i * oldGrid.gridSize + j].linkedTile) {
                    this.cells[(i + 1) * this.gridSize + j].linkTile(oldGrid.cells[i * oldGrid.gridSize + j].linkedTile)
                }
            }
        }
    }

    moveLeftTiles(oldGrid) {
        for (let i = 0; i < oldGrid.gridSize; i++) {
            for (let j = 0; j < oldGrid.gridSize; j++) {
                if (oldGrid.cells[i * oldGrid.gridSize + j].linkedTile) {
                    this.cells[i * this.gridSize + j].linkTile(oldGrid.cells[i * oldGrid.gridSize + j].linkedTile)
                }
            }
        }
    }

    moveRightTiles(oldGrid) {
        for (let i = 0; i < oldGrid.gridSize; i++) {
            for (let j = 0; j < oldGrid.gridSize; j++) {
                if (oldGrid.cells[i * oldGrid.gridSize + j].linkedTile) {
                    this.cells[(i + 1) * this.gridSize + (j + 1)].linkTile(oldGrid.cells[i * oldGrid.gridSize + j].linkedTile)
                }
            }
        }
    }

    removeFromDOM() {
        this.cells.forEach(cell => {
            cell.cell.remove()
        })
    }
}