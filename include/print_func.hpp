#ifndef __PRINT_FUNC_H__
#define __PRINT_FUNC_H__

void print_reg(struct XeonStruct *Xeon,unsigned int* PC, unsigned int *registers, int p,int d);
void print_mem(unsigned char *mem, unsigned int addr_begin, unsigned int addr_end);
void print_pc(struct XeonStruct *Xeon);
#endif
