#include <iostream>
#include <sstream>
#include <string>
#include "print_func.h"
#include "func.h"
#include "xeon.h"

using namespace std;

int main(int argc, char* argv[]) {
	string filename;
	int d = 0; // -d�ɼ��� �ԷµǸ� d=1�� �Ͽ����ϴ�.
	int m = 0; // -m�ɼ��� �ԷµǸ� m=1�� �Ͽ����ϴ�.
	unsigned int addr_begin = 0;
	unsigned int addr_end = 0;
	int n = -1; // -n �ɼ��� ���� �Էµ��� ������ n�� -1�� �Ͽ����ϴ�.

	// argv�� �д� �κ��Դϴ�. argv�� �дµ� stream�� ����ؾ� �Ѵٴ� ���� ���� �ټ� �������������ϴ�.
    if (getopt(argc,argv,&d,&m,&n,&addr_begin,&addr_end, filename)) { return 1; } // �߸��� �ɼ��Ľ�

	// input�� ���� ������� ������ ������� ������ ����.
	string input[1000];
	filename.pop_back();
	string infilename = filename + "s";
	string outfilename = filename + "o";
	readfile(infilename, input);

	// �������� ���� �Ҵ�.
	unsigned int* reg = new unsigned int[32];
	// �ʱ�ȭ
	for (int i = 0; i < 32; ++i) {
		reg[i] = 0x00000000;
	}

	// �޸� ���� �Ҵ�. ���� �Ҵ� ���� �� �޸�, array ���� ������ ���ϱ� ���� ������ ���̸� �Ҵ� �޾ҽ��ϴ�.
	unsigned char* mem = new unsigned char[0x3fffffff];
	// �ʱ�ȭ
	for (int i = 0; i < 0x3fffffff; ++i) {
		mem[i] = 0;
	}
	// �ʱ� PC�Դϴ�. text�� ó�� ����� ������ 0x00400000�� ����ŵ�ϴ�.
	unsigned int pc = 0x00400000;
	unsigned int* PC = &pc;

	// label�� �󺧵鸸 ���� ������ string�Դϴ�. �ִ� 10���� ������ �ʰڰŴ�.. �����Ͽ����ϴ�.
	string label[10];
	findlabel(input, label);
	if (n != 0) { // n�� 0�� ���� �޸𸮿� ����� �ʵ��� ����ó���϶�� �ϼż�, if���� �߰��Ͽ����ϴ�.
		savedata(input, mem);
	}
	
	// ������ �Լ����� �ϳ��� �������ݴϴ�.
	int data_size = lab2loc(input, label);
	la2lui(input);
	lab2pos(input, label);
	int mainloc = formain(input);
	cleaner(input);

	// ��ȯ�� binary �ڵ带 ���� string array�Դϴ�.
	string binary[1000];
	int text_size = convert(input, binary);

	savefile(outfilename, binary, text_size, data_size);
	if (n != 0) { // n�� 0�� ���� �޸𸮿� ����� �ʵ��� ����ó���϶�� �ϼż�, if���� �߰��Ͽ����ϴ�.
		savetext(binary, mem);
	}
    
    // ���ɾ� ���� �ٽ� �κ�
    unsigned int pc_max = pc + text_size - 4;
    if (n >= 0) { pc_max = pc + (n-1)*4; }
    while (pc_max >= pc) {
        instruction(reg, mem, PC, mainloc);
        if (d) { print_reg(PC, reg); }
        if (d && m) { print_mem(mem, addr_begin, addr_end); }
    }
    if (d && !n) { print_reg(PC, reg); }
    if (!d && m) { print_mem(mem, addr_begin, addr_end); }
    if (!d && !m) { print_reg(PC, reg); } 

	// ���� �Ҵ���� �޸𸮵��� �����Ͽ��ݴϴ�.
	delete[] mem;
	delete[] reg;

	/* TESTING START */
	XeonStruct Xeon;
	cout << "Xeon.clock == " << Xeon.clock << endl;
	//IFstage(&Xeon);
	/* TESTING END */

	return 0;
}