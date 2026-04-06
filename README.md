# [btpg (BetaTetris Playground)](https://betatetris.github.io/btpg/)

A frontend for using the [BetaTetris](https://github.com/BetaTetris/betatetris-tablebase) AI in the browser, through the magic of the [ONNX Runtime](https://onnxruntime.ai/).

## Setup

### Static files

Download the `dist.zip` file from the [Releases](https://github.com/BetaTetris/btpg/releases) page, decompress it and serve the directory as static files on any webserver.

### Manual build

1. Clone the repository, then run `npm install` in the root directory.
2. Compile the wasm modules by `./wasm/build.sh`.
3. Run the server in dev mode with `npm run dev`. Alternatively, run `npm run build && npm run preview` to build the website and run the server in production mode.

## Acknowledgements

[fractal161](https://github.com/fractal161) for creating this website, and [Adrien Wu](https://github.com/adrien1018) for creating BetaTetris.
