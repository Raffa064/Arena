# Arena.c

The arena allocator is a simple memory management technique where you pre-allocate a large block of memory and use it to store dynamic data.

## Why use an arena?

When allocating multiple data structures on the heap using the standard allocation functions like malloc, you need to carefully manage individual free() calls to avoid memory leaks. With an arena, you can allocate many objects without worrying about freeing them individually — a single call to arena_destroy() will release all the memory at once.

## Example

The above example shows a simple allocation of three different type variables using a arena to free them:

```c

arena arena = create_arena(17);

int *integer = (int*) arena_get(arena, sizeof(int));
short *smallInt = (int*) arena_get(arena, sizeof(short));

char *string = (int*) arena_get(arena, sizeof(char) * 12);
strcpy(string, "Hello world");

// use data ...

arena_destroy(arena);

```
