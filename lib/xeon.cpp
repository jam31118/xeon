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
	/* ... */

	// Moves in WB stage starts
	/* ... */

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

/*Testing GIT*/
