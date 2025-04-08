import { TapSpeed } from '../wasm/tetris';
import { TetrisPreview } from './preview';
import { generateRandomPiece, MAX_LINES, module, PIECE_NAMES, TRANSITION_PROBS } from './tetris';
import { Select, Checkbox, wrapSelectInField, NumberSelector } from './components';

export class Parameters {
    private pieceSelect: Select<number>;
    private lvlSelect: Select<number>;
    private linesInput: NumberSelector;
    private modelSelect: Select<number>;
    private hzSelect: Select<TapSpeed>;
    private reactionSelect: Select<number>;
    private aggressionSelect: Select<number>;
    private autoEvalCheckbox: Checkbox;
    private freezeLinesCheckbox: Checkbox;
    public scoreCounter: NumberSelector;
    public lineCounter: NumberSelector;
    get piece(): number {
        return this.pieceSelect.value;
    }
    get level(): number {
        return this.lvlSelect.value;
    }
    get model(): number {
        return this.modelSelect.value;
    }
    get tapSpeed(): TapSpeed {
        return this.hzSelect.value;
    }
    get reactionTime(): number {
        return this.reactionSelect.value;
    }
    get aggression(): number {
        return this.aggressionSelect.value;
    }
    get autoEval(): boolean {
        return this.autoEvalCheckbox.value;
    }
    get freezeLines(): boolean {
        return this.freezeLinesCheckbox.value;
    }

    private setLines(value: number, checkParity: Boolean = true) {
        if (checkParity) {
            value = (value & 0xfffffffe) + (this.linesInput.element.min == '1' ? 1 : 0);
        }
        this.linesInput.value = value;
        let level = 19 + ~~((value - 130) / 10);
        if (value < 130) {
            this.lvlSelect.selectedIndex = 0;
            level = 18;
        } else if (value < 230) {
            this.lvlSelect.selectedIndex = 1;
        } else if (value < 330) {
            this.lvlSelect.selectedIndex = 2;
        } else {
            this.lvlSelect.selectedIndex = 3;
        }
        this.lvlSelect.saveValue();
        this.preview.setLevel(level);
    }

    get lines(): number {
        return this.linesInput.value;
    }
    set lines(value: number) {
        this.setLines(value);
    }

    constructor(
        private preview: TetrisPreview,
    ) {
        const gameConfig = document.getElementById('game-param-config')! as HTMLDivElement;
        const modelConfig = document.getElementById('model-param-config')! as HTMLDivElement;
        const websiteConfig = document.getElementById('website-config')! as HTMLDivElement;

        /// Game config
        this.pieceSelect = new Select(
            'current-piece-select',
            [...PIECE_NAMES.keys()],
            PIECE_NAMES,
            Math.floor(Math.random() * 7),
        );
        const pieceField = wrapSelectInField(this.pieceSelect.element, 'Current piece:');

        const LEVELS = [18, 19, 29, 39];
        this.lvlSelect = new Select(
            'lvl-select',
            LEVELS,
            LEVELS.map((lvl) => lvl.toString()),
            0,
            false,
        );
        const lvlField = wrapSelectInField(this.lvlSelect.element, 'Level speed:');

        this.linesInput = new NumberSelector('lines-input', 0, MAX_LINES - 1, 2, 30);
        const linesField = wrapSelectInField(this.linesInput.element, 'Lines:');

        gameConfig.appendChild(pieceField);
        gameConfig.appendChild(lvlField);
        gameConfig.appendChild(linesField);

        /// Model config
        this.lvlSelect.onchange = (_: Event, level: number) => {
            if (level == 18) {
                this.lines = 30;
            } else if (level == 19) {
                this.lines = 160;
            } else if (level == 29) {
                this.lines = 260;
            } else if (level == 39) {
                this.lines = 360;
            }
        }

        const lineInput = (_: Event) => {
            this.setLines(this.linesInput.value, false);
        }
        const lineChange = (_: Event) => {
            this.setLines(this.linesInput.value, true);
        }
        this.linesInput.element.addEventListener('input', lineInput);
        this.linesInput.element.addEventListener('change', lineChange);
        this.setLines(this.linesInput.value, true);

        /// Model config
        this.modelSelect = new Select(
            'model-select',
            [0, 1],
            ['Normal', 'Aggro'],
            0,
        );
        const modelField = wrapSelectInField(this.modelSelect.element, 'Model:');

        this.hzSelect = new Select(
            'hz-select',
            [module.TapSpeed.kTap10Hz, module.TapSpeed.kTap12Hz, module.TapSpeed.kTap15Hz,
                module.TapSpeed.kTap20Hz, module.TapSpeed.kTap24Hz, module.TapSpeed.kTap30Hz,
                module.TapSpeed.kSlow5],
            ['10hz', '12hz', '15hz', '20hz', '24hz', '30hz', 'slow 5 tap'],
            4,
        );
        const hzField = wrapSelectInField(this.hzSelect.element, 'Tap speed:');

        const REACTION_TIMES = [0, 18, 21, 24, 30, 61];
        this.reactionSelect = new Select(
            'reaction-select',
            REACTION_TIMES,
            REACTION_TIMES.map((delay) => delay == 61 ? 'No adj' :
                Math.round((delay * 1000) / 60).toString() + 'ms (' + delay + 'f)'),
            2,
        );
        const reactionField = wrapSelectInField(this.reactionSelect.element, 'Reaction time:');

        this.aggressionSelect = new Select(
            'aggro-select',
            [2, 1, 0],
            ['Low', 'Medium', 'High'],
            2,
        );
        const aggroField = wrapSelectInField(this.aggressionSelect.element, 'Aggression:');

        modelConfig.appendChild(modelField);
        modelConfig.appendChild(hzField);
        modelConfig.appendChild(reactionField);
        modelConfig.appendChild(aggroField);

        this.autoEvalCheckbox = new Checkbox('auto-eval', 'Auto evaluate:');
        this.freezeLinesCheckbox = new Checkbox('freeze-lines', 'Freeze line count:');
        this.scoreCounter = new NumberSelector('score-counter', 0, 99999999);
        this.lineCounter = new NumberSelector('line-counter', 0, 99999);
        const scoreField = wrapSelectInField(this.scoreCounter.element, 'Score counter:');
        const lineField = wrapSelectInField(this.lineCounter.element, 'Line counter:');
        websiteConfig.appendChild(this.autoEvalCheckbox.wrapper);
        websiteConfig.appendChild(this.freezeLinesCheckbox.wrapper);
        websiteConfig.appendChild(scoreField);
        websiteConfig.appendChild(lineField);
    }

    public setPiece(piece: number) {
        this.pieceSelect.selectedIndex = piece;
    }

    public generateRandomPiece() {
        this.setPiece(generateRandomPiece(this.piece));
    }

    public changeLineMin(isOdd: Boolean) {
        this.linesInput.element.min = isOdd ? '1' : '0';
        this.linesInput.value = (this.lines & 0xfffffffe) + (isOdd ? 1 : 0);
    }

    public addLines(lines: number) {
        this.linesInput.element.min = (this.lines + lines) % 2 ? '1' : '0';
        this.lines = Math.min(MAX_LINES - 1, this.lines + lines);
    }
};