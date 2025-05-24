#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum : uint8_t { false = 0, true = 1 } bool;

#define padding_assert(type, size)                                             \
  _Static_assert(sizeof(type) == size, "Padding error")

typedef struct {
  uint32_t length;     // 4b
  bool active;         // 1b
  uint8_t _padding[3]; // 3b
} region;

padding_assert(region, 8);

typedef struct {
  intptr_t *buffer;  // 8b
  uint32_t capacity; // 4b
  char _padding[4];
} arena;

padding_assert(arena, 16);

region *set_region(void *buffer, bool active, int length) {
  region *reg = buffer;
  reg->active = active;
  reg->length = length;
  return reg;
}

int align(int value) { return value + value % 8; }

arena create_arena(int capacity) {
  capacity = align(capacity); // 13 -> 16

  arena arena;
  arena.buffer = malloc(capacity);
  arena.capacity = capacity;

  set_region(arena.buffer, false, align(capacity - sizeof(region)));

  return arena;
}

region *arena_next(arena arena, region *reg) {
  reg = reg + sizeof(region) + reg->length;

  intptr_t end_ptr = (intptr_t)(arena.buffer + arena.capacity);
  intptr_t reg_ptr = (intptr_t)reg;

  if (reg_ptr >= end_ptr)
    return NULL;

  return reg;
}

void *arena_get(arena arena, int length) {
  length = align(length);
  region *reg = (region *)arena.buffer;

  while (reg != NULL) {
    if (!reg->active && reg->length >= length) {
      int junk = reg->length - length;

      if (junk > 0) {
        if (junk > sizeof(region) + 8) { // ensure min aligment block
          region *split = reg + sizeof(region) + length;
          set_region(split, false, junk);
        } else {
          length = reg->length;
        }
      }

      set_region(reg, true, length);
      char *data = (char *)(reg) + sizeof(region);

      return data;
    }

    reg = arena_next(arena, reg);
  }

  return NULL;
}

void arena_gc(arena arena) {
  region *reg = (region *)arena.buffer;

  do {
    region *next = arena_next(arena, reg);

    if (next == NULL)
      break;

    if (!next->active)
      reg->length += sizeof(region) + next->length;
    else
      break;
  } while (true);
}

void arena_free(arena arena, void *data) {
  region *reg = (region *)((char *)data - sizeof(region));

  reg->active = false;
}

void arena_debug_memory(arena arena, int col) {
  uint8_t *buffer = (uint8_t *)arena.buffer;

  printf("Arena Debug Memory:\n");
  for (int i = 0; i < arena.capacity; i++) {
    printf("%02X ", buffer[i]);

    if (i % col == col - 1)
      printf("\n");
  }

  printf("\n");
}

void arena_destroy(arena arena) { free(arena.buffer); }

void arena_debug_regions(arena arena) {
  region *reg = (region *)arena.buffer;

  printf("Arena Debug Regions:\n");
  while (reg != NULL) {
    printf("%d | %d\n", reg->active, reg->length);
    reg = arena_next(arena, reg);
  }
}
