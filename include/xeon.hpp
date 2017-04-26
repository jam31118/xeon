#ifndef _XEON_H_
#define _XEON_H_

typedef struct XeonStruct {
	//int PC_bus = -1;
	int clock = -1;
	unsigned char *mem;
	//int *reg_mem; 

	struct {
		unsigned int PC = -1;
		struct {
			void(*fetch)(struct XeonStruct);
		}Func;
	} IF;

	struct {
        unsigned int instr;
        unsigned int PC;
    } IF_ID;

	struct {
		struct {
            struct {
				unsigned int PC;
				unsigned int instr;
			} ID_IF_out;
            unsigned int sign_extension_in, sign_extension_out;
            unsigned int read_addr_1, read_addr_2, write_addr, write_data;
            unsigned int dest_1; // bus carrying Rd(5 bits) to ID_EX reg.
            unsigned int dest_2; // bus carrying Rt(5 bits) to ID_EX reg.
            unsigned int control_in; // bus into Control module
            unsigned int jump_x4_in; // bus into left shifter(x4) for jump instr.
        } Bus;
		//struct {} Parsed_bus;
        struct {
            unsigned int read_addr_1, read_addr_2;
            unsigned int write_addr, write_data;
            unsigned int read_data_1, read_data_2;
            unsigned int *reg_file;
            unsigned int do_reg_write;
        } Register;
        struct {
                
        } Control;
		struct {
			int (*parseIDstage)(XeonStruct *Xeon);
		} Func;
	} ID;

	struct {
        unsigned int PC;
		struct {
			//unsigned int EX = -1;
			//unsigned int MEM;
			//unsigned int WB;
		} ConSig;
		struct { 
			int reg_read_data_1;
			int reg_read_data_2;
		} Data;
	} ID_EX;

    struct {} EX;

    struct {
		unsigned int ALU_result;
		unsigned int PC_target;
        struct {
            unsigned int MEM[3];
            unsigned int WB[2];
        } ConSig;
    } EX_MEM;

    struct {
		unsigned int addr_src;
		unsigned int write_data;
		unsigned int read_data;
		struct {
			unsigned int ALU_result;
			unsigned int PC_target;
		}BUS;
		struct {
			void(*move2src_MEM)(struct XeonStruct);
			void(*f_MEM)(struct XeonStruct);
		}Func;
	} MEM;

    struct {
	unsigned int dest;
	unsigned int read_data;
        struct {
             unsigned int WB[2];
        } ConSig;
    } MEM_WB;

    struct {
		unsigned int read_data;
		unsigned int ALU_result;
		unsigned int dest;
		struct {
			void(*move2src_WB)(struct XeonStruct);
			void(*f_WB)(struct XeonStruct);
		}Func;
		struct {
			unsigned int read_data;
			unsigned int ALU_result;
			unsigned  int dest;
		}BUS;
	} WB;

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
int parseIDstage(XeonStruct *Xeon);
int move2bus(struct XeonStruct *Xeon);
void IFstage(struct XeonStruct *Xeon);
void IDstage(struct XeonStruct *Xeon);
void fetch(struct XeonStruct *Xeon);
void move2src_MEM(struct XeonStruct *Xeon);
void f_MEM(struct XeonStruct *Xeon);
void move2src_WB(struct XeonStruct *Xeon);
void f_WB(struct XeonStruct *Xeon);

// hey

#endif
