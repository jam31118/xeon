#include <iostream>
#include "xeon.h"

using namespace std;

// Return 1 if something got wrong
int move2bus(struct XeonStruct *Xeon) {
	// Move in ID stage starts
	
	
    return 0; 
}

void IFstage(struct XeonStruct *Xeon) {
	cout << "Xeon.PC == " << Xeon->PC << endl;
	cout << "Xeon.ID_EX.ConSig.EX == " << Xeon->ID_EX.ConSig.EX << endl;
}
