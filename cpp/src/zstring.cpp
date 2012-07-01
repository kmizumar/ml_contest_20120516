#include <zlib.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>

void usage(const char *prog) {
  printf("%s df\n", prog);
  exit(-1);
}

int main(int argc, char *argv[]) {
  FILE *fp; char *buf;
  if (argc != 2 || (fp = fopen(argv[1], "r")) == NULL) usage(argv[0]);
  fseek(fp, 0, SEEK_END);
  fpos_t pos;
  fgetpos(fp, &pos);
#ifdef __APPLE__
  int fsiz = pos;
#elif __linux
  int fsiz = pos.__pos;
#endif
  if ((buf = (char *) malloc(fsiz)) == NULL) {
    printf("malloc failed for size (%d)\n", (int) fsiz);
    fclose(fp);
    return -1;
  }
  fseek(fp, 0, SEEK_SET);
  if (fread(buf, sizeof(char), fsiz, fp) != fsiz) {
    printf("read error (%s)\n", argv[1]);
    fclose(fp);
    return -1;
  }
  fclose(fp);
  uLong destLen = compressBound(fsiz);
  Bytef *dest = (Bytef *) malloc(destLen);
  assert(dest != NULL);
  int ret = compress2(dest, &destLen, (Bytef *) buf, fsiz, Z_BEST_COMPRESSION);
  assert(ret == Z_OK);
  for (int i=0; i<destLen; i++) {
    if (i % 16 == 0) printf("\"");
    printf("%02x", dest[i]);
    if (i % 16 == 15) printf("\",\n");
  }
  printf("\", 0 // sentinel\n");
  free(buf);
  free(dest);
  return 0;
}
