#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define VMC_STACK_CAPACITY 1024
#define VMC_PROGRAM_CAPACITY 1024
#define ARRAY_SIZE(xs) (sizeof(xs) / sizeof(xs[0]))

typedef enum {
  ERR_OK,
  ERR_STACK_OVERFLOW,
  ERR_STACK_UNDERFLOW,
  ERR_ILLEGAL_INST,
  ERR_DIV_BY_ZERO
} Err;

const char *err_as_cstr(Err err) {
  switch (err) {
  case ERR_STACK_OVERFLOW:
    return "ERR_STACK_OVERFLOW";
  case ERR_OK:
    return "ERR_OK";
  case ERR_STACK_UNDERFLOW:
    return "ERR_STACK_UNDERFLOW";
  case ERR_ILLEGAL_INST:
    return "ERR_ILLEGAL_INST";
  case ERR_DIV_BY_ZERO:
    return "ERR_DIV_BY_ZERO";
  default:
    assert(00 && "Unreachable");
  }
}

typedef int64_t Word;

typedef enum {
  INST_PUSH,
  INST_PLUS,
  INST_MINUS,
  INST_MULT,
  INST_DIV,
  INST_JMP,
  INST_HALT,
} Inst_Type;

const char *inst_type_as_cstr(Inst_Type type) {
  switch (type) {
  case INST_PUSH:
    return "INST_PUSH";
  case INST_PLUS:
    return "INST_PLUS";
  case INST_MINUS:
    return "INST_MINUS";
  case INST_MULT:
    return "INST_MULT";
  case INST_DIV:
    return "INST_DIV";
  case INST_JMP:
    return "INST_JMP";
  case INST_HALT:
    return "INST_HALT";
  default:
    assert(00 && "Unreachable");
  }
}

typedef struct {
  Inst_Type type;
  Word operand;
} Inst;

typedef struct {
  Word stack[VMC_STACK_CAPACITY];
  size_t stack_size;

  Inst program[VMC_PROGRAM_CAPACITY];
  Word ip;
  size_t program_size;

  int halt;
} Vmc;

#define MAKE_INST_PUSH(value) {.type = INST_PUSH, .operand = (value)}
#define MAKE_INST_PLUS {.type = INST_PLUS}
#define MAKE_INST_MINUS {.type = INST_MINUS}
#define MAKE_INST_MULT {.type = INST_MULT}
#define MAKE_INST_DIV {.type = INST_DIV}
#define MAKE_INST_JMP(addr) {.type = INST_JMP, .operand = (addr)}
#define MAKE_INST_HALT {.type = INST_DIV}

Err vmc_execute_inst(Vmc *vmc, Inst inst) {
  switch (inst.type) {
  case INST_PUSH:
    if (vmc->stack_size >= VMC_STACK_CAPACITY) {
      return ERR_STACK_OVERFLOW;
    }
    vmc->stack[vmc->stack_size++] = inst.operand;
    break;
  case INST_PLUS:
    if (vmc->stack_size < 2) {
      return ERR_STACK_UNDERFLOW;
    }
    vmc->stack[vmc->stack_size - 2] += vmc->stack[vmc->stack_size - 1];
    vmc->stack_size -= 1;
    break;
  case INST_MINUS:
    if (vmc->stack_size < 2) {
      return ERR_STACK_UNDERFLOW;
    }
    vmc->stack[vmc->stack_size - 2] -= vmc->stack[vmc->stack_size - 1];
    vmc->stack_size -= 1;
    break;

  case INST_MULT:
    if (vmc->stack_size < 2) {
      return ERR_STACK_UNDERFLOW;
    }
    vmc->stack[vmc->stack_size - 2] *= vmc->stack[vmc->stack_size - 1];
    vmc->stack_size -= 1;
    break;

  case INST_DIV:
    if (vmc->stack_size < 2) {
      return ERR_STACK_UNDERFLOW;
    }
    if (vmc->stack[vmc->stack_size - 1] == 0) {
      return ERR_DIV_BY_ZERO;
    }
    vmc->stack[vmc->stack_size - 2] /= vmc->stack[vmc->stack_size - 1];
    vmc->stack_size -= 1;
    break;

  case INST_JMP:

    break;
  default:
    return ERR_ILLEGAL_INST;
  }
  vmc->ip += 1;
  return ERR_OK;
}

void vmc_dump(FILE *stream, const Vmc *vmc) {
  fprintf(stream, "Stack:\n");
  if (vmc->stack_size == 0) {
    fprintf(stream, "  empty\n");
    return;
  }

  for (size_t i = 0; i < vmc->stack_size; i++) {
    fprintf(stream, " %ld\n", vmc->stack[i]);
  }
}

Vmc vmc = {0};

Inst program[] = {MAKE_INST_PUSH(4), MAKE_INST_PUSH(2), MAKE_INST_PLUS,
                  MAKE_INST_PUSH(2), MAKE_INST_MINUS,   MAKE_INST_PUSH(3),
                  MAKE_INST_DIV,     MAKE_INST_HALT};

void vmc_push_inst(Vmc *vmc, Inst inst) {
  assert(vmc->program_size < VMC_PROGRAM_CAPACITY);
  vmc->program[vmc->program_size++] = inst;
}

int main() {
  while (!vmc.halt) {
    printf("STEP: %zu, %s\n", vmc.ip + 1,
           inst_type_as_cstr(program[vmc.ip].type));
    Err err = vmc_execute_inst(&vmc, program[vmc.ip]);
    if (err != ERR_OK) {
      fprintf(stderr, "ERR ACTIVATED: %s\n", err_as_cstr(err));
      exit(1);
    }
    vmc_dump(stdout, &vmc);
  }
  vmc_dump(stdout, &vmc);

  return 0;
}
