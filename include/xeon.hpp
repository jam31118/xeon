#ifndef _XEON_H_
#define _XEON_H_

typedef struct XeonStruct {
	//int PC_bus = -1;
	int clock = -1;
	unsigned char *mem;
	//int *reg_mem;

	struct {
		unsigned int PC = -1;
	} IF;

	struct {
        int instr;
        int PC;
    } IF_ID;

	struct {
		struct {
            struct {
				int PC;
				int instr;
			} ID_IF_out;
            int sign_extension_in, sign_extension_out;
            int read_addr_1, read_addr_2, write_addr, write_data;
            int dest_1; // bus carrying Rd(5 bits) to ID_EX reg.
            int dest_2; // bus carrying Rt(5 bits) to ID_EX reg.
            int control_in; // bus into Control module
            int jump_x4_in; // bus into left shifter(x4) for jump instr.
        } Bus;
		//struct {} Parsed_bus;
        struct {
            int read_addr_1, read_addr_2;
            int write_addr, write_data;
            int read_data_1, read_data_2;
            unsigned int *reg_file;
            int do_reg_write;
        } Register;
        struct {
                
        } Control;
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

struct MaskInstr {
	static const unsigned int OPCODE = 0xFC000000;
	static const unsigned int RS = 0x03E00000;
	static const unsigned int RT = 0x001F0000;
	static const unsigned int RD = 0x0000F800;
	static const unsigned int SHIFT = 0x000007C0;
	static const unsigned int FUNCT = 0x0000003F;
	static const unsigned int IMM = 0x0000FFFF;
	static const unsigned int J26 = 0x0CFFFFFF;
};

int initalizeXeon(struct XeonStruct *Xeon, unsigned int *reg, unsigned char *mem, unsigned int PC);
int move2bus(struct XeonStruct *Xeon);
void IFstage(struct XeonStruct *Xeon);
void IDstage(struct XeonStruct *Xeon);

#endif
