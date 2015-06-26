#include <stdlib.h>
#include <unistd.h>
#include "cpu.h"
#include "logger.h"

#define CLOCK_SLEEP_MS 4
  
static AppTimer* timer;
static void timer_callback(void* data);
  
uint16_t execute_read_instruction(Memory* mem, uint16_t pointer);
void execute_cycle(CPU* proc);
void execute_instruction(CPU* proc, uint16_t instr);
void execute_ret(Registers* reg);
void execute_call(Registers* reg, int nnn);
void execute_se(Registers* reg, int x, int bite);
void execute_sne(Registers* reg, int x, int bite);
void execute_alu(Registers* reg, int x, int y, int opcode);
void execute_store_bcd(Memory* mem, int num, int location);
void execute_dispatch_ld(CPU* proc, int x, int opcode);

// Create a CHIP-8 CPU
CPU* cpu_create(Display* display) {
	CPU* proc = (CPU*) malloc(sizeof(CPU));
	proc->display = display;
	proc->registers = reg_create();
	proc->memory = mem_create();
	proc->keyboard = keyboard_create(); 
  proc->active = false;
  return proc;
}

// Destroy a CHIP-8 CPU
void cpu_destroy(CPU* proc) {
	reg_destroy(proc->registers);
	mem_destroy(proc->memory);
	keyboard_destroy(proc->keyboard);
}

// Start the CPU and load the ROM
void cpu_start(CPU* proc, Rom* rom) {
  
  APP_LOG(VERBOSE, "cpu_start()");

  ResHandle bios = resource_get_handle(RESOURCE_ID_DIGITS);
  mem_load_file(proc->memory, bios, MEM_BIOS_START);
  
  APP_LOG(VERBOSE, "cpu_start: bios loaded");

  proc->rom = rom;
  mem_load_file(proc->memory, rom->resource, MEM_PROGRAM_START);
  
  APP_LOG(VERBOSE, "cpu_start: rom loaded");
  cpu_toggle_pause(proc);
}

// Pause and unpause the CPU
void cpu_toggle_pause(CPU* proc) {
  if (proc->active) {
    proc->active = false;
  } else {
    proc->active = true;
    timer = app_timer_register(CLOCK_SLEEP_MS, timer_callback, (void*) proc);
  }
}

uint16_t execute_read_instruction(Memory* mem, uint16_t pointer) {
  uint8_t b1 = mem->ram[pointer];
  uint8_t b2 = mem->ram[pointer+1];
  return (b1 << 8) | b2;
}

static void timer_callback(void* data) {
  CPU* proc = (CPU*) data;
  if (proc->active) {
    execute_cycle(proc);
    timer = app_timer_register(CLOCK_SLEEP_MS, timer_callback, data);
  }
}

void execute_cycle(CPU* proc) {
	// Read instruction
	uint16_t instr = execute_read_instruction(proc->memory, proc->registers->pc);
	
	// Update PC 
	proc->registers->pc += 2;
	
	// Execute instruction
	execute_instruction(proc, instr);
}

void execute_ret(Registers* reg) {
  reg->sp--;
  reg->pc = reg->stack[reg->sp];
}

void execute_call(Registers* reg, int nnn) {
  reg->stack[reg->sp] = reg->pc;
  reg->sp++;
  reg->pc = nnn; 
}

void execute_instruction(CPU* proc, uint16_t instr) {
	
  Registers* reg = proc->registers;
  
  int x   = (instr & 0x0F00) >> 8;
  int y   = (instr & 0x00F0) >> 4;
  int kk  = instr & 0x00FF;
  int nnn = instr & 0x0FFF;
	
  //APP_LOG(VERBOSE, "instr = %x, x=%x, y=%x, kk=%x, nnnn=%x", instr, x, y, kk, nnn);
  
	switch(instr & 0xF000) {
      case 0x0000:
        if (instr == 0x00E0) {
          display_clear(proc->display);
          log_name("CLS");
        }
        else if (instr == 0x00EE) {
          execute_ret(reg);
          log_name("RET");
        }
        break;
      case 0x1000:
        reg->pc = (uint16_t) nnn;
        log_instrAddr("JP", nnn);
        break;
      case 0x2000:
        execute_call(reg, nnn);
        log_instrAddr("CALL", nnn);
        break;
      case 0x3000:
        execute_se(reg, x,kk);
        log_instrVRegAddr("SE", x, kk);
        break;
      case 0x4000:
        execute_sne(reg, x,kk);
        log_instrVRegAddr("SNE", x, kk);
        break;
      case 0x5000:
        execute_se(reg, x, reg->v[y]);
        log_instrVRegVReg("SE", x, y);
        break;
      case 0x6000:
        reg->v[x] = kk;
        log_instrVRegAddr("LD", x, kk);
        break;
      case 0x7000:
        reg->v[x] += kk;
        log_instrVRegAddr("ADD", x, kk);
        break;
      case 0x8000:
        execute_alu(reg, x, y, instr & 0x000F);
        break;
      case 0x9000:
        execute_sne(reg, x, reg->v[y]);
        log_instrVRegVReg("SNE", x, y);
        break;
      case 0xA000:
        reg->i = nnn;
        log_instrRegAddr("LD", "I", nnn);
        break;
      case 0xB000:
        reg->pc = (nnn + reg->v[0]);
        log_instrVRegAddr("JP", 0, nnn);
        break;
      case 0xC000:
      {
        int random = rand() % 255;
        reg->v[x] = random & kk;
        log_instrVRegAddr("RND", x, kk);
        break;
      }
      case 0xD000:
        reg->v[0xf] = display_draw(proc->display, reg->v[x], reg->v[y], reg->i, proc->memory, (instr & 0x000F));
        log_instrVRegVRegAddr("DRW", x, y, (instr & 0x000F));
        break;
      case 0xE000:
      {
        bool skp_if_pressed = (instr & 1) == 0;
        if (skp_if_pressed == keyboard_is_pressed(proc->keyboard, reg->v[x])) {
          keyboard_release(proc->keyboard, reg->v[x]);
          reg->pc += 2;
        }
        log_instrVReg((kk == 0x9E) ? "SKP" : "SKNP", x);
        break;
      }
      case 0xF000:
        execute_dispatch_ld(proc, x, kk);
        break;
    }
  }
  
/* Skip next instruction on 'equal' */
void execute_se(Registers* reg, int x, int bite) {
  if (reg->v[x] == bite)
    reg->pc += 2;
}

/* Skip next instruction on 'not equal' */
void execute_sne(Registers* reg, int x, int bite) {
  if (reg->v[x] != bite)
    reg->pc += 2;
}

/* Dispatch an instruction to the Arithmetic Logic Unit */
void execute_alu(Registers* reg, int x, int y, int opcode) {
  int result = 0;
  int vX = reg->v[x] & 0xFF;
  int vY = reg->v[y] & 0xFF;

  switch (opcode) {
    case 0:
      result = vY;
      break;
    case 1:
      result = (vX | vY);
      log_instrVRegVReg("OR", x, y);
      break;
    case 2:
      result = (vX & vY);
      log_instrVRegVReg("AND", x, y);
      break;
    case 3:
      result = (vX ^ vY);
      log_instrVRegVReg("XOR", x, y);
      break;
    case 4:
      result = (vX + vY);
      reg->v[0xF] = (result > 0xFF);
      log_instrVRegVReg("ADD", x, y);
      break;
    case 5:
      reg->v[0xF] = (vX > vY);
      result = (vX - vY);
      log_instrVRegVReg("SUB", x, y);
      break;
    case 6:
      reg->v[0xF] = ((vX & 1) == 1);
      result = (vX >> 1);
      log_instrVRegVReg("SHR", x, y);
      break;
    case 7:
      reg->v[0xF] = (vY > vX);
      result = (vY - vX);
      log_instrVRegVReg("SUBN", x, y);
      break;
    case 0xE:
      reg->v[0xF] = ((vX & 0x80) != 0);
      result = (vX << 1);
      log_instrVRegVReg("SHL", x, y);
      break;
  }
  reg->v[x] = result;
}

void execute_store_bcd(Memory* mem, int num, int location) {
  num &= 0xFF;
  int hun = (num / 100);
  int ten = ((num % 100) / 10);
  int one = ((num % 100) % 10);

  mem->ram[location] = (uint8_t) hun;
  mem->ram[location+1] = (uint8_t) ten;
  mem->ram[location+2] = (uint8_t) one;
}

/* Dispatch a general Load instruction */
void execute_dispatch_ld(CPU* proc, int x, int opcode) {
  Registers* reg = proc->registers;
  switch (opcode) {
    case 0x07:
      reg->v[x] = reg->dt;
      log_instrVRegReg("LD", x, "DT");
      break;
    case 0x0A:
    {
      key8_t k = keyboard_get_first_pressed(proc->keyboard);
      // If no button was pressed, repeat instruction
      if (k == KEY_INVALID) {
        reg->pc -= 2;
        break;
      }
      reg->v[x] = k;
      keyboard_release(proc->keyboard, k);
      log_instrVRegReg("LD", x, "K");
      break;
    }
    case 0x15:
      reg->dt = reg->v[x];
      log_instrRegVReg("LD", "DT", x);
      break;
    case 0x18:
      reg->st = reg->v[x];
      log_instrRegVReg("LD", "ST", x);
      break;
    case 0x1E:
      reg->i += (0xFF & reg->v[x]);
      log_instrRegVReg("ADD", "I", x);
      break;
    case 0x29:
      reg->i = (reg->v[x] & 0x0F) * SPRITE_LENGTH;
      log_instrRegVReg("LD", "F", x);
      break;
    case 0x33:
      execute_store_bcd(proc->memory, reg->v[x], reg->i);
      log_instrRegVReg("LD", "B", x);
      break;
    case 0x55:
      mem_store(proc->memory, x, reg);
      log_instrRegVReg("LD", "[I]", x);
      break;
    case 0x65:
      mem_load(proc->memory, x, reg);
      log_instrVRegReg("LD", x, "[I]");
      break;
  }
}
	