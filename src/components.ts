export class Select<T> {
    private select: HTMLSelectElement;
    get element(): HTMLSelectElement {
        return this.select;
    }
    get value(): T {
        return this.keys[this.select.selectedIndex];
    }
    public onchange: (e: Event, v: T) => void = () => {};

    set selectedIndex(i: number) {
        this.select.selectedIndex = i;
    }
    get selectedIndex(): number {
        return this.select.selectedIndex;
    }

    public saveValue() {
        if (!this.save) return;
        localStorage.setItem('field-' + this.select.id, this.select.value);
    }

    public invokeChange() {
        var event = new Event('change');
        this.select.dispatchEvent(event);
    }

    constructor(
        id: string,
        private keys: Array<T>,
        texts: Array<string>,
        defaultOption: number = 0,
        private save: boolean = true,
    ) {
        this.select = document.createElement('select');
        this.select.id = id;
        if (keys.length != texts.length) {
            throw new Error('keys and texts must have the same length');
        }
        for (let i = 0; i < keys.length; i++) {
            const option = document.createElement('option');
            option.value = i.toString();
            option.innerText = texts[i];
            this.select.appendChild(option);
        }
        const savedOption = localStorage.getItem('field-' + id);
        if (this.save && savedOption !== null) {
            this.select.selectedIndex = parseInt(savedOption);
        } else {
            this.select.selectedIndex = defaultOption;
            localStorage.setItem('field-' + id, defaultOption.toString());
        }
        this.select.addEventListener('change', (e: Event) => {
            this.saveValue();
            this.onchange(e, this.value);
        });
    }
}

export class Checkbox {
    private checkbox: HTMLInputElement;
    private _wrapper: HTMLDivElement;
    get wrapper(): HTMLDivElement {
        return this._wrapper;
    }
    get element(): HTMLInputElement {
        return this.checkbox;
    }
    get value(): boolean {
        return this.checkbox.checked;
    }
    set value(v: boolean) {
        this.checkbox.checked = v;
    }

    constructor(id: string, labelText: string) {
        this.checkbox = document.createElement('input');
        this.checkbox.type = 'checkbox';
        this.checkbox.id = id;
        this.checkbox.checked = localStorage.getItem('field-' + id) === '1';
        this.checkbox.addEventListener('change', (_) => {
            localStorage.setItem('field-' + id, this.checkbox.checked ? '1' : '0');
        });
        const label = document.createElement('label');
        label.textContent = labelText;
        label.htmlFor = id;
        const div = document.createElement('div');
        div.classList.add('field');
        div.appendChild(label);
        div.appendChild(this.checkbox);
        this._wrapper = div;
    }
}

export class NumberSelector {
    private input: HTMLInputElement;
    get element(): HTMLInputElement {
        return this.input;
    }
    get value(): number {
        return parseInt(this.input.value);
    }
    set value(v: number) {
        this.input.value = v.toString();
        localStorage.setItem('field-' + this.id, this.input.value);
    }

    constructor(
        private id: string,
        min: number,
        max: number,
        step: number = 1,
        defaultValue: number = 0,
    ) {
        this.input = document.createElement('input');
        this.input.type = 'number';
        this.input.id = id;
        this.input.min = min.toString();
        this.input.max = max.toString();
        this.input.step = step.toString();
        const savedValue = localStorage.getItem('field-' + id);
        if (savedValue !== null) {
            this.input.value = savedValue;
        } else {
            this.input.value = defaultValue.toString();
            localStorage.setItem('field-' + id, defaultValue.toString());
        }
    }
};

export function wrapSelectInField(
    select: HTMLSelectElement | HTMLInputElement,
    labelText: string,
): HTMLDivElement {
    const label = document.createElement('label');
    label.textContent = labelText;
    label.htmlFor = select.id;
    const div = document.createElement('div');
    div.classList.add('field');
    div.appendChild(label);
    div.appendChild(select);
    return div;
}
