# minimal-fftw
Minimal FFTW application

## Usage

```
minimal_fft filein fileout fftlength [nthreads]
```

### Parameters

Parameter | Meaning
----------|--------
filein    | Input file. Must be complex samples, 16bit signed integer I, 16bit signed integer Q, and so on
fileout   | Output file. Will be float32, containing |•|² of each FFT bin, of **one** FFT
fftlength | Length of the FFT
nthreads  | Number of threads that FFTW will spawn (defaults to 4, optional)

### Building from Source

You'll need FFTWf (the single precision version of FFTW) with multithreading enabled installed; then

```
make
```

from the source directory should work.

