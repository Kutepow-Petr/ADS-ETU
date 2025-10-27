import { Grid } from "./grid.js";
import { Tile } from "./tile.js"

const gameBoard = document.getElementById("game-board")

let gridSize = 2
let valueForExtension = 16
let grid = new Grid(gameBoard, gridSize)

grid.getRandomEmptyCell().linkTile(new Tile(gameBoard))
if (isMobileDevice()) {
    document.addEventListener('touchmove', function (event) {
                if (window.scrollY === 0) {
                    event.preventDefault();
                }
            }, { passive: false });
    setupInputByTouchscreen()
} else {
    setupInputByKeyboard()
}

function setupInputByKeyboard() {
    window.addEventListener("keydown", (event) => {
        makeMove(event.key.slice(5))
        setupInputByKeyboard()
    }, { once: true })
}

function setupInputByTouchscreen() {
    let startX
    let startY
    window.addEventListener("touchstart", (event) => {
        startX = event.touches[0].clientX;
        startY = event.touches[0].clientY;
    }, { once: true })
    let diffX
    let diffY
    window.addEventListener("touchend", (event) => {
        diffX = event.changedTouches[0].clientX - startX;
        diffY = event.changedTouches[0].clientY - startY;
        if (Math.abs(diffY) > Math.abs(diffX)) {
            if (diffY < 0) {
                makeMove("Up")
                setupInputByTouchscreen()
            }
            else {
                makeMove("Down")
                setupInputByTouchscreen()
            }
        } else {
            if (diffX < 0) {
                makeMove("Left")
                setupInputByTouchscreen()
            }
            else {
                makeMove("Right")
                setupInputByTouchscreen()
            }
        }
    }, { once: true })
}

async function makeMove(direction) {
    switch (direction) {
        case "Up":
            if (!canMoveUp()) {
                return
            }
            moveUp();
            break
        case "Down":
            if (!canMoveDown()) {
                return
            }
            moveDown();
            break
        case "Left":
            if (!canMoveLeft()) {
                return
            }
            moveLeft()
            break
        case "Right":
            if (!canMoveRight()) {
                return
            }
            moveRight()
            break
    }

    await new Promise(resolve => setTimeout(resolve, 100))

    const maxValue = grid.findMaxValue()
    if (maxValue === 2048) {
        await new Promise(resolve => setTimeout(resolve, 100))
        alert("YOU WON!")
        return
    } else if (maxValue === valueForExtension) {
        increasingGrid(direction)
    }

    grid.getRandomEmptyCell().linkTile(new Tile(gameBoard))

    if (!canMoveUp() && !canMoveDown() && !canMoveLeft() && !canMoveRight()) {
        await new Promise(resolve => setTimeout(resolve, 100))
        alert("Try again!")
        location.reload(true)
        return
    }
}

function moveUp() {
    slideTiles(grid.cellsGroupedByColumn)
}

function moveDown() {
    slideTiles(grid.cellsGroupedByReversedColumn)
}

function moveLeft() {
    slideTiles(grid.cellsGroupedByLine)
}

function moveRight() {
    slideTiles(grid.cellsGroupedByReversedLine)
}

function slideTiles(groupedCells) {
    for (let i = 0; i < groupedCells.length; i++) {
        const group = groupedCells[i]

        let usedCells = []

        for (let j = 1; j < group.length; j++) {
            if (group[j].isEmpty()) {
                continue
            }
            const cellWithTile = group[j]

            let targetCell
            for (let k = j - 1; k >= 0 && group[k].canAccept(cellWithTile.linkedTile); k--) {
                targetCell = group[k]
            }

            if (!targetCell) {
                continue
            } else if (targetCell.isEmpty()) {
                targetCell.linkTile(cellWithTile.linkedTile)
            } else if (usedCells.includes(targetCell)) {
                continue
            } else {
                targetCell.mergeTiles(cellWithTile.linkedTile)
                usedCells.push(targetCell)
            }
            cellWithTile.linkedTile = null
        }
    }
}

function canMoveUp() {
    return canMove(grid.cellsGroupedByColumn)
}

function canMoveDown() {
    return canMove(grid.cellsGroupedByReversedColumn)
}

function canMoveLeft() {
    return canMove(grid.cellsGroupedByLine)
}

function canMoveRight() {
    return canMove(grid.cellsGroupedByReversedLine)
}

function canMove(groupedCells) {
    for (let i = 0; i < groupedCells.length; i++) {
        const group = groupedCells[i]

        for (let j = 1; j < group.length; j++) {
            if (group[j].isEmpty()) {
                continue
            }
            const cellWithTile = group[j]

            const targetCell = group[j - 1]
            if (targetCell.canAccept(cellWithTile.linkedTile)) {
                return true
            }
        }
    }
    return false
}

function increasingGrid(direction) {
    gridSize++
        valueForExtension *= 4

        const oldGrid = grid

        grid = new Grid(gameBoard, gridSize)
        switch (direction) {
            case "Up":
                grid.moveUpTiles(oldGrid)
                break
            case "Down":
                grid.moveDownTiles(oldGrid)
                break
            case "Left":
                grid.moveLeftTiles(oldGrid)
                break
            case "Right":
                grid.moveRightTiles(oldGrid)
                break
        }

        oldGrid.removeFromDOM()
}

function isMobileDevice() {
    const userAgent = navigator.userAgent.toLowerCase();

    const mobileKeywords = [
        'android', 'iphone', 'ipod', 'windows phone', 'mobile', 'tablet', 'ipad'
    ];

    return mobileKeywords.some(keyword => userAgent.includes(keyword));
}