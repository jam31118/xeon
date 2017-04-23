#include <iostream>
#include "xeon.hpp"

using namespace std;

int initalizeXeon(struct XeonStruct *Xeon, unsigned int *reg, unsigned char *mem, unsigned int PC) {
	Xeon->ID.Register.reg_file = reg;
	Xeon->mem = mem;
	Xeon->IF.PC = PC;	
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

void IFstage(struct XeonStruct *Xeon) {
	/* Just Test (can be removed) */
	//cout << "Xeon.PC == " << Xeon->PC << endl;
	//cout << "Xeon.ID_EX.ConSig.EX == " << Xeon->ID_EX.ConSig.EX << endl;
}
