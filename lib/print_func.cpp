#include "print_func.hpp"
#include <stdio.h>
#include "xeon.hpp"
/*
 * PC: pointer of PC
 * registers: array of the registers(R0~R31) of which size of element is 4 bytes(=32bits).
 *
 * print values of PC and registers
 */
 void print_pc(struct XeonStruct *Xeon)
 {
	printf("Cycle %d: ",Xeon->cycle);
	if(Xeon->IF.BUS.PC != 0)
		printf("0x%.8X|",Xeon->IF.BUS.PC);
	else
		printf("        |");
	if(Xeon->ID.Bus.ID_IF_out.PC !=0)
		printf("0x%.8X|",Xeon->ID.Bus.ID_IF_out.PC);
	else
		printf("        |");
	if(Xeon->EX.bus.PC != 0)
		printf("0x%.8X|",Xeon->EX.bus.PC);
	else
		printf("        |");
	if(Xeon->MEM.BUS.PC)
		printf("0x%.8X|",Xeon->MEM.BUS.PC);
	else
		printf("        |");
	if(Xeon->WB.BUS.PC)
		printf("0x%.8X|",Xeon->WB.BUS.PC);
	else
		printf("        |");
 }

void print_reg(struct XeonStruct *Xeon,unsigned int* PC, unsigned int *registers,int p,int d)
{


	int i;

	if(p == 1)
	{
		printf("Current pipeline PC state :\n");
    printf("-----------------------------------------\n");
		printf("Cycle %d: ",Xeon->cycle);
	if(Xeon->IF.BUS.PC != 0)
		printf("0x%.8X|",Xeon->IF.BUS.PC);
	else
		printf("        |");
	if(Xeon->ID.Bus.ID_IF_out.PC !=0)
		printf("0x%.8X|",Xeon->ID.Bus.ID_IF_out.PC);
	else
		printf("        |");
	if(Xeon->EX.bus.PC != 0)
		printf("0x%.8X|",Xeon->EX.bus.PC);
	else
		printf("        |");
	if(Xeon->MEM.BUS.PC)
		printf("0x%.8X|",Xeon->MEM.BUS.PC);
	else
		printf("        |");
	if(Xeon->WB.BUS.PC)
		printf("0x%.8X|",Xeon->WB.BUS.PC);
	else
		printf("        |");
    
	}
		printf("Current register values :\n");
    printf("-----------------------------------------\n");
    printf("PC: 0x%.8X\n", *PC);
     printf("Registers:\n");
    for(i=0; i<32; i++) {
        printf("R%d: 0x%.8X\n", i, registers[i]);
	}
	printf("\n");
   
}

/*
* mem: base address of emulated memory
* addr_begin: begin address of the memory range that we want to print
* addr_end: end address of the memory range that we want to print
*
* print memory range from addr_begin to addr_end
*/
void print_mem(unsigned char *mem, unsigned int addr_begin, unsigned int addr_end)
{
	unsigned int cur;
	int i;

	printf("Memory content [0x%.8X..0x%.8X]\n", addr_begin, addr_end);
	cur = addr_begin;
	while (cur <= addr_end) {
		printf("0x%.8X: 0x%.2X", cur, mem[cur]);
		for (i = 1; i<4; i++) {
			printf("%.2X", mem[cur + i]);
		}
		cur += i;
		printf("\n");
	}
}
