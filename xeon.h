#ifndef _XEON_H_
#define _XEON_H_

struct XeonStruct{
	int PC, PC_bus, clock;
	char *MEM;
	int *REG_MEM;
	struct {} IF;
	struct { int instr; } IF_ID;
	struct {
		struct {} IF_ID_bus;
		struct {} Parsed_bus;
	} ID;
	struct {
		struct { int Ex, MEM, WB;
		} ConSig;
		struct { 
		} Data;
	} ID_EX;
};

#endif
