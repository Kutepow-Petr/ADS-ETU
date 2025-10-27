export class Tile {
    tile
    x
    y
    value

    constructor(gridElement) {
        this.tile = document.createElement("div")
        this.tile.classList.add("tile")
        this.setValue(Math.random() > 0.5 ? 2 : 4)
        gridElement.append(this.tile)
    }

    setXY(x, y) {
        this.x = x
        this.y = y
        this.tile.style.setProperty("--x", x)
        this.tile.style.setProperty("--y", y)
    }

    setValue(value) {
        this.value = value
        this.tile.textContent = value
        const color = Math.log2(value) * 34
        this.tile.style.setProperty("--bg-color", `hsl(${color}, 40%, 60%)`)
    }
}