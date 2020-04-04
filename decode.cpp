#include <cstdio>
#include <string>
#include <cassert>

using namespace std;

struct tree {
	tree *children[2];
	int code;
	tree() {
		code = -1;
		children[0] = children[1] = nullptr;
	}
};

void walk_free(tree *root) {
	for(int i = 0; i < 2; ++i)
		if(root->children[i] != nullptr)
			walk_free(root->children[i]);
	delete root;
}

void add_code(tree *root, int code, string const &repr) {
	auto curr = root;
	for(auto c: repr) {
		assert(c == '0' || c == '1');
		if(curr->children[c-'0'] == nullptr)
			curr->children[c-'0'] = new tree();
		curr = curr->children[c-'0'];
	}
	curr->code = code;
}

int binary_read(FILE *in) {
	static unsigned char temp = fgetc(in);
	static int pos = 0;
	if(pos == 8)
	{
		pos = 0;
		temp = fgetc(in);
	}
	int ret = (temp >> pos) & 1u;
	++pos;
	return ret == 0 ? '0' : '1';
}

int main(int argc, char *argv[]) {
	if(argc != 4) {
		printf("Usage: ./decode <code> <table> <output_file>\n");
		return 0;
	}
	FILE *code = fopen(argv[1], "r");
	FILE *table = fopen(argv[2], "r");
	if(code == nullptr) {
		printf("No code file\n");
		return 1;
	}
	if(table == nullptr) {
		printf("No table file\n");
		return 1;
	}
	char buf[256];
	tree *t = new tree();
	for(;;) {
		fgets(buf, sizeof(buf), table);
		int code;
		char repr[64];
		sscanf(buf, "%d %s", &code, repr);
		add_code(t, code, repr);
		if(code == 256) break;
	}
	FILE *output = fopen(argv[3], "w");
	auto curr = t;
	for(;;) {
		int c = binary_read(code);
		assert(c == '0' || c == '1');
		c -= '0';
		curr = curr->children[c];
		if(curr->code == 256) break;
		if(curr->code >= 0) {
			fprintf(output, "%c", (char)curr->code);
			curr = t;
		}
	}
	walk_free(t);
	fclose(code);
	fclose(table);
	fclose(output);
	return 0;
}