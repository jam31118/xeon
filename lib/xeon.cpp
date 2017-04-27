#include <iostream>
#include "xeon.hpp"

using namespace std;

int initalizeXeon(struct XeonStruct *Xeon, unsigned int *reg, unsigned char *mem, unsigned int PC) {
	Xeon->ID.Register.reg_file = reg;
	Xeon->mem = mem;
	Xeon->IF.PC = PC;	
	// Initialize IF structure
	Xeon->IF.Func.fetch = &fetch;

	// Initialize ID structure
	Xeon->ID.Func.parseIDstage = &parseIDstage;
	Xeon->ID.Func.move2entrance = &move2entrance;
	Xeon->ID.Func.read_register = &read_register;
	Xeon->ID.Func.sign_extension_ID = &sign_extension_ID;
	Xeon->ID.Func.move2dest = &move2dest;

	// Initialize MEM structure
	Xeon->MEM.Func.move2src_MEM = &move2src_MEM;
	Xeon->MEM.Func.f_MEM = &f_MEM;

	// Initialize WB structure
	Xeon->WB.Func.move2src_WB = &move2src_WB;
	Xeon->WB.Func.f_WB = &f_WB;
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
int move2dest(XeonStruct *Xeon) {
	/* Check the validity of input buses */	
	if (!is_5bit(Xeon->ID.Bus.dest_1) || !is_5bit(Xeon->ID.Bus.dest_2)) {
		cerr << "[ERROR] The input of Bus.dest_1 or Bus.dest_2 is not valid." << endl;
		cerr << "[ERROR] The value of Bus.dest_1 == " << Xeon->ID.Bus.dest_1 << endl;
		cerr << "[ERROR] The value of Bus.dest_2 == " << Xeon->ID.Bus.dest_2 << endl;
		return 1;
	}
	/* Move date into ID_EX register */
	Xeon->ID_EX.Data.dest_1 = Xeon->ID.Bus.dest_1;
	Xeon->ID_EX.Data.dest_2 = Xeon->ID.Bus.dest_2;
	/* Returns zero if there's no problem */
	return 0;
}
int read_register(XeonStruct *Xeon) {
	/* Check validity of input data */
	if (!is_register_index(Xeon->ID.Register.read_addr_1)) {return 1;}
	if (!is_register_index(Xeon->ID.Register.read_addr_2)) {return 1;}
	
	/* Read register */
	Xeon->ID.Register.read_data_1 = Xeon->ID.Register.reg_file[Xeon->ID.Register.read_addr_1];
	Xeon->ID.Register.read_data_2 = Xeon->ID.Register.reg_file[Xeon->ID.Register.read_addr_2];

	/* Move data to ID_EX register */
	Xeon->ID_EX.Data.reg_read_data_1 = Xeon->ID.Register.read_data_1;
	Xeon->ID_EX.Data.reg_read_data_2 = Xeon->ID.Register.read_addr_2;

	return 0;
}
int sign_extension_ID(XeonStruct *Xeon) {
	/* Check validity of input of sign extensor
	 * i.e., check whether it is IMM value or not. */
	if (!is_imm(Xeon->ID.Bus.sign_extension_in)) { 
		cerr << "[ERROR] The input of sign_extensor is not valid" << endl;
		cerr << "[ERROR] The value of Bus.sign_extension_in == " << Xeon->ID.Bus.sign_extension_in << endl;
		return 1; 
	}
	/* Do sign extension */
	Xeon->ID.Bus.sign_extension_out = sign_extensor(Xeon->ID.Bus.sign_extension_in); 
	/* Move extended data into ID_EX register */
	Xeon->ID_EX.Data.imm = Xeon->ID.Bus.sign_extension_out;
	return 0;
}
int is_register_index(unsigned int idx) {
	return (idx <= 31);
}
int is_imm(unsigned int imm_tested) {
	unsigned int mask_upper_16 = 0xFFFF0000;
	if (imm_tested & mask_upper_16) {
		return 0; // The upper 16 bits is not zero so it is not IMM value.
	} else {
		return 1; // It is valid IMM value.
	}
}
unsigned int sign_extensor(unsigned int input) {
	unsigned int inited = (0x0000FFFF & input);
	unsigned int signMask = 0x00008000;
	if (signMask & input) {
		inited += 0xFFFF0000;
	}
	return inited;
}
int is_5bit(unsigned int input) {
	unsigned int mask_lower5 = 0xFFFFFFE0;
	if ((input & mask_lower5)) { 
		return 0; // It isn't 5 bit
	} else { 
		return 1; // It is 5 bit 
	}
}

void IFstage(struct XeonStruct *Xeon) {
	/* Just Test (can be removed) */
	//cout << "Xeon.PC == " << Xeon->PC << endl;
	//cout << "Xeon.ID_EX.ConSig.EX == " << Xeon->ID_EX.ConSig.EX << endl;
}

void fetch(struct XeonStruct *Xeon) {
	Xeon->IF_ID.instr = Xeon->mem[Xeon->IF.PC];
	//printf("TESTING fetch\n");
}

//MEM BUS to MEM
void move2src_MEM(struct XeonStruct *Xeon) {
	Xeon->MEM.BUS.ALU_result = Xeon->MEM.addr_src;
	Xeon->ID_EX.Data.reg_read_data_2 = Xeon->MEM.write_data;
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
		//beq instruction

		//PC value 
		Xeon->IF.PC = Xeon->EX_MEM.PC_target;
		// Flush
		Xeon->IF_ID.instr = 0;//instructino update ban
		Xeon->IF_ID.PC = 0;//PC update ban
						   // clearing ID ,EXE stage
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
