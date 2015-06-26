#include "registers.h"
#include "memory.h"

#define TIME_INTERVAL_MS (1000 / 130)

static AppTimer* timer;
static void timer_callback(void*);

// Create the registers
Registers* reg_create() {
	Registers* regs = (Registers*) malloc(sizeof(Registers));
	timer = app_timer_register(TIME_INTERVAL_MS, timer_callback, regs);
	regs->pc = MEM_PROGRAM_START;
	regs->sp = 0;
	regs->dt = 0;
	regs->st = 0;
	regs->i = 0;
	
	return regs;
}

// Destroy the registers
void reg_destroy(Registers* regs) {
	free(regs);
}

static void timer_callback(void* data) {
	Registers* regs = (Registers*) data;
	
  APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "timer_callback(): dt=%x, st=%x", regs->dt, regs->st);
  
	if (regs->dt > 0) {
		regs->dt--;
	}
	if (regs->st > 0) {
		vibes_short_pulse();
		regs->st--;
	} else {
		vibes_cancel();
	}
	timer = app_timer_register(TIME_INTERVAL_MS, timer_callback, data);
}
