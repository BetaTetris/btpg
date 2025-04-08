const urlParams = new URLSearchParams(window.location.search);
const PARAM_KEYS = {
    'piece': 'field-current-piece-select',
    'lines': 'field-lines-input',
    'model': 'field-model-select',
    'hz': 'field-hz-select',
    'reaction': 'field-reaction-select',
    'aggro': 'field-aggro-select',
    'freeze': 'field-freeze-lines',
    'board': 'tetris-preview-state',
}

export function loadUrlParams() {
    // read URL parameters and load them into local storage
    for (const [key, localDbKey] of Object.entries(PARAM_KEYS)) {
        const paramValue = urlParams.get(key);
        if (paramValue) localStorage.setItem(localDbKey, paramValue);
    }
}

export function generateUrl(): string {
    // generate a URL with the current parameters
    const params = new URLSearchParams();
    for (const [key, localDbKey] of Object.entries(PARAM_KEYS)) {
        const value = localStorage.getItem(localDbKey);
        if (value) params.set(key, value);
    }
    return window.location.origin + window.location.pathname + '?' + params.toString();
}