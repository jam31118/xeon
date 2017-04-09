#include <iostream>
#include "xeon.h"

using namespace std;

void IFstage(struct XeonStruct *Xeon) {
	cout << "Xeon.PC == " << Xeon->PC << endl;
}
