#pragma once

#include <pebble.h>
#include <stdint.h>

#define NUM_REGS 16
#define STACK_SIZE 16

typedef struct {
  
	/* 16 8-bit general purpose registers. regV[0xF] is a flag */
	uint8_t v[NUM_REGS];

	/* Special 16-bit register I, holds memory addresses */
	uint16_t i;

	/* 16-bit program counter */
	uint16_t pc;

	/* Array of 16 16-bit values which store return addresses. (16 levels of subroutines) */
	uint16_t stack[STACK_SIZE];

	/* 8-bit Stack Pointer, points to the top most level of the stack */
	uint8_t sp;

	/* Delay Timer. Subtract 1 from the register at 60Hz when non-zero */
	uint8_t dt;

	/* Sound Timer. Same as DT, but buzzes when non-zero */
	uint8_t st;

} Registers;

// Create the registers
Registers* reg_create();

// Destroy the registers
void reg_destroy(Registers*);
