#include<stdio.h>
#include<string.h>

int main(int argc, char** argv){
  size_t comprsize, insize;

  if (argc == 0){
    fprintf(stderr, "usage: lz4test file\n");
    return 1;
  }
  char* fileName = argv[0];
  FILE* in;
  if (!(in=fopen(fileName, "rb"))) {
    fprintf(stderr, "fail to open file %s", fileName);
    return 1;
  }

  fseeko(in, 0L, SEEK_END);
  insize = ftello(in);
  rewind(in);

  printf("size %lu\n", insize);

  fclose(in);
  return 0;
}
