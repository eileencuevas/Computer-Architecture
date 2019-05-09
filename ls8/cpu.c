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
  case ALU_ADD:
    // Add the value in two registers and store the result in registerA
    cpu->registers[regA] = cpu->registers[regA] + cpu->registers[regB];
    break;
  case ALU_AND:
    cpu->registers[regA] = cpu->registers[regA] & cpu->registers[regB];
    break;
  case ALU_CMP:
    // fl bits: 00000LGE
    // Compare the values in two registers.
    // If they are equal, set the Equal E flag to 1, otherwise set it to 0.
    if (cpu->registers[regA] == cpu->registers[regB])
    {
      cpu->fl = 0b00000001;
    }
    // If registerA is less than registerB, set the Less-than L flag to 1,
    //  otherwise set it to 0.
    else if (cpu->registers[regA] < cpu->registers[regB])
    {
      cpu->fl = 0b00000100;
    }
    // If registerA is greater than registerB, set the Greater-than G flag to 1,
    // otherwise set it to 0.
    else
    {
      cpu->fl = 0b00000010;
    }
    break;
  case ALU_DEC:
    // Decrement (subtract 1 from) the value in the given register.
    cpu->registers[regA]--;
    break;
  case ALU_DIV:
    // If the value in the second register is 0, the system should
    // print an error message and halt
    if (cpu->registers[regB] == 0)
    {
      printf("Error: Cannot divide by zero");
      exit(2);
    }
    // Divide the value in the first register by the value in the second,
    // storing the result in registerA
    cpu->registers[regA] = cpu->registers[regA] / cpu->registers[regB];
    break;
  case ALU_INC:
    // Increment (add 1 to) the value in the given register.
    cpu->registers[regA]++;
    break;
  case ALU_MOD:
    // If the value in the second register is 0, the system should
    // print an error message and halt
    if (cpu->registers[regB] == 0)
    {
      printf("Error: Cannot divide by zero");
      exit(2);
    }
    // Divide the value in the first register by the value in the second,
    // storing the remainder of the result in registerA
    cpu->registers[regA] = cpu->registers[regA] % cpu->registers[regB];
    break;
  case ALU_NOT:
    // Perform a bitwise-NOT on the value in a register.
    cpu->registers[regA] = ~cpu->registers[regA];
    break;
  case ALU_OR:
    // Perform a bitwise-OR between the values in registerA and registerB,
    // storing the result in registerA.
    cpu->registers[regA] = cpu->registers[regA] | cpu->registers[regB];
    break;
  case ALU_SHL:
    // Shift the value in registerA left by the number of bits specified in registerB,
    // filling the low bits with 0.
    cpu->registers[regA] = cpu->registers[regA] << cpu->registers[regB];
    break;
  case ALU_SHR:
    // Shift the value in registerA right by the number of bits specified in registerB,
    // filling the high bits with 0.
    cpu->registers[regA] = cpu->registers[regA] >> cpu->registers[regB];
    break;
  case ALU_SUB:
    // Subtract the value in the second register from the first,
    // storing the result in registerA.
    cpu->registers[regA] = cpu->registers[regA] - cpu->registers[regB];
    break;
  case ALU_XOR:
    // Perform a bitwise-XOR between the values in registerA and registerB,
    // storing the result in registerA.
    cpu->registers[regA] = cpu->registers[regA] ^ cpu->registers[regB];
    break;
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
    case ADD:
      alu(cpu, ALU_ADD, operandA, operandB);
      break;
    case AND:
      alu(cpu, ALU_AND, operandA, operandB);
      break;
    case CMP:
      alu(cpu, ALU_CMP, operandA, operandB);
      break;
    case CALL:
      // The address of the instruction directly after CALL is pushed onto the stack
      cpu->registers[7]--;
      cpu_ram_write(cpu, cpu->registers[7], cpu->pc + num_of_operations);
      // The PC is set to the address stored in the given register
      // Also, decrement amount set to PC by num_of_operations as it'll be increased
      // after exiting case
      cpu->pc = cpu->registers[operandA] - num_of_operations;
      // We jump to that location in RAM and execute the first instruction in the subroutine
      break;
    case DEC:
      alu(cpu, ALU_DEC, operandA, operandB);
      break;
    case DIV:
      alu(cpu, ALU_DIV, operandA, operandB);
      break;
    case INC:
      alu(cpu, ALU_INC, operandA, operandB);
      break;
    case LDI: // 2 operands
      // set the value of a register to an integer
      cpu->registers[operandA] = operandB;
      break;
    case MOD:
      alu(cpu, ALU_MOD, operandA, operandB);
      break;
    case NOT:
      alu(cpu, ALU_NOT, operandA, operandB);
      break;
    case OR:
      alu(cpu, ALU_OR, operandA, operandB);
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
    case RET:
      // Pop the value from the top of the stack and store it in the PC
      // Also, decrement amount returned by num_of_operations as it'll be increased
      // after exiting case
      cpu->pc = cpu_ram_read(cpu, cpu->registers[7]) - num_of_operations;
      cpu->registers[7]++;
      break;
    case SHL:
      alu(cpu, ALU_SHL, operandA, operandB);
      break;
    case SHR:
      alu(cpu, ALU_SHR, operandA, operandB);
      break;
    case SUB:
      alu(cpu, ALU_SUB, operandA, operandB);
      break;
    case XOR:
      alu(cpu, ALU_XOR, operandA, operandB);
      break;
    default: // instruction not found
      printf("Unknown instruction. PC = %d || IR = %d\n", cpu->pc, ir);
      exit(1);
    }
    // 6. Move the PC to the next instruction.
    // printf("PC: %d\n", cpu->pc);
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