#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"

#define DATA_LEN 6

unsigned char cpu_ram_read(struct cpu *cpu, unsigned char index)
{
  return cpu->ram[index];
}

void cpu_ram_write(struct cpu *cpu, unsigned char index, unsigned char thing_to_write)
{
  cpu->ram[index] = thing_to_write;
}

/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */
void cpu_load(struct cpu *cpu, char *filedir)
{
  /*
  char data[DATA_LEN] = {
      // From print8.ls8
      0b10000010, // LDI R0,8
      0b00000000,
      0b00001000,
      0b01000111, // PRN R0
      0b00000000,
      0b00000001 // HLT
  };

  int address = 0;

  for (int i = 0; i < DATA_LEN; i++)
  {
    cpu->ram[address++] = data[i];
  }
  */

  // TODO: Replace this with something less hard-coded
  FILE *file = fopen(filedir, "r");

  if (file == NULL)
  {
    printf("File not found!\n\n");
    exit(2);
  }

  char line[1024];
  int address = 0;

  while (fgets(line, 1024, file) != NULL)
  {
    char *endpoint;
    unsigned char val = strtoul(line, &endpoint, 2);

    if (endpoint == line)
    {
      continue;
    }

    cpu->ram[address++] = val;
  }

  fclose(file);
}

/**
 * ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{
  switch (op)
  {
  case ALU_MUL:
    // TODO
    break;

    // TODO: implement more ALU ops
  }
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  int running = 1; // True until we get a HLT instruction
  unsigned char operandA;
  unsigned char operandB;

  while (running)
  {
    unsigned int num_of_operations;
    // TODO
    // 1. Get the value of the current instruction (in address PC).
    unsigned char ir = cpu_ram_read(cpu, cpu->pc);
    // 2. Figure out how many operands this next instruction requires
    num_of_operations = ir >> 6; // number of operations are stored in bits #6-7
    // 3. Get the appropriate value(s) of the operands following this instruction
    operandA = cpu_ram_read(cpu, cpu->pc + 1);
    operandB = cpu_ram_read(cpu, cpu->pc + 2);
    // 4. switch() over it to decide on a course of action.
    switch (ir)
    {
    // 5. Do whatever the instruction should do according to the spec.
    // 6. Move the PC to the next instruction.
    case LDI: // 2 operands
      // set the value of a register to an integer
      cpu->registers[operandA] = operandB;
      cpu->pc += 1 + num_of_operations;
      break;
    case PRN: // PRN, 1 operands
      // Print to the console the decimal integer value stored in the given register
      printf("%d\n", *cpu->registers[operandA]);
      cpu->pc += 1 + num_of_operations;
      break;
    case HLT: // HLT, no operands
      running = 0;
      cpu->pc += 1 + num_of_operations;
      break;
    default: // instruction not found
      printf("Unknown instruction. PC = %d || IR = %d\n", cpu->pc, ir);
      exit(1);
    }
  }
}

/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu)
{
  // TODO: Initialize the PC and other special registers
  cpu->pc = 0;
  cpu->fl = 0;
  memset(cpu->ram, 0, 256 * sizeof(cpu->ram[0]));
  memset(cpu->registers, 0, 8 * sizeof(cpu->registers[0]));
  cpu->registers[7] = 0xF4;
}