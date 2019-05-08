#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"

#define DATA_LEN 6

// -------- HELPER FUNCTIONS -------- //
unsigned char cpu_ram_read(struct cpu *cpu, unsigned char index)
{
  return cpu->ram[index];
}

void cpu_ram_write(struct cpu *cpu, unsigned char index, unsigned char thing_to_write)
{
  cpu->ram[index] = thing_to_write;
}
// --------------------------------- //

/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */
void cpu_load(struct cpu *cpu, char *file)
{

  // TODO: Replace this with something less hard-coded
  FILE *fp = fopen(file, "r");

  if (fp == NULL)
  {
    printf("File not found!\n\n");
    exit(2);
  }

  char line[1024];
  int address = 0;

  while (fgets(line, 1024, fp) != NULL)
  {
    char *endpoint;
    unsigned char val = strtoul(line, &endpoint, 2);

    if (endpoint == line)
    {
      continue;
    }

    cpu->ram[address++] = val;
  }

  fclose(fp);
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
    // Multiply the values in two registers together and store the result in registerA.
    cpu->registers[regA] = cpu->registers[regA] * cpu->registers[regB];
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
    unsigned char num_of_operations;
    // TODO
    // 1. Get the value of the current instruction (in address PC).
    unsigned char ir = cpu_ram_read(cpu, cpu->pc);
    // 2. Figure out how many operands this next instruction requires
    num_of_operations = ((ir >> 6) & 0b11) + 1; // number of operations are stored in bits #6-7
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
      break;
    case POP:
      // Copy the value from the address pointed to by SP to the given register
      cpu->registers[operandA] = cpu_ram_read(cpu, cpu->registers[7]);
      // Increment SP
      cpu->registers[7]++;
      break;
      //TODO: implement something for potential stack underflow
    case PRN: // PRN, 1 operands
      // Print to the console the decimal integer value stored in the given register
      printf("%d\n", cpu->registers[operandA]);
      break;
    case PUSH: //PUSH, 1 operand
      // Decrement the SP
      cpu->registers[7]--;
      // Copy the value in the given register to the address pointed to by SP
      cpu_ram_write(cpu, cpu->registers[7], cpu->registers[operandA]);
      // TODO: implement something for potential stack overflow
      break;
    case HLT: // HLT, no operands
      running = 0;
      break;
    case MUL:
      alu(cpu, ALU_MUL, operandA, operandB);
      break;
    default: // instruction not found
      printf("Unknown instruction. PC = %d || IR = %d\n", cpu->pc, ir);
      exit(1);
    }
    cpu->pc += num_of_operations;
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