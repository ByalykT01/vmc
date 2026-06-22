#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define VMC_STACK_CAPACITY 1024

typedef enum {
  TRAP_OK,
  TRAP_STACK_OVERFLOW,
  TRAP_STACK_UNDERFLOW,
  TRAP_ILLEGAL_INST,
} Trap;

typedef int64_t Word;

typedef struct {
  Word stack[VMC_STACK_CAPACITY];
  size_t stack_size;
} Vmc;

typedef enum { INST_PUSH, INST_PLUS } Inst_Type;

typedef struct {
  Inst_Type type;
  Word operand;
} Inst;

Inst inst_push(Word operand) {
  return (Inst){
      .type = INST_PUSH,
      .operand = operand,
  };
}

Inst inst_plus() { return (Inst){.type = INST_PLUS}; }

Trap vmc_execute_inst(Vmc *vmc, Inst inst) {
  switch (inst.type) {
  case INST_PUSH:
    if (vmc->stack_size >= VMC_STACK_CAPACITY) {
      return TRAP_STACK_OVERFLOW;
    }
    vmc->stack[vmc->stack_size++] = inst.operand;
    break;
  case INST_PLUS:
    if (vmc->stack_size < 2) {
      return TRAP_STACK_UNDERFLOW;
    }
    vmc->stack[vmc->stack_size - 2] += vmc->stack[vmc->stack_size - 1];
    vmc->stack_size -= 1;
    break;
  default:
    return TRAP_ILLEGAL_INST;
  }
  return TRAP_OK;
}

void vmc_dump(const Vmc *vmc) {
  printf("Stack:\n");
  if (vmc->stack_size == 0) {
    printf("  empty\n");
    return;
  }

  for (size_t i = 0; i < vmc->stack_size; i++) {
    printf(" %ld\n", vmc->stack[i]);
  }
}

Vmc vmc = {0};

int main(int argc, char *argv[]) {
  vmc_dump(&vmc);
  vmc_execute_inst(&vmc, inst_push(1));

  vmc_dump(&vmc);
  vmc_execute_inst(&vmc, inst_push(2));

  vmc_dump(&vmc);
  vmc_execute_inst(&vmc, inst_plus());

  vmc_dump(&vmc);

  return 0;
}
