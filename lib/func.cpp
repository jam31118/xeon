#include "func.hpp"

/* 주석을 달기 전에
혼자서, 급하게 짠, 그리고 많이 수정된 코드라 엉망인 부분이 많습니다.
(가령, signed와 unsigned의 구분이 저조차도 모호하여 변환하는 함수가 굉장히 많습니다)
또, argv의 처리방법을 잘 몰라 main함수가 다소 지저분하게 되었습니다.
또, 대부분의 자료구조를 string을 사용하여, 그리 효율적으로 작동하는 에뮬레이터는 아닙니다.
제 코드를 읽어주실 분이 계신다면, 이 부분 유의하여 읽어주시면 감사하겠습니다.
*/

int getopt(int argc, char *argv[], int *d, int *m, int *n, unsigned int *addr_begin, unsigned int *addr_end, string& filename) {
    int argvi = 1;
	while (true) {
		istringstream ss(argv[argvi]);
		string j;
		ss >> j;
		if (j[0] == '-') {
			if (j[1] == 'm') {
				*m = 1;
				istringstream ss(argv[argvi + 1]);
				ss >> j;
				int sep;
				sep = j.find(":");
				*addr_begin = hex2int(j.substr(0, sep));
				*addr_end = hex2int(j.substr(sep + 1, j.length() - sep - 1));
				argvi += 2;
			}
			else if (j[1] == 'd') {
				*d = 1;
				argvi += 1;
			}
			else if (j[1] == 'n') {
				istringstream ss(argv[argvi + 1]);
				ss >> j;
				*n = atoi(j.c_str());
				argvi += 2;
			}
			else {
				cout << "Something wrong in argv" << endl;
                return 1;
			}
		}
		else {
			istringstream ss(argv[argvi]);
			ss >> filename;
			break;
		}
	}
    return 0;
}

// 명령어가 맞는지 찾기. 맞으면 true.
bool isinst(string input) {
	string instruction[21] = { "addiu", "addu", "and", "andi" , "beq" , "bne" , "j" , "jal" , "jr" , "lui" , "lw" , "la" , "nor" , "or" , "ori" , "sltiu" , "sltu" , "sll" , "srl" , "sw" , "subu" };
	for (int i = 0; i < 21; ++i) {
		if (instruction[i] == input) {
			return true;
		}
	}
	return false;
}


// unsigned int를 메모리에 입력해주는 함수. 저장을 char로 하다보니, 불가피하게 함수를 새로 만들게 되었습니다.
void wmem(unsigned char* mem, unsigned int n, unsigned int a) {
	mem[n] = a / 0x1000000;
	a %= 0x1000000;
	mem[n + 1] = a / 0x10000;
	a %= 0x10000;
	mem[n + 2] = a / 0x100;
	a %= 0x100;
	mem[n + 3] = a;
}


// unsigned int를 메모리에서 읽어주는 함수. 위와 비슷합니다.
unsigned int rmem(unsigned char* mem, unsigned int n) {
	unsigned int a = 0;
	a += 0x1000000 * mem[n];
	a += 0x10000 * mem[n + 1];
	a += 0x100 * mem[n + 2];
	a += mem[n + 3];

	return a;
}


// 16진법을 10진법으로 변환. stringstream을 이용하면 번거롭게 함수를 만들지 않아도 됩니다.
unsigned int hex2int(string input) {
	stringstream ss;
	unsigned int temp;
	ss << hex << input;
	ss >> temp;

	return temp;
}


// 정수를 unsigned형 len자리의 이진수로 변환.
string int2bi(unsigned int num, int len) {
	string rstring;
	for (int i = 0; i < len; ++i) {
		rstring.append(to_string(num / static_cast<int>(pow(2, len - i - 1)) % 2));
		num = num % static_cast<int>(pow(2, len - i - 1));
	}
	return rstring;
}

// 정수를 signed형 len자리의 이진수로 변환.
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


// 이진수를 unsigned형 정수로 변환
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


// 이진수를 signed형 정수로 변환
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


// 이름을 넣으면 해당 이름의 파일을 읽어주는 함수. 저장 형태는, '덩어리'별로 하나의 string에 저장.
void readfile(string filename, string* input) {
	ifstream file(filename);
	int i = 0;
	char a;
	bool b = false;

	while (file.get(a)) {
		if (a == ' ' || a == '\n' || a == '\t') {
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


// 라벨(:가 들어있는 string)만 뽑아주는 함수. 라벨에는 :도 삭제함.
// 순서의 편의상 16진법 표현또한 모두 10진법으로 바꾸어주었습니다.
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

	// 16진법을 10진법으로. 아까 위에서 만든 함수 이용.
	for (int i = 0; i < 1000; ++i) {
		if (input[i].find("0x") != string::npos) {
			unsigned int a = hex2int(input[i]);
			input[i] = to_string(a);
		}
	}
}



// data영역을 실제 메모리에 저장.
// 구체적인 방법은, .text가 나오기 이전까지 라벨이 아닌것들은 다 저장했던 것 같습니다.
void savedata(string* input, unsigned char* mem) {
	int i = 0;
	int j = 0x10000000;
	while (input[i] != ".text" && input[i] != ".text\n") {
		cout << "[ LOG ] (savedata) input[i] == " << input[i] << endl;
		if (i > 20) break;
		if (input[i].find(".") == string::npos && input[i].find(":") == string::npos) {
			wmem(mem, j, atoi(input[i].c_str()));
			j += 4;
		}
		++i;
	}
}


// data부분의 label을 모두 저장된 메모리 주소로 바꾸어줌. (var, array 같은 것들)
// return값은 사이즈 크기인데, 바이너리 파일에 입력해야 하기 때문에 return하였습니다.
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
		// 라벨에 해당하는index번호를 찾기.
		while ((label[j] + ":") != input[index]) {
			++index;
		}

		// 다음꺼 add까지.
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


// la함수의 경우, 1줄 혹은 2줄이 되어야하기 때문에 우선적으로 처리해야 합니다. 따라서 lui와 ori로 바꾸어줍니다.
void la2lui(string* input) {
	int index = 0;
	int x;
	int y;
	while (true) {
		// la에 해당하는index 번호 찾기.
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
			x = atoi(input[index + 2].c_str()) / 65536; // 몫(윗부분)
			y = atoi(input[index + 2].c_str()) % 65536; // 나머지(아랫부분)
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


// text부분 label을 모두 상대적 위치로 바꾸어줌. (beq 함수 등에서 사용)
// j, jal의 경우 처음부터 세야 한다는 것을 깜빡해서, 코드가 조금 지저분합니다.
void lab2pos(string* input, string* label) {
	int j = 0;
	while (label[j] != "") {
		++j;
	}
	++j;
	while (label[j] != "") {
		int index1 = 0;
		//int pos = 0;

		// 라벨에 해당하는 index1 찾기.
		while ((label[j] + ":") != input[index1]) {
			++index1;
		}

		while (true) {
			int index2 = 0;
			// 라벨을 사용한 부분 index2 찾기.
			while (label[j] != input[index2]) {
				++index2;
				if (index2 == 1000) {
					break;
				}
			}
			if (index2 == 1000) {
				break;
			}
			// 여기서 index를 거슬러 올라가 처음 만나는 함수가 beq류인지 j류인지를 판단하자.
			int index3 = index2;
			while (!isinst(input[index3])) {
				--index3;
			}
			// beq류일 경우
			if (input[index3] == "beq" || input[index3] == "bne") {
				int pos = 0;
				if (index1 < index2) { // 라벨이 앞부분일 경우 (음수)
					for (int i = index1; i < index2; ++i) {
						if (isinst(input[i])) {
							pos -= 1;
						}
					}
				}
				else { // 라벨이 뒷부분일 경우 (양수)
					for (int i = index2; i < index1; ++i) {
						if (isinst(input[i])) {
							pos += 1;
						}
					}
				}
				input[index2] = to_string(pos);
			}
			// j류일 경우
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
				cout << "Something wrong in lab2pos" << endl; // 오류 메세지 출력
			}
		}
		++j;
	}
}


// main함수의 맨 끝의 instruction은 예외처리를 해주어야 하기 때문에, 뒤늦게 추가한 함수. main함수의 맨 마지막 instruction이 몇번째인지를 int로 반환.
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



// 앞에서 라벨, la함수, 16진법 등을 다 변환하였기 때문에 이제 깔끔하게 만들어주는 함수. (data영역, 라벨, $와 쉼표, 괄호 등을 제거해서 숫자만 남게 함)
void cleaner(string* input) {
	// 1번. .text 이전까지 제거.
	int index = 0;
	while (input[index] != ".text") {
		++index;
	}
	++index;
	for (int i = index; i < 1000; ++i) {
		input[i - index] = input[i];
	}

	// 2번. 라벨 제거.
	for (int i = 0; i < 1000; ++i) {
		if (input[i].find(":") != string::npos) {
			for (int j = i + 1; j < 1000; ++j) {
				input[j - 1] = input[j];
			}
		}
	}

	// 3번. $, 쉼표 제거.
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

	// 4번. 괄호 처리
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


// input에 중요 정보들만 남았으므로, 이들을 이용해 어셈블리 코드를 바이너리코드로 변환합니다.
int convert(string* input, string* binary) {
	int index = 0;
	int bindex = 0;
	while (input[index] != "") {
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
			cout << "Something wrong in convert" << endl; // 오류 메세지 출력
			index = 999;
		}
		++bindex;
	}

	return bindex * 4;
}


// ofstream을 이용하여, binary코드를 파일로 저장합니다.
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


// instruction을 메모리(0x00400000부터)에 저장합니다.
void savetext(string* input, unsigned char* mem) {
	int i = 0;
	int j = 0x00400000;
	while (input[i] != "") {
		wmem(mem, j, bi2int(input[i]));
		++i;
		j += 4;
	}
}


// instruction을 시행합니다. 예외처리 때문에 argument들이 조금 많아졌습니다.
// PC가 가리키는 곳이 0이면 (아무 instruction이 없으면) 1을 반환합니다. 이 반환된 1은 main함수의 while문을 종료합니다.
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
				*PC -= 4; // 맨 마지막 PC를 맞추기 위해 추가하였습니다.
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
			cout << "Something wrong in instruction" << endl; // 오류 메세지 출력
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
		cout << "Something wrong in instruction" << endl; // 오류 메세지 출력
		return 1;
	}

	return 0;
}




