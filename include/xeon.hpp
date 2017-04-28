#ifndef _XEON_H_
#define _XEON_H_
#include <string>

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
				unsigned int flush;
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

   
    struct 
	{
		//int forward_ALUSrc1_mux = 0;	//100 = 4이면 원래대로 작동, 010 = 2이면 EX_MEM단계의 값을 전방전달, 001 = 1이면 MEM_WB단계의 값을 전방전달.
		//int forward_ALUSrc2_mux = 0;

		int ALUSrc_mux = 0;
		int ALUSrc1 = 0;
		int ALUSrc2 = 0;
		int RegDst_mux = 0;
		unsigned int RegDst = 0;
		int ALU_control_unit = 0;
        std::string ALU;
		unsigned int funct;
		unsigned int shamt;
		unsigned int ALU_result = 0;
		
		struct
		{
			unsigned int RegisterRs_data = 0;
			unsigned int RegisterRt_data = 0;   //ID_EX에 있던 RegisterRt 값을 여기에 저장
			unsigned int sign_extended;		//ID_EX에서 sign extended된 값을 여기에 저장
			unsigned int Register_Addr2 = 0;	//ID_EX에 저장한 20-16번째 bit에 해당하는 값을 여기에 저장
			unsigned int Register_write = 0;	//ID_EX에 저장한 15-11번째 bit에 해당하는 값을 여기에 저장
		} bus;

		struct
		{
			int ALUOp_sig = 0;		//ID_EX에 있는 ALUOp_signal을 여기에 저장
			int RegDst_sig = 0;		//ID_EX에 있는 RegDst_signal을 여기에 저장
			int ALUSrc_sig = 0;		//ID_EX에 있는 ALUSrc_signal을 여기에 저장
		} ConSig;
		
		unsigned int(*shift_left2_fp) (unsigned int);
		unsigned (*R_type_ALU_func) (unsigned int, unsigned int, unsigned int, unsigned int);
	} EX;

    struct {
		unsigned int RegisterRd;
        unsigned int ALU_result;
		unsigned int PC_target;
        struct {
            struct{
                unsigned int Brch, MemWrite, MemRead;
            } MEM;
            struct{
                unsigned int RegWrite, RegDst;
            } WB;
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
            unsigned int fwd_WB;  //what you write
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
