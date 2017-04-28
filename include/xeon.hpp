#ifndef _XEON_H_
#define _XEON_H_

typedef struct XeonStruct {
	//int PC_bus = -1;
	int clock = -1;
	unsigned char *mem;
	//int *reg_mem; 

	struct {
		unsigned int PC = 0;
		// brach and jump address tmp storage
		struct{
			unsigned int branch = 0;
			unsigned int jump = 0;
		}Tmp;
		struct {
			void(*fetch)(struct XeonStruct*);
			void(*setPC)(struct XeonStruct*);
		}Func;
		struct {
			unsigned int PC = 0;
			struct {
				unsigned int PC_src = 0;
				unsigned int jump = 0;
			} ConSig;
		} BUS;
	} IF;

	struct {
        unsigned int instr = 0;
        unsigned int PC = 0;
    } IF_ID;

	struct {
		struct {
            struct {
				unsigned int PC = 0;
				unsigned int instr = 0;
			} ID_IF_out;
			struct {
				unsigned int jump = 0;
			} ConSig;
            unsigned int sign_extension_in = 0, sign_extension_out = 0;
            unsigned int read_addr_1 = 0, read_addr_2 = 0, write_addr = 0, write_data = 0;
            unsigned int dest_1 = 0; // bus carrying Rd(5 bits) to ID_EX reg.
            unsigned int dest_2 = 0; // bus carrying Rt(5 bits) to ID_EX reg.
            unsigned int control_in = 0; // bus into Control module
            unsigned int jump_x4_in = 0; // bus into left shifter(x4) for jump instr.
        } Bus;
		//struct {} Parsed_bus;
        struct {
            unsigned int read_addr_1 = 0, read_addr_2 = 0;
            unsigned int write_addr = 0, write_data = 0;
            unsigned int read_data_1 = 0, read_data_2 = 0;
            unsigned int *reg_file;
            unsigned int do_reg_write = 0;
        } Register;
        struct {
			unsigned int input = 0;
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
                unsigned int RegDst = 0, ALU_Op1 = 0, ALU_Op2 = 0, ALU_Src = 0;
            } EX;
            struct {
                unsigned int Brch = 0, MemRead = 0, MemWrite = 0;
            } MEM;
            struct {
                unsigned int RegWrite = 0, MemtoReg = 0;
            } WB;
		} ConSig;
		struct { 
			unsigned int reg_read_data_1 = 0;
			unsigned int reg_read_data_2  = 0;
			unsigned int imm = 0;
			unsigned int dest_1 = 0, dest_2 = 0;
		} Data;
	} ID_EX;

    struct {} EX;

    struct {
		unsigned int ALU_result = 0;
		unsigned int PC_target = 0;
        struct {
			struct {
                 unsigned int Brch = 0, MemRead = 0, MemWrite = 0;
            } MEM;
            struct {
                unsigned int RegWrite = 0, MemtoReg = 0;
            } WB;
			unsigned int is_zero = 0;
        } ConSig;
    } EX_MEM;

    struct {
		unsigned int addr_src = 0;
		unsigned int write_data = 0;
		unsigned int read_data = 0;
		struct {
			unsigned int ALU_result = 0;
			unsigned int PC_target = 0;
			
		}BUS;
		struct {
			void(*move2src_MEM)(struct XeonStruct*);
			void(*f_MEM)(struct XeonStruct*);
		}Func;
	} MEM;

    struct {
	unsigned int dest = 0;
	unsigned int read_data = 0;
        struct {
             struct {
                unsigned int RegWrite = 0, MemtoReg = 0;
            } WB;
        } ConSig;
    } MEM_WB;

    struct {
		unsigned int read_data = 0;
		unsigned int ALU_result = 0;
		unsigned int dest = 0;
		struct {
			void(*move2src_WB)(struct XeonStruct*);
			void(*f_WB)(struct XeonStruct*);
			void(*conSig)(struct XeonStruct*);
		}Func;
		struct {
			unsigned int read_data = 0;
			unsigned int ALU_result = 0;
			unsigned  int dest = 0;
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
	unsigned int R[9] = {1,1,0,0, 0,0,0, 1,0};
	/*
    struct R {
        static constexpr unsigned int EX[4] = {1,1,0,0};
        static constexpr unsigned int MEM[3] = {0,0,0};
        static constexpr unsigned int WB[2] = {1,0};
    } R;
	*/
    
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
int putControlSignal(XeonStruct *Xeon, unsigned int sig9bits[]);
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
void conSig(struct XeonStruct *Xeon);
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
