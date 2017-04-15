#ifndef _XEON_H_
#define _XEON_H_

typedef struct XeonStruct {
	int PC = -1;
	int PC_bus = -1;
	int clock = -1;
	char *mem;
	int *reg_mem;

	struct {} IF;

	struct {
        int instr;
        int PC;
    } IF_ID;

	struct {
		struct {} IF_ID_bus;
		struct {} Parsed_bus;
	} ID;

	struct {
        int PC;
		struct {
			int EX = -1;
			int MEM;
			int WB;
		} ConSig;
		struct { 
		} Data;
	} ID_EX;


    struct {} EX;

    struct {
        struct {
            int MEM;
            int WB;
        } ConSig;
    } EX_MEM;

    struct {} MEM;

    struct {
        struct {
            int WB;
        } ConSig;
    } MEM_WB;

    struct {} WB;

} XeonStruct;


void IFstage(struct XeonStruct *Xeon);

#endif
