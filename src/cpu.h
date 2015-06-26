#pragma once

#include "display.h"
#include "memory.h"
#include "keyboard.h"
#include "registers.h"
#include "rom.h"

typedef struct {
	Keyboard* keyboard;
	Registers* registers;
	Memory* memory;
	Display* display;
  Rom* rom;
  bool active;
} CPU;

// Create a CHIP-8 CPU
CPU* cpu_create(Display* display);

// Destroy a CHIP-8 CPU
void cpu_destroy(CPU* proc);

// Start emulating the CPU
void cpu_start(CPU* proc, Rom* rom);

// Pause and unpause the CPU
void cpu_toggle_pause(CPU* proc);

