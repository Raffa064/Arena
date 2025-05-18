#include "./arena.c"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
  arena arena = create_arena(255);

  int *i = (int *)arena_get(arena, 4);
  *i = 0x10;

  int *j = arena_get(arena, 4);
  *j = 64;

  char *str = arena_get(arena, 12);
  strcpy(str, "Hello world");

  printf("[ Before freed ]\n");
  printf("i = %d\n", *i);
  printf("j = %d\n", *j);
  printf("%s\n", str);

  arena_free(arena, str);
  arena_free(arena, j);
  arena_free(arena, (char *)i);

  arena_gc(arena);

  int n = 40;
  int *buff = (int *)arena_get(arena, sizeof(int) * n);
  memset(buff, 0x64, n * 4);

  buff[0] = 0xC0FFEE;

  printf("[ After freed ]\n");
  printf("i = %d\n", *i);
  printf("j = %d\n", *j);
  printf("%s\n", str);

  printf("\n");
  arena_debug_memory(arena, 16);
  printf("\n");
  arena_debug_regions(arena);
  printf("\n");
}
