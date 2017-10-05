#define _LARGEFILE64_SOURCE
#define _POSIX_C_SOURCE 200112L
#define _DEFAULT_SOURCE

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
#include <volk/volk.h>

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

int
main(int argc, char *argv[])
{
  void *addr_in;
	float *addr_out;
	int fd_in;
	int fd_out;
	struct stat sb;
	size_t length;
	int fftlen;
	size_t num_ffts;
	size_t index;

	if (argc < 4|| argc > 5) {
		fprintf(stderr, "%s filein fileout fftlength [nthreads]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	fd_in = open(argv[1], O_RDONLY|O_LARGEFILE);
	if (fd_in == -1)
		handle_error("in open");
  if(posix_fadvise(fd_in, 0, 0, POSIX_FADV_SEQUENTIAL))
    handle_error("in sequential");
  fd_out = open(argv[2], O_LARGEFILE|O_CREAT|O_TRUNC|O_RDWR, 0666);
	if (fd_out == -1)
		handle_error("out open");
  if(posix_fadvise(fd_in, 0, 0, POSIX_FADV_SEQUENTIAL))
    handle_error("out sequential");

	fftlen = atoi(argv[3]);
	int nthreads = (argc <= 4) ? 4 : atoi(argv[4]);


  if (fstat(fd_in, &sb) == -1)           /* To obtain file size */
    handle_error("fstat");

  length = sb.st_size; //two bytes per short, two shorts per complex value
  num_ffts = length/sizeof(fftwf_complex)/fftlen;

  //map the input into memory.
  addr_in = mmap(NULL, length, PROT_READ,
                 MAP_PRIVATE, fd_in, 0);
  if (addr_in == MAP_FAILED)
    handle_error("input mmap");

  //map the output into memory.
  addr_out = mmap(NULL, sizeof(float)*fftlen*num_ffts, PROT_WRITE,
                  MAP_SHARED, fd_out, 0);
  if (addr_out == MAP_FAILED)
		handle_error("output mmap");
	int fallocate_return = posix_fallocate(fd_out, 0, sizeof(float)*num_ffts*fftlen);

  printf("address mapping %p, length %d, num_ffts %d, posix_fallocate said %d \n", addr_in, fftlen, num_ffts, fallocate_return);

	//fftwf stuff
	fftwf_init_threads();
	fftwf_plan_with_nthreads(nthreads);
	fftwf_complex *out;
	fftwf_plan my_plan;
	out = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex)*fftlen*num_ffts);
  int n[] = {fftlen};
  my_plan = fftwf_plan_many_dft(1,            /* rank */
                                n,     /* length */
                                num_ffts,  /* how many */
                                (fftwf_complex*)addr_in,    /* input pointer */
                                n,       /* inembed */
                                1,          /* istride */
                                fftlen,     /* idist */
                                out,        /* output pointer */
                                n,       /* onembed */
                                1,          /* ostride */
                                fftlen,     /* odist */
                                -1,          /* sign */
                                FFTW_PRESERVE_INPUT | FFTW_ESTIMATE/* flags */
                                );


	//run that FFT
	fftwf_execute(my_plan);
	fftwf_destroy_plan(my_plan);

  volk_32fc_magnitude_squared_32f(addr_out,out, num_ffts*fftlen);

	fftwf_free(out);
	if(msync(addr_out, sizeof(float)*num_ffts*fftlen, MS_SYNC))
    handle_error("msync");

	exit(EXIT_SUCCESS);
}
