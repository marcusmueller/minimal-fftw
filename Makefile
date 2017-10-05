CFLAGS+=-O3

all: minimal_fft

minimal_fft: minimal_fft.c
	${CC} --std=c99 -o minimal_fft -lfftw3f_threads -lfftw3f -lm `pkgconf --libs --cflags volk` --std=c99 ${CFLAGS} minimal_fft.c
clean:
	rm minimal_fft
