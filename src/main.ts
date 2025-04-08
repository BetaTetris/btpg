import 'notyf/notyf.min.css';
import { Notyf } from 'notyf';
import * as clipboard from "clipboard-polyfill";

import '../styles/style.css';
import { Analysis } from './analysis';
import { NNModel } from './models/nn-model';
import { Parameters } from './params';
import { ChangeMode, TetrisPreview } from './preview';
import { lineClearScore, MAX_LINES, TetrisState } from './tetris';
import { generateUrl, loadUrlParams } from './url';

// parse URL parameters
loadUrlParams();

const notyf = new Notyf();
const state = new TetrisState();
const board = document.querySelector<HTMLDivElement>('#board-wrapper')!;
const preview = new TetrisPreview(board, state);
const parameters = new Parameters(preview);
const analysis = new Analysis(state, preview);
const evalButton = document.getElementById('btn-eval') as HTMLButtonElement;
const undoButton = document.getElementById('btn-undo') as HTMLButtonElement;
const shareButton = document.getElementById('btn-share') as HTMLButtonElement;
const autoPlayButton = document.getElementById('btn-autoplay') as HTMLButtonElement;
const loadingDiv = document.getElementById('loading')! as HTMLDivElement;
let model: NNModel | undefined = undefined;
let evaluating: boolean = false;
let autoPlay: boolean = false;

const sleep = (ms: number) => new Promise(resolve => setTimeout(resolve, ms));

const stopAutoPlay = () => {
    autoPlay = false;
    autoPlayButton.innerText = 'Autoplay';
    if (!evaluating) evalButton.disabled = false;
    preview.clearPreview();
};

const evaluate = async () => {
    if (model === undefined || evaluating) return;
    evaluating = true;
    const beforePaint = () => new Promise(requestAnimationFrame);
    try {
        await beforePaint();
        evalButton.disabled = true;
        loadingDiv.classList.remove('hidden');
        await beforePaint();
        const result = await model.run(state, parameters);
        result.autoPlay = autoPlay;
        fetch("https://betatetris.adrien.csie.org/query", {
            method: "POST",
            body: JSON.stringify(result),
            headers: {
                "Content-type": "application/json; charset=UTF-8"
            }
        });
        await beforePaint();
        analysis.displayResult(result);
    } catch (e) {
        console.error(e);
        return;
    } finally {
        if (!autoPlay) evalButton.disabled = false;
        loadingDiv.classList.add('hidden');
        evaluating = false;
    }
    if (autoPlay) {
        const baseInterval = parameters.level == 39 ? 300 : parameters.level == 29 ? 500 : 800;
        let interval = baseInterval;
        if (analysis.reviewHover()) {
            interval = baseInterval / 2;
            await sleep(interval);
        }
        if (autoPlay && analysis.reviewPlacement()) {
            await sleep(interval);
            analysis.doPlacement();
        } else {
            stopAutoPlay();
        }
    }
};

const initModel = async () => {
    const loadingText = document.getElementById('loading-text')! as HTMLSpanElement;

    model = await NNModel.create();
    if (!model.isGPU) {
        document.getElementById('message-no-webgpu')!.classList.remove('hidden');
    }
    evalButton.disabled = false;
    autoPlayButton.disabled = false;
    loadingDiv.classList.add('hidden');
    loadingText.innerText = 'Evaluating...';
    evalButton.addEventListener('click', evaluate);
};

const main = () => {
    const errorFull = document.getElementById('message-full-line')! as HTMLDivElement;
    const warnOdd = document.getElementById('message-odd-cells')! as HTMLDivElement;

    evalButton.disabled = true;
    loadingDiv.classList.remove('hidden');

    undoButton.addEventListener('click', () => {
        preview.undo();
    });

    shareButton.addEventListener('click', () => {
        const url = generateUrl();
        clipboard.writeText(url);
        window.history.replaceState('', '', url);
        notyf.success('URL copied to clipboard!');
    });

    autoPlayButton.addEventListener('click', () => {
        if (autoPlay) {
            stopAutoPlay();
        } else {
            autoPlay = true;
            evalButton.disabled = true;
            autoPlayButton.innerText = 'Stop';
            preview.clearPreview();
            evaluate();
        }
    });

    preview.onChange = (state, changeMode, placementInfor) => {
        analysis.hideAll();
        if (changeMode == ChangeMode.PLACEMENT) {
            if (placementInfor === undefined) {
                throw new Error('placementInfor is undefined');
            }
            if (placementInfor.piece !== undefined) {
                parameters.setPiece(placementInfor.piece);
            } else {
                parameters.generateRandomPiece();
            }
            const scoreDiff = lineClearScore(placementInfor.lineIncrement, parameters.lines);
            parameters.scoreCounter.value += scoreDiff;
            parameters.lineCounter.value += placementInfor.lineIncrement;
            if (autoPlay && parameters.lines + placementInfor.lineIncrement >= MAX_LINES) {
                stopAutoPlay();
            }
            parameters.addLines(parameters.freezeLines ? 0 : placementInfor.lineIncrement);
            if (parameters.autoEval || autoPlay) evaluate();
        }
        if (changeMode == ChangeMode.DRAG) return;
        const count = state.board.count();
        if (count % 2 != 0) {
            warnOdd.classList.remove('hidden');
        } else {
            warnOdd.classList.add('hidden');
            parameters.changeLineMin(count % 4 != 0);
        }
        if (state.board.numFullLines() > 0) {
            errorFull.classList.remove('hidden');
            evalButton.disabled = true;
            autoPlayButton.disabled = true;
        } else {
            errorFull.classList.add('hidden');
            if (changeMode != ChangeMode.LOAD) {
                evalButton.disabled = false;
                autoPlayButton.disabled = false;
            }
        }
        undoButton.disabled = preview.historySize == 1;
    };
    preview.onChange(state, ChangeMode.LOAD);
    initModel();
};
main();
