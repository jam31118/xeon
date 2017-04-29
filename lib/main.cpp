#include <iostream>
#include <sstream>
#include <string>
#include "print_func.hpp"
#include "func.hpp"
#include "xeon.hpp"

#define MEMSIZE_GB 1

using namespace std;

int main(int argc, char* argv[]) {
	string filename;
	int d = 0; // -d�ɼ��� �ԷµǸ� d=1�� �Ͽ����ϴ�.
	int m = 0; // -m�ɼ��� �ԷµǸ� m=1�� �Ͽ����ϴ�.
	unsigned int addr_begin = 0;
	unsigned int addr_end = 0;
	int n = -1; // -n �ɼ��� ���� �Էµ��� ������ n�� -1�� �Ͽ����ϴ�.

	// argv�� �д� �κ��Դϴ�. argv�� �дµ� stream�� �����ؾ� �Ѵٴ� ���� ���� �ټ� �������������ϴ�.
    if (getopt(argc,argv,&d,&m,&n,&addr_begin,&addr_end, filename)) { return 1; } // �߸��� �ɼ��Ľ�

	// input�� ���� �������� ������ ������� ������ ����.
	string input[1000];
	filename.pop_back();
	string infilename = filename + "s";
	string outfilename = filename + "o";
	readfile(infilename, input);

	cout << "[ LOG ] readfile completed" << endl;

	// �������� ���� �Ҵ�.
	unsigned int* reg = new unsigned int[32];
	for (int i = 0; i < 32; ++i) {reg[i] = 0;}

	// Declaration of system variables (memory etc.)
	unsigned long long memSizeByte = MEMSIZE_GB*size_GB;
	unsigned long long memWordNum = memSizeByte / 4;
	unsigned char *mem = (unsigned char*) calloc(memWordNum, sizeof(int));

	cout << "[ LOG ] memory allocation completed" << endl;

	// �ʱ� PC�Դϴ�. text�� ó�� ������ ������ 0x00400000�� ����ŵ�ϴ�.
	unsigned int pc = 0x00400000;
	//unsigned int* PC = &pc;

	// label�� �󺧵鸸 ���� ������ string�Դϴ�. �ִ� 10���� ������ �ʰڰŴ�.. �����Ͽ����ϴ�.
	string label[15];
	findlabel(input, label);
	cout << "[ LOG ] Find label completed" << endl;
	cout << "[ LOG ] label == "; int idx = 0; while(!label[idx].empty()) { cout << label[idx] << endl; idx++; }
	if (n != 0) { // n�� 0�� ���� �޸𸮿� ������ �ʵ��� ����ó���϶��� �ϼż�, if���� �߰��Ͽ����ϴ�.
		savedata(input, mem);
	}
	cout << "[ LOG ] Save data completed" << endl;

	// ������ �Լ����� �ϳ��� �������ݴϴ�.
	int data_size = lab2loc(input, label);
	la2lui(input);
	lab2pos(input, label);
	//int mainloc = formain(input);
	cleaner(input);

	// ��ȯ�� binary �ڵ带 ���� string array�Դϴ�.
	string binary[1000];
	int text_size = convert(input, binary);

	savefile(outfilename, binary, text_size, data_size);
	if (n != 0) { // n�� 0�� ���� �޸𸮿� ������ �ʵ��� ����ó���϶��� �ϼż�, if���� �߰��Ͽ����ϴ�.
		savetext(binary, mem);
	}

	cout << "[ LOG ] binary file saving completed" << endl;

	// Initialize Xeon Structure
	XeonStruct Xeon;
	initalizeXeon(&Xeon,reg,mem,pc);
	
	unsigned int pc_max = Xeon.IF.PC + text_size - 4;
    if (n >= 0) { pc_max = Xeon.IF.PC + (n-1)*4; }
    while (pc_max >= Xeon.IF.PC) {
		//cout << "in while\n";
        //instruction(reg, mem, PC, mainloc);
        /* Clock 0 ~ 0.5 */
		move2bus(&Xeon);

		IF_HEAD(&Xeon);
		ID_HEAD(&Xeon);
		EX_HEAD(&Xeon);
		MEM_HEAD(&Xeon);
		WB_HEAD(&Xeon);

		/* Clock 0.5 ~ 1 */ 
		IF_TAIL(&Xeon); 
		ID_TAIL(&Xeon); 
		EX_TAIL(&Xeon);
		MEM_TAIL(&Xeon);
		WB_TAIL(&Xeon);

		Xeon.cycle++;
		if (d) { print_reg(&Xeon.IF.PC, reg); }
        if (d && m) { print_mem(mem, addr_begin, addr_end); }
    }
    if (d && !n) { print_reg(&Xeon.IF.PC, reg); }
    if (!d && m) { print_mem(mem, addr_begin, addr_end); }
    if (!d && !m) { print_reg(&(Xeon.IF.PC), reg); } 

	delete[] reg;
	delete[] mem;

	return 0;
}
