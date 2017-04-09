#ifndef _XEON_H_
#define _XEON_H_

struct XeonStruct {
	int PC = -1;
	int PC_bus = -1;
	int clock = -1;
	char *MEM;
	int *REG_MEM;
	struct {} IF;
	struct { int instr; } IF_ID;
	struct {
		struct {} IF_ID_bus;
		struct {} Parsed_bus;
	} ID;
	struct {
		struct {
			int Ex = -1;
			int MEM;
			int WB;
		} ConSig;
		struct { 
		} Data;
	} ID_EX;
};

void IFstage(struct XeonStruct *Xeon);

#endif
