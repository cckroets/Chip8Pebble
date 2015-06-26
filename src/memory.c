#include <string.h>
#include "memory.h"

// Create the memory
Memory* mem_create() {
	Memory* mem = (Memory*) malloc(sizeof(Memory));
	return mem;
}

// Destroy the memory
void mem_destroy(Memory* mem) {
	free(mem);
}

// Store [v[0], v[x]] at I in memory
void mem_store(Memory* mem, int x, Registers* regs) {
	memcpy((void*) &mem->ram[regs->i], (void*) regs->v, sizeof(char) * x);
}

// Load memory at I into [v[0], v[x]]
void mem_load(Memory* mem, int x, Registers* regs) {
	memcpy((void*) regs->v, (void*) &(mem->ram[regs->i]), sizeof(uint8_t) * x);
}

// Get byte at index in memory
inline uint8_t mem_at(Memory* mem, int index) {
	return mem->ram[index];
}

// Load bytes from a file into memory at the given location
void mem_load_file(Memory* mem, ResHandle resource, int location) {
	resource_load(resource, &(mem->ram[location]), RAM_SIZE);	
}

// Dump the contents of memory to the log
void mem_dump(Memory* mem) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "mem_dump()");
  APP_LOG(APP_LOG_LEVEL_DEBUG, "mem_dump: sizeof(uint8_t) = %d", sizeof(uint8_t));
  for (int i = 0; i < RAM_SIZE; i++) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "%x", mem->ram[i]);
  }
}
