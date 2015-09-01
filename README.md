# minimal-fftw
Minimal FFTW application

## Usage

```
minimal_fft filein fileout fftlength [nthreads]
```

If you want to have timing characters, you might want to prepend that command with `time`, which is installed on many Unix systems (Linux, OS X) by default.

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


### Getting samples

If you have a USRP and UHD installed

```
rx_samples_to_file --file inputsamples --type short --nsamps 100000000 --rate 20e6 --freq 1.8e9 --gain 30
```

would give you samples in the right format in the file `inputsamples`, with sampling rate 20 MS/s, taken from 1.8GHz, for 100MS/(20MS/s) = 5s duration.
