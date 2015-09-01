#define _LARGEFILE64_SOURCE
#define _POSIX_C_SOURCE 200112L

#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <complex.h>
#include <math.h>
#include <fftw3.h>

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

int
main(int argc, char *argv[])
{
	uint16_t *addr_in;
	float *addr_out;
	int fd_in;
	int fd_out;
	struct stat sb;
	size_t length;
	uint64_t fftlen;
	size_t num_ffts;
	size_t index;

	if (argc < 4|| argc > 5) {
		fprintf(stderr, "%s filein fileout fftlength [nthreads]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	fd_in = open(argv[1], O_RDONLY|O_LARGEFILE);
	if (fd_in == -1)
		handle_error("in open");
	fd_out = open(argv[2], O_LARGEFILE|O_CREAT|O_TRUNC|O_RDWR, 0666);
	if (fd_out == -1)
		handle_error("out open");

	fftlen = atoll(argv[3]);
	int nthreads = (argc == 5) ? 4 : atoi(argv[4]);

	posix_fallocate(fd_out, 0, fftlen* sizeof(float));

	if (fstat(fd_in, &sb) == -1)           /* To obtain file size */
		handle_error("fstat");

	length = sb.st_size; //two bytes per short, two shorts per complex value
	num_ffts = length/fftlen;

	//map the input into memory.
	addr_in = mmap(NULL, length, PROT_READ,
			MAP_PRIVATE, fd_in, 0);
	if (addr_in == MAP_FAILED)
		handle_error("input mmap");

	//map the output into memory.
	addr_out = mmap(NULL, fftlen*sizeof(float), PROT_WRITE,
			MAP_SHARED, fd_out, 0);
	if (addr_out == MAP_FAILED)
		handle_error("output mmap");


	//fftwf stuff
	fftwf_init_threads();
	fftwf_plan_with_nthreads(nthreads);
	fftwf_complex *in, *out;
	fftwf_plan my_plan;
	in = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex)*fftlen);
	out = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex)*fftlen);
	my_plan = fftwf_plan_dft_1d(fftlen, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

	float *fin = (float*)in;
	float *fout = (float*)out;

	for(index = 0; index < fftlen*2; index++) {
		float tmp =  (float) addr_in[index];
		fin[index] =tmp;
	}
	addr_in += 2 * fftlen;

	//run that FFT
	fftwf_execute(my_plan);

	//calculate Power 
	size_t currindex=0;
	float real, imag;
	for(index = 0; index < fftlen; index++)	{
		real = fout[currindex++];
		imag = fout[currindex++];
		addr_out[index] = 10.0f*log10f( real * real + imag * imag );
	}

	fftwf_destroy_plan(my_plan);
	fftwf_free(in);
	fftwf_free(out);
	msync(addr_out, fftlen * sizeof(float), MS_SYNC);

	exit(EXIT_SUCCESS);
}
