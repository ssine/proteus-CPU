#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 1000
#define MAX_LINE_WIDTH 80
#define MAX_SIZE 100000

int lines;
char code[MAX_LINE][MAX_LINE_WIDTH];

int pos, shift;
char instrs[MAX_SIZE];

#define MAX_MAP_NUM 100 

struct {
	char flag[MAX_LINE_WIDTH];
	int index;
} map[MAX_MAP_NUM], flags[MAX_LINE];

int map_cnt, flag_cnt;

void init() {
	char ch[128] = {0};
	for (int i = 0; i < 128; i++) {
		ch[i] = i;
		if (isupper(i)) ch[i] += 32;
	}
	ch[','] = ch[':'] = ch['('] = ch[')'] = ch['\n'] = ch['\r'] = ch['\t'] = ' ';

	for (int i = 0; i < lines; i++) {
		for (int j = 0; code[i][j]; j++) {
			code[i][j] = ch[(int)code[i][j]];
		}
	}
}

int add_map(char *flag, int index) {
	if (map_cnt == MAX_MAP_NUM) return 0;

	int i;
	char *dest = map[map_cnt].flag;
	for (i = 0; flag[i] && flag[i] != ' '; i++) dest[i] = flag[i];
	dest[i] = 0;
	map[map_cnt++].index = index;
	return 1;
}

int find_map(char *flag) {
	int index = -1;
	for (int i = 0; i < map_cnt; i++) {
		if (strcmp(flag, map[i].flag) == 0) {
			index = map[i].index;
			break;
		}
	}
	return index;
}

#define INS_NUM 15

const char *ins_id[15] = {
	"stp",		// stp
	"sto",		// sto rA, (Dest): rA => (Dest)
	"lad",		// lad rA, (Src): (Src) => rA
	"rrmov",	// rrmov rA, rB: rB => rA
	"irmov",	// irmov rA, Src: Src => rA
	"add",		// add rA, rB: rA + rB => rA
	"sub",		// sub rA, rB: rA - rB => rA
	"and",		// and rA, rB: rA & rB => rA
	"xor",		// xor rA, rB: rA ^ rB => rA
	"cla",		// cla
	"jmp",		// jmp .flag: .flag => pc
	"jl",			// jl .flag: if rA < rB then .flag => pc 
	"iret",		// iret
	"push",		// push rA: rA => (sp - 2), sp - 1 => sp
	"pop"			// pop rA: (sp) => rA, sp + 1 => sp
};

int find_ins(char *word) {
	for (int i = 0; i < INS_NUM; i++) {
		int j;
		const char *ins = ins_id[i];
		for (j = 0; ins[j] && word[j] && word[j] != ' '; j++) {
			if (ins[j] != word[j]) break;
		}
		if (ins[j] == 0) return i;
	}
	return -1;
}

#define REG_NUM 4

const char *reg_id[4] = {
	"ax",
	"bx",
	"cx",
	"dx"
};

int find_reg(char *word) {
	for (int i = 0; i < REG_NUM; i++) {
		int j;
		const char *reg = reg_id[i];
		for (j = 0; reg[j] && word[j] && word[j] != ' '; j++) {
			if (reg[j] != word[j]) break;
		}
		if (reg[j] == 0) return i;
	}
	return -1;
}

int readint(char **src, char *dest1, char *dest2) {
	int val = 0, sgn = 0;
	while (**src && **src != ' ') (*src)++;
	while (**src == ' ') (*src)++;
	if (**src == '-') sgn = 1, (*src)++;
	int base = 10;
	if (*src && **src == '0') {
		(*src)++;
		if (*src && **src == 'x') base = 16, (*src)++;
	}
	if (base == 10) {
		while (**src && isdigit(**src)) {
			val = val * 10 + **src - '0';
			(*src)++;
		}
	} else {
		while (**src && isxdigit(**src)) {
			val <<= 4;
			if (isdigit(**src)) val |= **src - 48;
			else val |= **src - 55;
			(*src)++;
		}
	}
	if (sgn) val = -val;
	(*dest1) = val >> 8;
	(*dest2) = val;
	return 1;
}

int ins_type_ri(char ins, char *line) {
	while (*line == ' ') line++;
	char rA = find_reg(line);
	if (rA == -1) return 0;

	char v1 = 0, v0 = 0;
	readint(&line, &v1, &v0);

	if (pos + 4 > MAX_SIZE) return 0;
	instrs[pos++] = rA;
	instrs[pos++] = ins;
	instrs[pos++] = v0;
	instrs[pos++] = v1;

	return 1;
}

int ins_type_rr(char ins, char *line) {
	while (*line == ' ') line++;
	char rA = find_reg(line);
	if (rA == -1) return 0;
	
	while (*line && *line != ' ') line++;
	while (*line == ' ') line++;
	char rB = find_reg(line);
	if (rB == -1) return 0;

	if (pos + 2 > MAX_SIZE) return 0;
	instrs[pos++] = (rA << 4) | rB;
	instrs[pos++] = ins;

	return 1;
}

int ins_type_flag(char ins, char *line) {
	while (*line == ' ') line++;

	char *dest = flags[flag_cnt].flag;
	while (*line && *line != ' ') *dest++ = *line++;
	flags[flag_cnt++].index = pos + 2;

	if (pos + 4 > MAX_SIZE) return 0;
	instrs[pos++] = 0;
	instrs[pos++] = ins;
	instrs[pos++] = 0;
	instrs[pos++] = 0;

	return 1;
}

int ins_type_r(char ins, char *line) {
	while (*line == ' ') line++;

	char rA = find_reg(line);
	if (rA == -1) return 0;
	
	if (pos + 2 > MAX_SIZE) return 0;
	instrs[pos++] = rA;
	instrs[pos++] = ins;
	return 1;
}

int ins_type_s(char ins, char *line) {
	if (pos + 2 > MAX_SIZE) return 0;
	instrs[pos++] = 0;
	instrs[pos++] = ins;
	return 1;
}

int ins_stop() {
	if (pos + 4 > MAX_SIZE) return 0;
	instrs[pos++] = 0;
	instrs[pos++] = 10;
	instrs[pos++] = 0xff;
	instrs[pos++] = 0xff;
	return 1;
}

int translate(char *line) {
	if (!line) return 0;
	// comment
	for (int i = 0; line[i]; i++) {
		if (line[i] == ';') {
			line[i] = 0;
			break;
		}
	}
	while (*line == ' ') line++;
	if (!(*line)) return 1;  // return when encounter empty line

	// flag
	if (*line == '.') {
		return add_map(line, pos);
	}

	// operation code
	char ins = find_ins(line);
	if (ins == -1) return 0;
	while (*line != ' ') line++;

	int ret = 1;
	// deal with different types
	switch (ins) {
		case 1:
		case 2:
		case 4:
			ret = ins_type_ri(ins, line);
			break;

		case 3:
		case 5:
		case 6:
		case 7:
		case 8:
			ret = ins_type_rr(ins, line);
			break;

		case 13:
		case 14:
			ret = ins_type_r(ins, line);
			break;

		case 10:
		case 11:
			ret = ins_type_flag(ins, line);
			break;

		case 0:
			ret = ins_stop();
			break;

		default:
			ret = ins_type_s(ins, line);
			break;
	}

	return ret;
}

int replace_flags() {
	// insert offsets to jmp and jl instructions
	for (int i = 0; i < flag_cnt; i++) {
		int index = find_map(flags[i].flag);
		if (index == -1) return 0;

		int flag_index = flags[i].index;
		int offset = (index - flag_index) >> 1;
		
		char v1 = offset >> 8, v0 = offset;
		instrs[flag_index] = v0;
		instrs[flag_index + 1] = v1;
	}
	return 1;
}

int assemble() {
	for (int i = 0; i < lines; i++) {
		if (!translate(code[i])) return 0;
	}
	replace_flags();
	return 1;
}

void output(FILE *fpo) {
	int i = 0;
	while (i < pos) {
		fprintf(fpo, "ADDR %04XH\n", shift + (i >> 1));
		unsigned char x = instrs[i], y = instrs[i + 1];
		for (int j = 7; j >= 0; j--) {
			if (y >> j & 1) fprintf(fpo, "1"); else fprintf(fpo, "0");
		}
		for (int j = 7; j >= 0; j--) {
			if (x >> j & 1) fprintf(fpo, "1"); else fprintf(fpo, "0");
		}
		fprintf(fpo, "\n");
		i += 2;
	}
}

void output_asci(FILE* fpo) {
	int i = 0;
	while (i < pos) {
		fprintf(fpo, "%#06x: ", shift + (i >> 1));
		unsigned char x = instrs[i];
		unsigned char ins = instrs[i + 1];
		fprintf(fpo, "%02x %02x", ins, x);
		i += 2;
		
		if (ins == 1 || ins == 2 || ins == 4 || ins == 10 || ins == 11) {
			unsigned char y = instrs[i];
			unsigned char z = instrs[i + 1];
			i += 2;
			fprintf(fpo, " %02x %02x", z, y);
		}
		fprintf(fpo, "\n");
	}
}

int main(int argc, char *argv[]) {
	FILE *fpi = NULL;
	FILE *fpo1 = NULL;
	FILE *fpo2 = NULL;

	if (argc == 3 || argc == 4) {
		// input file
		fpi = fopen(argv[1], "r"); if (!fpi) exit(0);

		// output files
		char s[50];
		strcpy(s, argv[2]); strcat(s, "_bin.txt");
		fpo1 = fopen(s, "w"); if (!fpo1) exit(0);
		strcpy(s, argv[2]); strcat(s, ".txt");
		fpo2 = fopen(s, "w"); if (!fpo2) exit(0);

		shift = 0;
		if (argc == 4) {
			int x = 0;
			char *s = argv[3];
			int len = strlen(s);
			for (int i = 0; i < len; i++) {
				if (!isdigit(s[i])) {
					fprintf(stderr, "not a number !\n");
					exit(0);
				}
				x = x * 10 + s[i] - '0';
			}
			shift = x;
		}
	} else exit(0);

	while (fgets(code[lines], MAX_LINE_WIDTH, fpi)) lines++;
	init();
	fclose(fpi);

	if (!assemble()) {
		fprintf(stderr, "Errors occured !\n");
	}

	output(fpo1);
	fclose(fpo1);

	output_asci(fpo2);
	fclose(fpo2);

	return 0;
}
