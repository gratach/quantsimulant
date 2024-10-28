# Quantsimulant 

This is a web application for simulating the one-dimensional schrödinger equation in a custom time-dependent potential. The simulation is done by solving the time-dependent schrödinger equation using the runge-kutta method. The simulation is done in the browser using WebAssembly.

The website is hosted on [quantsimulant.de](https://quantsimulant.de).

## Building the project

This project uses the [emscripten](https://emscripten.org/) compiler to compile the C code to WebAssembly.

Download the latest version of emscripten and activate it by running the following commands in the installation directory:

```bash
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

Build the project by running the following command in the root directory of the project:
```bash
./erstelle
```

## Hosting the project

The www directory contains the files that need to be hosted on a web server. Use the following command to start a simple web server in the www directory:

```bash
./server
```

The website can now be accessed by opening a browser and navigating to `localhost:9090`.



