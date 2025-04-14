# [btpg (BetaTetris Playground)](https://betatetris.github.io/btpg/)

A frontend for using the [BetaTetris](https://github.com/BetaTetris/betatetris-tablebase) AI in the browser, through the magic of the [ONNX Runtime](https://onnxruntime.ai/).

## Setup

### Static files

Download the `dist.zip` file from the [Releases](https://github.com/BetaTetris/btpg/releases) page, decompress it and serve the directory as static files on any webserver.

### Manual build

Clone the repository, then run `npm install` in the root directory. To compile the wasm modules, navigate to the `wasm` directory and run `./build.sh`. This will require Emscripten, which can be installed following the instructions [here](https://emscripten.org/docs/getting_started/downloads.html).

From here, simply run the server in dev mode with `npm run dev`. Alternatively, run `npm run build` to build the website and run the server in production mode with `npm run preview`.

## Acknowledgements

[fractal161](https://github.com/fractal161) for creating this website, and [Adrien Wu](https://github.com/adrien1018) for creating BetaTetris.
