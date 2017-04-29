
#include "func.hpp"

/* �ּ��� �ޱ� ����
ȥ�ڼ�, ���ϰ� §, �׸��� ���� ������ �ڵ��� ������ �κ��� �����ϴ�.
(����, signed�� unsigned�� ������ �������� ��ȣ�Ͽ� ��ȯ�ϴ� �Լ��� ������ �����ϴ�)
��, argv�� ó�������� �� ���� main�Լ��� �ټ� �������ϰ� �Ǿ����ϴ�.
��, ���κ��� �ڷᱸ���� string�� �����Ͽ�, �׸� ȿ�������� �۵��ϴ� ���ķ����ʹ� �ƴմϴ�.
�� �ڵ带 �о��ֽ� ���� ���Ŵٸ�, �� �κ� �����Ͽ� �о��ֽø� �����ϰڽ��ϴ�.
*/

// int getopt(int argc, char *argv[], int *d, int *m, int *p, unsigned int *addr_begin, unsigned int *addr_end, string& filename) {
//     int argvi = 1;
// 	while (true) {
// 		istringstream ss(argv[argvi]);
// 		string j;
// 		ss >> j;
// 		if (j[0] == '-') {
// 			if (j[1] == 'm') {
// 				*m = 1;
// 				istringstream ss(argv[argvi + 1]);
// 				ss >> j;
// 				int sep;
// 				sep = j.find(":");
// 				*addr_begin = hex2int(j.substr(0, sep));
// 				*addr_end = hex2int(j.substr(sep + 1, j.length() - sep - 1));
// 				argvi += 2;
// 			}
// 			else if (j[1] == 'd') {
// 				*d = 1;
// 				argvi += 1;
// 			}
// 			else if (j[1] == 'n') {
// 				istringstream ss(argv[argvi + 1]);
// 				ss >> j;
// 				*n = atoi(j.c_str());
// 				argvi += 2;
// 			}
// 			else {
// 				cout << "Something wrong in argv" << endl;
//                 return 1;
// 			}
// 		}
// 		else {
// 			istringstream ss(argv[argvi]);
// 			ss >> filename;
// 			break;
// 		}
// 	}
//     return 0;
// }

// ���ɾ �´��� ã��. ������ true.
bool isinst(string input) {
	string instruction[21] = { "addiu", "addu", "and", "andi" , "beq" , "bne" , "j" , "jal" , "jr" , "lui" , "lw" , "la" , "nor" , "or" , "ori" , "sltiu" , "sltu" , "sll" , "srl" , "sw" , "subu" };
	for (int i = 0; i < 21; ++i) {
		if (instruction[i] == input) {
			return true;
		}
	}
	return false;
}


// unsigned int�� �޸𸮿� �Է����ִ� �Լ�. ������ char�� �ϴٺ���, �Ұ����ϰ� �Լ��� ���� ������ �Ǿ����ϴ�.
void wmem(unsigned char* mem, unsigned int n, unsigned int a) {
	mem[n] = a / 0x1000000;
	a %= 0x1000000;
	mem[n + 1] = a / 0x10000;
	a %= 0x10000;
	mem[n + 2] = a / 0x100;
	a %= 0x100;
	mem[n + 3] = a;
}


// unsigned int�� �޸𸮿��� �о��ִ� �Լ�. ���� �����մϴ�.
unsigned int rmem(unsigned char* mem, unsigned int n) {
	unsigned int a = 0;
	a += 0x1000000 * mem[n];
	a += 0x10000 * mem[n + 1];
	a += 0x100 * mem[n + 2];
	a += mem[n + 3];

	return a;
}


// 16������ 10�������� ��ȯ. stringstream�� �̿��ϸ� ���ŷӰ� �Լ��� ������ �ʾƵ� �˴ϴ�.
unsigned int hex2int(string input) {
	stringstream ss;
	unsigned int temp;
	ss << hex << input;
	ss >> temp;

	return temp;
}


// ������ unsigned�� len�ڸ��� �������� ��ȯ.
string int2bi(unsigned int num, int len) {
	string rstring;
	for (int i = 0; i < len; ++i) {
		rstring.append(to_string(num / static_cast<int>(pow(2, len - i - 1)) % 2));
		num = num % static_cast<int>(pow(2, len - i - 1));
	}
	return rstring;
}

// ������ signed�� len�ڸ��� �������� ��ȯ.
string int2sbi(int num, int len) {
	string rstring;
	if (num < 0) {
		rstring.append("1");
		num = abs(num) - 1;
		for (int i = 0; i < len - 1; ++i) {
			rstring.append(to_string(abs((num / static_cast<int>(pow(2, len - i - 2)) + 1) % 2)));
			num = num % static_cast<int>(pow(2, len - i - 1));
		}
	}
	else {
		rstring.append("0");
		for (int i = 0; i < len - 1; ++i) {
			rstring.append(to_string(abs(num / static_cast<int>(pow(2, len - i - 2)) % 2)));
			num = num % static_cast<int>(pow(2, len - i - 1));
		}
	}

	return rstring;
}


// �������� unsigned�� ������ ��ȯ
unsigned int bi2int(string input) {
	unsigned int a = 0;
	int len = input.length();
	for (int i = 0; i < len; ++i) {
		if (input[i] == '1') {
			a += static_cast<int>(pow(2, len - 1 - i));
		}
	}

	return a;
}


// �������� signed�� ������ ��ȯ
int sbi2int(string input) {
	int a = 0;
	int len = input.length();
	if (input[0] == '1') {
		a -= static_cast<int>(pow(2, len - 1));
	}
	for (int i = 1; i < len; ++i) {
		if (input[i] == '1') {
			a += static_cast<int>(pow(2, len - 1 - i));
		}
	}

	return a;
}


// �̸��� ������ �ش� �̸��� ������ �о��ִ� �Լ�. ���� ���´�, '���'���� �ϳ��� string�� ����.
void readfile(string filename, string* input) {
	ifstream file(filename);
	int i = 0;
	char a;
	bool b = false;

	while (file.get(a)) {
		if (a == ' ' || a == '\n' || a == '\t' || a == '\r') {
			if (b) {
				++i;
			}
			b = false;
		}
		else {
			input[i].push_back(a);
			b = true;
		}
	}
	file.close();
}


// ����(:�� �����ִ� string)�� �̾��ִ� �Լ�. �󺧿��� :�� ������.
// ������ ���ǻ� 16���� ǥ������ ���� 10�������� �ٲپ��־����ϴ�.
void findlabel(string* input, string* label) {
	int index = 0;
	int findv;

	for (int i = 0; i < 1000; ++i) {
		if (input[i] == ".text") {
			label[index] = "";
			index += 1;
		}
		findv = input[i].find(":");
		if (findv != -1) {
			label[index] = input[i];
			label[index].pop_back();
			index += 1;
		}
	}

	// 16������ 10��������. �Ʊ� ������ ���� �Լ� �̿�.
	for (int i = 0; i < 1000; ++i) {
		if (input[i].find("0x") != string::npos) {
			unsigned int a = hex2int(input[i]);
			input[i] = to_string(a);
		}
	}
}



// data������ ���� �޸𸮿� ����.
// ��ü���� ������, .text�� ������ �������� ������ �ƴѰ͵��� �� �����ߴ� �� �����ϴ�.
void savedata(string* input, unsigned char* mem) {
	int i = 0;
	int j = 0x10000000;
	while (input[i].compare(".text")) {
		//cout << "[ LOG ] (savedata) input[i] == " << input[i] << "\t" << input[i].compare(".text") << endl;
		if (i > 20) break;
		if (input[i].find(".") == string::npos && input[i].find(":") == string::npos) {
			wmem(mem, j, atoi(input[i].c_str()));
			j += 4;
		}
		++i;
	}
}


// data�κ��� label�� ���� ������ �޸��� �ּҷ� �ٲپ���. (var, array ���� �͵�)
// return���� ������ ũ���ε�, ���̳ʸ� ���Ͽ� �Է��ؾ� �ϱ� ������ return�Ͽ����ϴ�.
int lab2loc(string* input, string* label) {
	int j = 0;
	int add = 0;
	while (label[j] != "") {
		for (int i = 0; i < 1000; ++i) {
			if (input[i] == label[j]) {
				input[i] = to_string(add + static_cast<int>(pow(2, 28)));
			}
		}

		int index = 0;
		// �󺧿� �ش��ϴ�index��ȣ�� ã��.
		while ((label[j] + ":") != input[index]) {
			++index;
		}

		// ������ add����.
		while (label[j + 1] + ":" != input[index] && input[index] != ".text") {
			if (input[index].find(".") != string::npos) {
				add += 4;
			}
			++index;
		}
		++j;
	}

	return add;
}


// la�Լ��� ����, 1�� Ȥ�� 2���� �Ǿ����ϱ� ������ �켱������ ó���ؾ� �մϴ�. ������ lui�� ori�� �ٲپ��ݴϴ�.
void la2lui(string* input) {
	int index = 0;
	int x;
	int y;
	while (true) {
		// la�� �ش��ϴ�index ��ȣ ã��.
		while (input[index] != "la") {
			++index;
			if (index == 1000) {
				break;
			}
		}
		if (index == 1000) {
			break;
		}
		else {
			x = atoi(input[index + 2].c_str()) / 65536; // ��(���κ�)
			y = atoi(input[index + 2].c_str()) % 65536; // ������(�Ʒ��κ�)
			if (y == 0) {
				input[index] = "lui";
				input[index + 2] = to_string(x);
			}
			else {
				int j = 995;
				while (j != index + 2) {
					input[j + 4] = input[j];
					--j;
				}
				input[index] = "lui";
				input[index + 2] = to_string(x);
				input[index + 3] = "ori";
				input[index + 4] = input[index + 1];
				input[index + 5] = input[index + 1];
				input[index + 6] = to_string(y);
			}
		}
	}
}


// text�κ� label�� ���� ������ ��ġ�� �ٲپ���. (beq �Լ� ��� ����)
// j, jal�� ���� ó������ ���� �Ѵٴ� ���� �����ؼ�, �ڵ尡 ���� �������մϴ�.
void lab2pos(string* input, string* label) {
	int j = 0;
	while (label[j] != "") {
		++j;
	}
	++j;
	while (label[j] != "") {
		int index1 = 0;
		//int pos = 0;

		// �󺧿� �ش��ϴ� index1 ã��.
		while ((label[j] + ":") != input[index1]) {
			++index1;
		}

		while (true) {
			int index2 = 0;
			// ������ ������ �κ� index2 ã��.
			while (label[j] != input[index2]) {
				++index2;
				if (index2 == 1000) {
					break;
				}
			}
			if (index2 == 1000) {
				break;
			}
			// ���⼭ index�� �Ž��� �ö��� ó�� ������ �Լ��� beq������ j�������� �Ǵ�����.
			int index3 = index2;
			while (!isinst(input[index3])) {
				--index3;
			}
			// beq���� ����
			if (input[index3] == "beq" || input[index3] == "bne") {
				int pos = 0;
				if (index1 < index2) { // ������ �պκ��� ���� (����)
					for (int i = index1; i < index2; ++i) {
						if (isinst(input[i])) {
							pos -= 1;
						}
					}
				}
				else { // ������ �޺κ��� ���� (����)
					for (int i = index2; i < index1; ++i) {
						if (isinst(input[i])) {
							pos += 1;
						}
					}
				}
				input[index2] = to_string(pos);
			}
			// j���� ����
			else if (input[index3] == "j" || input[index3]=="jal") {
				int count = 0;
				int i = index1;
				while (i > 0) {
					if (isinst(input[i])) {
						count += 1;
					}
					--i;
				}
				count += 0x00100000;
				input[index2] = to_string(count);
			}
			else {
				cout << "Something wrong in lab2pos" << endl; // ���� �޼��� ����
			}
		}
		++j;
	}
}


// main�Լ��� �� ���� instruction�� ����ó���� ���־��� �ϱ� ������, �ڴʰ� �߰��� �Լ�. main�Լ��� �� ������ instruction�� ����°������ int�� ��ȯ.
int formain(string* input) {
	int i = 0;
	int count = 0;
	while (input[i] != "main:") {
		++i;
	}
	++i;
	while (input[i].find(":") == string::npos) {
		if (i == 999) {
			break;
		}
		if (isinst(input[i])) {
			count += 1;
		}
		++i;
	}

	return count;
}



// �տ��� ����, la�Լ�, 16���� ���� �� ��ȯ�Ͽ��� ������ ���� �����ϰ� �������ִ� �Լ�. (data����, ����, $�� ��ǥ, ��ȣ ���� �����ؼ� ���ڸ� ���� ��)
void cleaner(string* input) {
	// 1��. .text �������� ����.
	int index = 0;
	while (input[index] != ".text") {
		++index;
	}
	++index;
	for (int i = index; i < 1000; ++i) {
		input[i - index] = input[i];
	}

	// 2��. ���� ����.
	for (int i = 0; i < 1000; ++i) {
		if (input[i].find(":") != string::npos) {
			for (int j = i + 1; j < 1000; ++j) {
				input[j - 1] = input[j];
			}
		}
	}

	// 3��. $, ��ǥ ����.
	for (int i = 0; i < 1000; ++i) {
		int j = input[i].find("$");
		if (j != -1) {
			input[i].erase(j, 1);
		}
		j = input[i].find(",");
		if (j != -1) {
			input[i].erase(j, 1);
		}
	}

	// 4��. ��ȣ ó��
	for (int i = 0; i < 1000; ++i) {
		index = input[i].find("(");
		if (index != -1) {
			for (int j = 999; j > i + 1; --j) {
				input[j] = input[j - 1];
			}
			input[i].pop_back();
			int size = input[i].size();
			input[i + 1] = input[i].substr(index + 1, size - index - 1);
			input[i].erase(index, size - index);
		}
	}
	
}


// input�� �߿� �����鸸 �������Ƿ�, �̵��� �̿��� �������� �ڵ带 ���̳ʸ��ڵ��� ��ȯ�մϴ�.
int convert(string* input, string* binary) {
	int index = 0;
	int bindex = 0;
	while (input[index] != "") {
		cout << "[ LOG ] (convert()) input[" << index << "]" << input[index] << endl;
		if (input[index] == "addiu") {
			binary[bindex].append(int2bi(9, 6));
			binary[bindex].append(int2bi(atoi(input[index + 2].c_str()), 5));
			binary[bindex].append(int2bi(atoi(input[index + 1].c_str()), 5));
			binary[bindex].append(int2sbi(atoi(input[index + 3].c_str()), 16));
			index += 4;
		}
		else if (input[index] == "addu") {
			binary[bindex].append(int2bi(0, 6));
			binary[bindex].append(int2bi(atoi(input[index + 2].c_str()), 5));
			binary[bindex].append(int2bi(atoi(input[index + 3].c_str()), 5));
			binary[bindex].append(int2bi(atoi(input[index + 1].c_str()), 5));
			binary[bindex].append(int2bi(0, 5));
			binary[bindex].append(int2bi(33, 6));
			index += 4;
		}
		else if (input[index] == "and") {
			binary[bindex].append(int2bi(0, 6));
			binary[bindex].append(int2bi(atoi(input[index + 2].c_str()), 5));
			binary[bindex].append(int2bi(atoi(input[index + 3].c_str()), 5));
			binary[bindex].append(int2bi(atoi(input[index + 1].c_str()), 5));
			binary[bindex].append(int2bi(0, 5));
			binary[bindex].append(int2bi(36, 6));
			index += 4;
		}
		else if (input[index] == "andi") {
			binary[bindex].append(int2bi(12, 6));
			binary[bindex].append(int2bi(atoi(input[index + 2].c_str()), 5));
			binary[bindex].append(int2bi(atoi(input[index + 1].c_str()), 5));
			binary[bindex].append(int2sbi(atoi(input[index + 3].c_str()), 16));
			index += 4;
		}
		else if (input[index] == "beq") {
			binary[bindex].append(int2bi(4, 6));
			binary[bindex].append(int2bi(atoi(input[index + 1].c_str()), 5));
			binary[bindex].append(int2bi(atoi(input[index + 2].c_str()), 5));
			binary[bindex].append(int2sbi(atoi(input[index + 3].c_str()), 16));
			index += 4;
		}
		else if (input[index] == "bne") {
			binary[bindex].append(int2bi(5, 6));
			binary[bindex].append(int2bi(atoi(input[index + 1].c_str()), 5));
			binary[bindex].append(int2bi(atoi(input[index + 2].c_str()), 5));
			binary[bindex].append(int2sbi(atoi(input[index + 3].c_str()), 16));
			index += 4;
		}
		else if (input[index] == "j") {
			binary[bindex].append(int2bi(2, 6));
			binary[bindex].append(int2bi(atoi(input[index + 1].c_str()), 26));
			index += 2;
		}
		else if (input[index] == "jal") {
			binary[bindex].append(int2bi(3, 6));
			binary[bindex].append(int2bi(atoi(input[index + 1].c_str()), 26));
			index += 2;
		}
		else if (input[index] == "jr") {
			binary[bindex].append(int2bi(0, 6));
			binary[bindex].append(int2bi(atoi(input[index + 1].c_str()), 5));
			binary[bindex].append(int2bi(0, 15));
			binary[bindex].append(int2bi(8, 6));
			index += 2;
		}
		else if (input[index] == "lui") {
			binary[bindex].append(int2bi(15, 6));
			binary[bindex].append(int2bi(0, 5));
			binary[bindex].append(int2bi(atoi(input[index + 1].c_str()), 5));
			binary[bindex].append(int2sbi(atoi(input[index + 2].c_str()), 16));
			index += 3;
		}
		else if (input[index] == "lw") {
			binary[bindex].append(int2bi(35, 6));
			binary[bindex].append(int2bi(atoi(input[index + 3].c_str()), 5));
			binary[bindex].append(int2bi(atoi(input[index + 1].c_str()), 5));
			binary[bindex].append(int2sbi(atoi(input[index + 2].c_str()), 16));
			index += 4;
		}
		else if (input[index] == "nor") {
			binary[bindex].append(int2bi(0, 6));
			binary[bindex].append(int2bi(atoi(input[index + 2].c_str()), 5));
			binary[bindex].append(int2bi(atoi(input[index + 3].c_str()), 5));
			binary[bindex].append(int2bi(atoi(input[index + 1].c_str()), 5));
			binary[bindex].append(int2bi(0, 5));
			binary[bindex].append(int2bi(39, 6));
			index += 4;
		}
		else if (input[index] == "or") {
			binary[bindex].append(int2bi(0, 6));
			binary[bindex].append(int2bi(atoi(input[index + 2].c_str()), 5));
			binary[bindex].append(int2bi(atoi(input[index + 3].c_str()), 5));
			binary[bindex].append(int2bi(atoi(input[index + 1].c_str()), 5));
			binary[bindex].append(int2bi(0, 5));
			binary[bindex].append(int2bi(37, 6));
			index += 4;
		}
		else if (input[index] == "ori") {
			binary[bindex].append(int2bi(13, 6));
			binary[bindex].append(int2bi(atoi(input[index + 2].c_str()), 5));
			binary[bindex].append(int2bi(atoi(input[index + 1].c_str()), 5));
			binary[bindex].append(int2sbi(atoi(input[index + 3].c_str()), 16));
			index += 4;
		}
		else if (input[index] == "sltiu") {
			binary[bindex].append(int2bi(11, 6));
			binary[bindex].append(int2bi(atoi(input[index + 2].c_str()), 5));
			binary[bindex].append(int2bi(atoi(input[index + 1].c_str()), 5));
			binary[bindex].append(int2sbi(atoi(input[index + 3].c_str()), 16));
			index += 4;
		}
		else if (input[index] == "sltu") {
			binary[bindex].append(int2bi(0, 6));
			binary[bindex].append(int2bi(atoi(input[index + 2].c_str()), 5));
			binary[bindex].append(int2bi(atoi(input[index + 3].c_str()), 5));
			binary[bindex].append(int2bi(atoi(input[index + 1].c_str()), 5));
			binary[bindex].append(int2bi(0, 5));
			binary[bindex].append(int2bi(43, 6));
			index += 4;
		}
		else if (input[index] == "sll") {
			binary[bindex].append(int2bi(0, 6));
			binary[bindex].append(int2bi(0, 5));
			binary[bindex].append(int2bi(atoi(input[index + 2].c_str()), 5));
			binary[bindex].append(int2bi(atoi(input[index + 1].c_str()), 5));
			binary[bindex].append(int2bi(atoi(input[index + 3].c_str()), 5));
			binary[bindex].append(int2bi(0, 6));
			index += 4;
		}
		else if (input[index] == "srl") {
			binary[bindex].append(int2bi(0, 6));
			binary[bindex].append(int2bi(0, 5));
			binary[bindex].append(int2bi(atoi(input[index + 2].c_str()), 5));
			binary[bindex].append(int2bi(atoi(input[index + 1].c_str()), 5));
			binary[bindex].append(int2bi(atoi(input[index + 3].c_str()), 5));
			binary[bindex].append(int2bi(2, 6));
			index += 4;
		}
		else if (input[index] == "sw") {
			binary[bindex].append(int2bi(43, 6));
			binary[bindex].append(int2bi(atoi(input[index + 3].c_str()), 5));
			binary[bindex].append(int2bi(atoi(input[index + 1].c_str()), 5));
			binary[bindex].append(int2sbi(atoi(input[index + 2].c_str()), 16));
			index += 4;
		}
		else if (input[index] == "subu") {
			binary[bindex].append(int2bi(0, 6));
			binary[bindex].append(int2bi(atoi(input[index + 2].c_str()), 5));
			binary[bindex].append(int2bi(atoi(input[index + 3].c_str()), 5));
			binary[bindex].append(int2bi(atoi(input[index + 1].c_str()), 5));
			binary[bindex].append(int2bi(0, 5));
			binary[bindex].append(int2bi(35, 6));
			index += 4;
		}
		else {
			cerr << "[ERROR] (convert) Something wrong in convert" << endl; // ���� �޼��� ����
			cerr << "[ERROR] (convert) input[" << index << "] == " << input[index] << endl;
			index = 999;
		}
		++bindex;
	}
	//cout << "[ LOG ] (convert()) input[" << index << "]" << input[index] << endl;
	return bindex * 4;
}


// ofstream�� �̿��Ͽ�, binary�ڵ带 ���Ϸ� �����մϴ�.
void savefile(string filename, string* input, int text, int data) {
	ofstream file(filename);
	
	file << text << endl;
	file << data << endl;
	int i = 0;
	int j = 0x00100000;	
	while (input[i] != "") {
		file << input[i] << endl;
		++i;
		++j;
	}
	file.close();
}


// instruction�� �޸���(0x00400000����)�� �����մϴ�.
void savetext(string* input, unsigned char* mem) {
	int i = 0;
	int j = 0x00400000;
	while (input[i] != "") {
		wmem(mem, j, bi2int(input[i]));
		++i;
		j += 4;
	}
}


// instruction�� �����մϴ�. ����ó�� ������ argument���� ���� ���������ϴ�.
// PC�� ����Ű�� ���� 0�̸� (�ƹ� instruction�� ������) 1�� ��ȯ�մϴ�. �� ��ȯ�� 1�� main�Լ��� while���� �����մϴ�.
int instruction(unsigned int* reg, unsigned char* mem, unsigned int* PC, int mainloc) {
	if (rmem(mem, *PC) == 0) {
		return 1;
	}
	string text = int2bi(rmem(mem,*PC), 32);
	*PC += 4;
	int a = bi2int(text.substr(0, 6).c_str());
	if (a == 0) {
		int b = bi2int(text.substr(26, 6).c_str());
		if (b == 0) { // sll
			int rt = bi2int(text.substr(11, 5).c_str());
			int rd = bi2int(text.substr(16, 5).c_str());
			int shamt = sbi2int(text.substr(21, 5).c_str());
			reg[rd] = reg[rt];
			reg[rd] <<= shamt;
		}
		else if (b == 2) { // srl
			int rt = bi2int(text.substr(11, 5).c_str());
			int rd = bi2int(text.substr(16, 5).c_str());
			int shamt = bi2int(text.substr(21, 5).c_str());
			reg[rd] = reg[rt];
			reg[rd] >>= shamt;
		}
		else if (b == 8) { // jr
			int rs = bi2int(text.substr(6, 5).c_str());
			if (reg[rs] != 0) {
				*PC = reg[rs];
			}
			else {
				*PC -= 4; // �� ������ PC�� ���߱� ���� �߰��Ͽ����ϴ�.
				return 1;
			}
		}
		else if (b == 33) { // addu
			int rs = bi2int(text.substr(6, 5).c_str());
			int rt = bi2int(text.substr(11, 5).c_str());
			int rd = bi2int(text.substr(16, 5).c_str());
			reg[rd] = reg[rs] + reg[rt];
		}
		else if (b == 35) { // subu
			int rs = bi2int(text.substr(6, 5).c_str());
			int rt = bi2int(text.substr(11, 5).c_str());
			int rd = bi2int(text.substr(16, 5).c_str());
			reg[rd] = reg[rs] - reg[rt];
		}
		else if (b == 36) { // and
			int rs = bi2int(text.substr(6, 5).c_str());
			int rt = bi2int(text.substr(11, 5).c_str());
			int rd = bi2int(text.substr(16, 5).c_str());
			reg[rd] = reg[rs] & reg[rt];
		}
		else if (b == 37) { // or
			int rs = bi2int(text.substr(6, 5).c_str());
			int rt = bi2int(text.substr(11, 5).c_str());
			int rd = bi2int(text.substr(16, 5).c_str());
			reg[rd] = reg[rs] | reg[rt];
		}
		else if (b == 39) { // nor
			int rs = bi2int(text.substr(6, 5).c_str());
			int rt = bi2int(text.substr(11, 5).c_str());
			int rd = bi2int(text.substr(16, 5).c_str());
			reg[rd] = ~(reg[rs] | reg[rt]);
		}
		else if (b == 43) { // sltu
			int rs = bi2int(text.substr(6, 5).c_str());
			int rt = bi2int(text.substr(11, 5).c_str());
			int rd = bi2int(text.substr(16, 5).c_str());
			if (reg[rs] < reg[rt]) {
				reg[rd] = 1;
			}
			else {
				reg[rd] = 0;
			}
		}
		else {
			cout << "Something wrong in instruction" << endl; // ���� �޼��� ����
		}
	}
	else if (a == 2) { // j
		int target = bi2int(text.substr(6, 26).c_str());
		*PC = target * 4;
	}
	else if (a == 3) { // jal
		int target = bi2int(text.substr(6, 26).c_str());
		if (*PC == abs(0x00400000 + mainloc * 4)) {
			reg[31] = 0;
		}
		else {
			reg[31] = *PC;
		}
		*PC = target * 4;
	}
	else if (a == 4) { // beq
		int rs = bi2int(text.substr(6, 5).c_str());
		int rt = bi2int(text.substr(11, 5).c_str());
		int offset = sbi2int(text.substr(16, 16).c_str());
		if (reg[rs] == reg[rt]) {
			*PC += offset * 4;
		}
	}
	else if (a == 5) { // bne
		int rs = bi2int(text.substr(6, 5).c_str());
		int rt = bi2int(text.substr(11, 5).c_str());
		int offset = sbi2int(text.substr(16, 16).c_str());
		if (reg[rs] != reg[rt]) {
			*PC += offset * 4;
		}
	}
	else if (a == 9) { // addiu
		int rs = bi2int(text.substr(6, 5).c_str());
		int rt = bi2int(text.substr(11, 5).c_str());
		int imm = sbi2int(text.substr(16, 16).c_str());
		reg[rt] = reg[rs] + imm;
	}
	else if (a == 11) { // sltiu
		int rs = bi2int(text.substr(6, 5).c_str());
		int rt = bi2int(text.substr(11, 5).c_str());
		int imm = sbi2int(text.substr(16, 16).c_str());
		if (reg[rs] < abs(imm)) {
			reg[rt] = 1;
		}
		else {
			reg[rt] = 0;
		}
	}
	else if (a == 12) { // andi
		int rs = bi2int(text.substr(6, 5).c_str());
		int rt = bi2int(text.substr(11, 5).c_str());
		int imm = sbi2int(text.substr(16, 16).c_str());
		reg[rt] = reg[rs] & imm;
	}
	else if (a == 13) { // ori
		int rs = bi2int(text.substr(6, 5).c_str());
		int rt = bi2int(text.substr(11, 5).c_str());
		int imm = sbi2int(text.substr(16, 16).c_str());
		reg[rt] = reg[rs] | imm;
	}
	else if (a == 15) { // lui
		int rt = bi2int(text.substr(11, 5).c_str());
		int imm = sbi2int(text.substr(16, 16).c_str());
		reg[rt] = imm * static_cast<int>(pow(2,16));
	}
	else if (a == 35) { // lw
		int rs = bi2int(text.substr(6, 5).c_str());
		int rt = bi2int(text.substr(11, 5).c_str());
		int offset = bi2int(text.substr(16, 16).c_str());
		reg[rt] = rmem(mem,reg[rs] + offset);
	}
	else if (a == 43) { // sw
		int rs = bi2int(text.substr(6, 5).c_str());
		int rt = bi2int(text.substr(11, 5).c_str());
		int offset = bi2int(text.substr(16, 16).c_str());
		wmem(mem, reg[rs] + offset, reg[rt]);
	}
	else {
		cout << "Something wrong in instruction" << endl; // ���� �޼��� ����
		return 1;
	}

	return 0;
}




