#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<time.h>
#include"lz4.h"

#define PAD_SIZE (16*1024)
#define MIN_PAGE_SIZE 4096  // smallest page size we expect, if it's wrong the first algorithm might be a bit slower
#define DEFAULT_LOOP_TIME (100*1000000)  // 1/10 of a second
#define GET_COMPRESS_BOUND(insize) (insize + insize/6 + PAD_SIZE)  // for pithy

typedef struct timespec bench_rate_t;
typedef struct timespec bench_timer_t;
#define GetTime(now) if (clock_gettime(CLOCK_MONOTONIC, &now) == -1 ){ printf("clock_gettime error"); };
#define GetDiffTime(start_ticks, end_ticks) (1000000000ULL*( end_ticks.tv_sec - start_ticks.tv_sec ) + ( end_ticks.tv_nsec - start_ticks.tv_nsec ))

/*
 * Allocate a buffer of size bytes using malloc (or equivalent call returning a buffer
 * that can be passed to free). Touches each page so that the each page is actually
 * physically allocated and mapped into the process.
 */
void* alloc_and_touch(size_t size, int must_zero) {
  void *buf = must_zero ? calloc(1, size) : malloc(size);
  volatile char zero = 0;
  for (size_t i = 0; i < size; i += MIN_PAGE_SIZE) {
    ((char * volatile) buf)[i] = zero;
  }
  return buf;
}

int main(int argc, char** argv){
  size_t comprsize, insize;
  char *inbuf, *compbuf, *decomp;
  bench_timer_t start_ticks, end_ticks;

  if (argc != 2){
    fprintf(stderr, "usage: lz4test file\n");
    return 1;
  }
  char* fileName = argv[1];
  FILE* in;
  if (!(in=fopen(fileName, "rb"))) {
    fprintf(stderr, "fail to open file %s", fileName);
    return 1;
  }

  fseeko(in, 0L, SEEK_END);
  insize = ftello(in);
  rewind(in);
  //printf("loading %s size %lu bytes ... ", fileName, insize);

  comprsize = GET_COMPRESS_BOUND(insize);

  inbuf = (char*)alloc_and_touch(insize + PAD_SIZE, 0);
  compbuf = (char*)alloc_and_touch(comprsize, 0);
  decomp = (char*)alloc_and_touch(insize + PAD_SIZE, 1);

  if (!inbuf || !compbuf || !decomp){
    printf("not enough memory\n");
    return 1;
  }

  insize = fread(inbuf, 1, insize, in);
  //printf("done\n");

  printf("compressing %s ... ", fileName);
  GetTime(start_ticks);
  int complen = LZ4_compress_default(inbuf, compbuf, insize, comprsize);
  GetTime(end_ticks);
  unsigned long long comp_nanosec = GetDiffTime(start_ticks, end_ticks);
  float comp_speed = (float)insize*1000/comp_nanosec;

  if (!complen){
    printf("error\n");
    return 1;
  } else {
    float ratio = (float)insize/complen;
    printf("done %lu to %d bytes ratio %.3f in %llu ns speed %.2f MB/s\n", insize, complen, ratio, comp_nanosec, comp_speed);
  }

  fclose(in);
  return 0;
}
