#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define VMC_STACK_CAPACITY 1024
#define ARRAY_SIZE(xs) (sizeof(xs) / sizeof(xs[0]))

typedef enum {
  ERR_OK,
  ERR_STACK_OVERFLOW,
  ERR_STACK_UNDERFLOW,
  ERR_ILLEGAL_INST,
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
  default:
    assert(00 && "Unreachable");
  }
}

typedef int64_t Word;

typedef struct {
  Word stack[VMC_STACK_CAPACITY];
  size_t stack_size;
} Vmc;

typedef enum {
  INST_PUSH,
  INST_PLUS,
  INST_MINUS,
  INST_MULT,
  INST_DIV
} Inst_Type;

const char *inst_type_as_cstr(Inst_Type type) {
  switch (type) {
  case INST_PLUS:
    return "INST_PLUS";
  case INST_MINUS:
    return "INST_MINUS";
  case INST_MULT:
    return "INST_MULT";
  case INST_DIV:
    return "INST_DIV";
  default:
    assert(00 && "Unreachable");
  }
}

typedef struct {
  Inst_Type type;
  Word operand;
} Inst;

#define MAKE_INST_PUSH(value) {.type = INST_PUSH, .operand = (value)}

#define MAKE_INST_PLUS {.type = INST_PLUS}
#define MAKE_INST_MINUS {.type = INST_MINUS}
#define MAKE_INST_MULT {.type = INST_MULT}
#define MAKE_INST_DIV {.type = INST_DIV}

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
    vmc->stack[vmc->stack_size - 2] /= vmc->stack[vmc->stack_size - 1];
    vmc->stack_size -= 1;
    break;
  default:
    return ERR_ILLEGAL_INST;
  }
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

Inst program[] = {
    MAKE_INST_PUSH(1), MAKE_INST_PUSH(2), MAKE_INST_PLUS,
    MAKE_INST_PUSH(2), MAKE_INST_MINUS,
};

int main() {
  for (size_t i = 0; i < ARRAY_SIZE(program); ++i) {
    printf("STEP: %zu\n", i + 1);
    Err err = vmc_execute_inst(&vmc, program[i]);
    if (err != ERR_OK) {
      fprintf(stderr, "Err activated: %s\n", err_as_cstr(err));
      vmc_dump(stdout, &vmc);
      return 1;
    }
    vmc_dump(stdout, &vmc);
  }
  vmc_dump(stdout, &vmc);

  return 0;
}
