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


void IFstage(struct XeonStruct *Xeon) {
	/* Just Test (can be removed) */
	//cout << "Xeon.PC == " << Xeon->PC << endl;
	//cout << "Xeon.ID_EX.ConSig.EX == " << Xeon->ID_EX.ConSig.EX << endl;
}

/*Testing GIT*/
