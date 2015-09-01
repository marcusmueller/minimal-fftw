CFLAGS+=-O3

all: minimal_fft

minimal_fft: minimal_fft.c
	cc --std=c99 -o minimal_fft -lfftw3f_threads -lfftw3f -lm --std=c99 ${CFLAGS} minimal_fft.c
clean:
	rm minimal_fft
