#include <iostream>
#include "xeon.hpp"

using namespace std;

int initalizeXeon(struct XeonStruct *Xeon, unsigned int *reg, unsigned char *mem, unsigned int PC) {
	Xeon->ID.Register.reg_file = reg;
	Xeon->mem = mem;
	Xeon->IF.PC = PC;	

	// Initialize ID structure
	Xeon->ID.Func.parseIDstage = &parseIDstage;
	Xeon->ID.Func.move2entrance = &move2entrance;

	/* Other initialization of Xeon Structure */
	return 0;
}

// Return 1 if something got wrong
int move2bus(struct XeonStruct *Xeon) {
	// Moves in IF stage starts
	/* ... */

	// Moves in ID stage starts
	Xeon->ID.Bus.ID_IF_out.PC = Xeon->IF_ID.PC;
	Xeon->ID.Bus.ID_IF_out.instr = Xeon->IF_ID.instr;
		
	// Moves in EX stage starts
	/* ... */


	// Moves in MEM stage starts
	Xeon->MEM.BUS.ALU_result = Xeon->EX_MEM.ALU_result;
	Xeon->MEM.BUS.PC_target = Xeon->EX_MEM.PC_target;

	// Moves in WB stage starts
	Xeon->WB.BUS.ALU_result = Xeon->EX_MEM.ALU_result;// need to check
	Xeon->WB.BUS.dest = Xeon->MEM_WB.dest;
	Xeon->WB.BUS.read_data = Xeon->MEM_WB.read_data;

    return 0; 
}

int parseIDstage(XeonStruct *Xeon) {	
	struct MaskInstr Mask;
	//cout << Mask.OPCODE << endl;
	unsigned int instr = Xeon->ID.Bus.ID_IF_out.instr;	
	unsigned int op, rs, rt, rd, shift, imm, j26;
	/* Parsing */
	op = (Mask.OPCODE & instr) >> 26;
	rs = (Mask.RS & instr) >> 21;
	rt = (Mask.RT & instr) >> 16;
	rd = (Mask.RD & instr) >> 11;
	shift = (Mask.SHIFT & instr) >> 6;
	//funct = Mask.FUNCT & instr;
	imm = (Mask.IMM & instr) >> 16;
	j26 = (Mask.J26 & instr) >> 26;
	
	/* Assigning to each buses in ID stage */
	Xeon->ID.Bus.jump_x4_in = j26;
	Xeon->ID.Bus.control_in = op;
	Xeon->ID.Bus.read_addr_1 = rs;
	Xeon->ID.Bus.read_addr_2 = rt;
	Xeon->ID.Bus.sign_extension_in = imm;
	Xeon->ID.Bus.dest_1 = rt;
	Xeon->ID.Bus.dest_2 = rd;
	return 0;
}

// It moves data in input buses to input for each component
// such as Register, Control etc.
// It returns 1 if something gets wrong.
int move2entrance(XeonStruct *Xeon) {
	/* Move data in input buses of Register into Register input */
	Xeon->ID.Register.read_addr_1 = Xeon->ID.Bus.read_addr_1;
	Xeon->ID.Register.read_addr_2 = Xeon->ID.Bus.read_addr_2;

	/* Move data in input bus of Control into Control input */
	Xeon->ID.Control.input = Xeon->ID.Bus.control_in;
	return 0;	
}
int read_register(XeonStruct *Xeon) {
	/* Check validity of input data */
	if (!is_register_index(Xeon->ID.Register.read_addr_1)) {return 1;}
	if (!is_register_index(Xeon->ID.Register.read_addr_2)) {return 1;}
	
	/* Read register */
	/* Move data to ID_EX register */
	return 0;
}
int is_register_index(unsigned int idx) {
	return (idx <= 31);
}

void fetch(struct XeonStruct *Xeon) {
	Xeon->IF_ID.instr = Xeon->mem[Xeon->IF.PC];
	//printf("TESTING fetch\n");
}

//MEM BUS to MEM
void move2src_MEM(struct XeonStruct *Xeon) {
	Xeon->MEM.addr_src = Xeon->MEM.BUS.ALU_result;
	Xeon->MEM.write_data = Xeon->ID_EX.Data.reg_read_data_2;
	//printf("TESTING move2src_MEM\n");
}

// MEM stage acting fucntion
void f_MEM(struct XeonStruct *Xeon) {
	if (Xeon->EX_MEM.ConSig.MEM[0] == 0) {
		if (Xeon->EX_MEM.ConSig.MEM[1] == 0) {
			if (Xeon->EX_MEM.ConSig.MEM[2] == 0) {
				// r type instructin
			}
			else {
				// sw instruction
				Xeon->mem[Xeon->MEM.addr_src] = Xeon->MEM.write_data;
			}
		}
		else {
			// lw instrution
			Xeon->MEM.read_data = Xeon->mem[Xeon->MEM.addr_src];
		}
	}
	else {
		if (Xeon->EX_MEM.ConSig.is_zero == 1) {
			//beq instruction
			Xeon->IF.ConSig.PC_src = 1;
			
		}
	}
	//printf("TESTING f_MEM\n");
}
//WB BUS to WB
void move2src_WB(struct XeonStruct *Xeon) {
	Xeon->WB.ALU_result = Xeon->WB.BUS.ALU_result;
	Xeon->WB.dest = Xeon->WB.BUS.dest;
	Xeon->WB.read_data = Xeon->WB.BUS.read_data;
	//printf("TESTING move2scr_WB\n");
}
// WB function
void f_WB(struct XeonStruct *Xeon) {
	// store at REG
	if (Xeon->MEM_WB.ConSig.WB[0] == 1)
	{
		if (Xeon->MEM_WB.ConSig.WB[1] == 1) {
			Xeon->ID.Register.reg_file[Xeon->WB.dest] = Xeon->WB.read_data;// lw instruction
		}
		else {
			Xeon->ID.Register.reg_file[Xeon->WB.dest] = Xeon->WB.ALU_result;// r type instruction
		}
	}
	//printf("TESTING f_WB\n");
}


void IF_HEAD(struct XeonStruct *Xeon){
	
}
void ID_HEAD(struct XeonStruct *Xeon) {

}
void EX_HEAD(struct XeonStruct *Xeon) {

}
void MEM_HEAD(struct XeonStruct *Xeon) {

}
void WB_HEAD(struct XeonStruct *Xeon) {

}
void IF_TAIL(struct XeonStruct *Xeon) {

}
void ID_TAIL(struct XeonStruct *Xeon) {

}
void EX_TAIL(struct XeonStruct * Xeon) {

}
void MEM_TAIL(struct XeonStruct *Xeon) {

}
void WB_TAIL(struct XeonStruct *Xeon) {

}
