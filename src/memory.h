#pragma once

#include <pebble.h>
#include "registers.h"

#define RAM_SIZE 4096
#define MEM_BIOS_START 0x0
#define MEM_PROGRAM_START 0x200

typedef struct {
	uint8_t ram[RAM_SIZE];
} Memory;

// Create the memory
Memory* mem_create();

// Destroy the memory
void mem_destroy(Memory*);

// Store [v[0], v[x]] at I in memory
void mem_store(Memory* mem, int x, Registers* regs);

// Load memory at I into [v[0], v[x]]
void mem_load(Memory* mem, int x, Registers* regs);

// Get byte at index in memory
uint8_t mem_at(Memory* mem, int index);

// Load bytes from a file into memory at the given location
void mem_load_file(Memory* mem, ResHandle resource, int location);

// Dump the contents of memory to the log
void mem_dump(Memory* mem);
