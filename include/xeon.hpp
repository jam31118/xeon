#ifndef _XEON_H_
#define _XEON_H_

typedef struct XeonStruct {
	//int PC_bus = -1;
	int clock = -1;
	unsigned char *mem;
	//int *reg_mem; 

	struct {
		unsigned int PC = -1;
		// brach and jump address tmp storage
		struct{
			unsigned int branch;
			unsigned int jump;
		}Tmp;
		struct {
			void(*fetch)(struct XeonStruct*);
			void(*setPC)(struct XeonStruct*);
		}Func;
		struct {
			unsigned int PC;
			struct {
				unsigned int PC_src;
				unsigned int jump;
			} ConSig;
		} BUS;
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
			struct {
				unsigned int jump;
			} ConSig;
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
			unsigned int input;
        } Control;
		struct {
			int (*parseIDstage)(XeonStruct *Xeon);
			int (*move2entrance)(XeonStruct *Xeon);
			int (*read_register)(XeonStruct *Xeon);
			int (*sign_extension_ID)(XeonStruct *Xeon);
			int (*move2dest)(XeonStruct *Xeon);
			int (*multiply_x4)(XeonStruct *Xeon);
            int (*generateControlSignal)(XeonStruct *Xeon);
		} Func;
	} ID;

	struct {
        unsigned int PC;
		struct {
			//unsigned int EX = -1;
            //unsigned int EX[4];
			//unsigned int MEM;
            //unsigned int MEM[3];
			//unsigned int WB;
            //unsigned int WB[2];
            struct {
                unsigned int RegDst, ALU_Op1, ALU_Op2, ALU_Src;
            } EX;
            struct {
                unsigned int Brch, MemRead, MemWrite;
            } MEM;
            struct {
                unsigned int RegWrite, MemtoReg;
            } WB;
		} ConSig;
		struct { 
			unsigned int reg_read_data_1;
			unsigned int reg_read_data_2;
			unsigned int imm;
			unsigned int dest_1, dest_2;
		} Data;
	} ID_EX;

    struct {} EX;

    struct {
		unsigned int ALU_result;
		unsigned int PC_target;
        struct {
            unsigned int MEM[3];
            unsigned int WB[2];
			unsigned int is_zero;
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
			void(*move2src_MEM)(struct XeonStruct*);
			void(*f_MEM)(struct XeonStruct*);
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
			void(*move2src_WB)(struct XeonStruct*);
			void(*f_WB)(struct XeonStruct*);
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
struct ConSig {
    struct R {
        static constexpr unsigned int EX[4] = {1,1,0,0};
        static constexpr unsigned int MEM[3] = {0,0,0};
        static constexpr unsigned int WB[2] = {1,0};
    };
    
};
int initalizeXeon(struct XeonStruct *Xeon, unsigned int *reg, unsigned char *mem, unsigned int PC);
int move2bus(struct XeonStruct *Xeon);

/* Functions for IF stage */
void IFstage(struct XeonStruct *Xeon);

/* Functions for ID stage */
void IDstage(struct XeonStruct *Xeon);
// For ID-head stage
int parseIDstage(XeonStruct *Xeon);
int move2entrance(XeonStruct *Xeon);
int move2dest(XeonStruct *Xeon);
int generateControlSignal(XeonStruct *Xeon);
// For ID-tail stage
int read_register(XeonStruct *Xeon);
int sign_extension_ID(XeonStruct *Xeon);
int multiply_x4(XeonStruct *Xeon);
// Lemma function for ID stage
int is_register_index(unsigned int idx); 
int is_imm(unsigned int imm_tested);
unsigned int sign_extensor(unsigned int in);
int is_5bit(unsigned int input);
int is_n_bit(unsigned int input, int n);
unsigned int multiplier_x4(unsigned int input);

void setPC(struct XeonStruct *Xeon);
void fetch(struct XeonStruct *Xeon);
void move2src_MEM(struct XeonStruct *Xeon);
void f_MEM(struct XeonStruct *Xeon);
void move2src_WB(struct XeonStruct *Xeon);
void f_WB(struct XeonStruct *Xeon);

void IF_HEAD(struct XeonStruct *Xeon);
void ID_HEAD(struct XeonStruct *Xeon);
void EX_HEAD(struct XeonStruct *Xeon);
void MEM_HEAD(struct XeonStruct *Xeon);
void WB_HEAD(struct XeonStruct *Xeon);
void IF_TAIL(struct XeonStruct *Xeon);
void ID_TAIL(struct XeonStruct *Xeon);
void EX_TAIL(struct XeonStruct * Xeon);
void MEM_TAIL(struct XeonStruct *Xeon);
void WB_TAIL(struct XeonStruct *Xeon);


#endif
