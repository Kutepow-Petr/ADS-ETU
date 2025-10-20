export class Cell {
    cell
    x
    y
    linkedTile

    constructor(gridElement, x, y) {
        this.cell = document.createElement("div")
        this.cell.classList.add("cell")
        this.x = x
        this.y = y
        gridElement.append(this.cell)
    }

    linkTile(tile) {
        tile.setXY(this.x, this.y)
        this.linkedTile = tile
    }

    isEmpty() {
        return !this.linkedTile
    }

    canAccept(newTile) {
        return (this.isEmpty() || this.linkedTile.value === newTile.value)
    }

    async mergeTiles(tile) {
        tile.setXY(this.x, this.y)
        await new Promise(resolve => setTimeout(resolve, 100))
        this.linkedTile.setValue(this.linkedTile.value + tile.value)
        tile.tile.remove()
    }
}