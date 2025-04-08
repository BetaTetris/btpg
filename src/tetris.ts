import Module, { Board } from '../wasm/tetris.js';

export const module = await Module();

export const BOARD_HEIGHT = 20;
export const BOARD_WIDTH = 10;

export enum Piece {
    T = 0,
    J = 1,
    Z = 2,
    O = 3,
    S = 4,
    L = 5,
    I = 6,
};
export const PIECE_NAMES = ['T', 'J', 'Z', 'O', 'S', 'L', 'I'];
export const TRANSITION_PROBS = [
    [1, 5, 6, 5, 5, 5, 5],
    [6, 1, 5, 5, 5, 5, 5],
    [5, 6, 1, 5, 5, 5, 5],
    [5, 5, 5, 2, 5, 5, 5],
    [5, 5, 5, 5, 2, 5, 5],
    [6, 5, 5, 5, 5, 1, 5],
    [5, 5, 5, 5, 6, 5, 1],
];
export const MAX_LINES = 430;

export function generateRandomPiece(piece: number): number {
    const transition_probs = TRANSITION_PROBS[piece];
    const random = Math.random() * 32;
    let sum = 0;
    for (let i = 0; i < transition_probs.length; i++) {
        sum += transition_probs[i];
        if (random < sum) return i;
    }
    return 0; // should never happen
}

export function lineClearScore(clearType: number, lines: number): number {
    const linesAfter = lines + clearType;
    const level = linesAfter < 130 ? 18 : ~~((linesAfter - 130) / 10) + 19;
    const LINE_CLEAR_SCORES = [0, 40, 100, 300, 1200];
    return LINE_CLEAR_SCORES[clearType] * (level + 1);
}

// coordinate system is right+down, i.e. lower-indexed rows are higher up
export class TetrisState {
    constructor(
        public readonly board: Board = new module.Board,
    ) {}

    public setCell(x: number, y: number, value: boolean) {
        value ? this.board.setCellFilled(x, y) : this.board.setCellEmpty(x, y);
    }

    public getCell(x: number, y: number): boolean {
        return this.board.isCellFilled(x, y);
    }
}

export class Placement {
    public r: number;
    public x: number;
    public y: number;

    constructor(
        r: number,
        x?: number,
        y?: number,
    ) {
        if (x === undefined || y === undefined) {
            this.r = ~~(r / 200);
            this.x = ~~(r / 10) % 20;
            this.y = r % 10;
        } else {
            this.r = r;
            this.x = x;
            this.y = y;
        }
    }
}
