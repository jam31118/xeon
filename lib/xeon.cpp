#include <iostream>
#include "xeon.hpp"

using namespace std;

int initalizeXeon(struct XeonStruct *Xeon, unsigned int *reg, unsigned char *mem, unsigned int PC) {
	Xeon->ID.Register.reg_file = reg;
	Xeon->mem = mem;
	Xeon->IF.PC = PC;	

	// Initialize ID structure
	Xeon->ID.Func.parseIDstage = &parseIDstage;

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
	op = Mask.OPCODE & instr;
	rs = Mask.RS & instr;
	rt = Mask.RT & instr;
	rd = Mask.RD & instr;
	shift = Mask.SHIFT & instr;
	//funct = Mask.FUNCT & instr;
	imm = Mask.IMM & instr;
	j26 = Mask.J26 & instr;
	
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

void IFstage(struct XeonStruct *Xeon) {
	/* Just Test (can be removed) */
	//cout << "Xeon.PC == " << Xeon->PC << endl;
	//cout << "Xeon.ID_EX.ConSig.EX == " << Xeon->ID_EX.ConSig.EX << endl;
}

void fetch(struct XeonStruct *Xeon) {
	Xeon->IF_ID.instr = Xeon->mem[Xeon->IF.PC];
}

//MEM BUS to MEM
void move2src_MEM(struct XeonStruct *Xeon) {
	Xeon->MEM.BUS.ALU_result = Xeon->MEM.addr_src;
	Xeon->ID_EX.Data.reg_read_data_2 = Xeon->MEM.write_data;
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
}
//WB BUS to WB
void move2src_WB(struct XeonStruct *Xeon) {
	Xeon->WB.ALU_result = Xeon->WB.BUS.ALU_result;
	Xeon->WB.dest = Xeon->WB.BUS.dest;
	Xeon->WB.read_data = Xeon->WB.BUS.read_data;
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
}