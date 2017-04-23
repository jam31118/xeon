#ifndef __PRINT_FUNC_H__
#define __PRINT_FUNC_H__

void print_reg(unsigned int* PC, unsigned int *registers);
void print_mem(unsigned char *mem, unsigned int addr_begin, unsigned int addr_end);

#endif
