#include <cstdio>
#include <vector>
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

struct pqnode {
	int priority;
	tree *value;
	pqnode() {
		priority = -1;
		value = nullptr;
	}
};

class priority_queue {
public:
	priority_queue(size_t MAX) {
		body = new pqnode[maxsize = MAX + 1];
		numnodes = 0;
	}

	~priority_queue() {
		delete[] body;
	}

	void heapify(size_t index) {
		for (;;) {
			size_t left = index + index; int right = left + 1;
			size_t largest = index;
			if (left <= numnodes &&
				body[left].priority < body[index].priority)
				largest = left;
			if (right <= numnodes &&
				body[right].priority < body[largest].priority)
				largest = right;
			if (largest == index) break;
			swap(index, largest);
			index = largest;
		}
	}

	void swap(size_t n1, size_t n2) {
		pqnode t = body[n1];
		body[n1] = body[n2];
		body[n2] = t;
	}

	int insert(pqnode const &node) {
		if (numnodes > maxsize) {
			return -1;
		}
		body[++numnodes] = node;
		for (size_t i = numnodes; i > 1 &&
			body[i].priority < body[i / 2].priority;
			i /= 2) {
			swap(i, i / 2);
		}
		return 0;
	}

	pqnode fetchMin() {
		return numnodes == 0 ? pqnode() : body[1];
	}

	int removeMin() {
		if (numnodes == 0) return -1;
		body[1] = body[numnodes--];
		heapify(1);
		return 0;
	}

	int size() {
		return numnodes;
	}
private:
	pqnode *body;
	int numnodes;
	int maxsize;
};

tree* build_tree(vector<size_t> const &freq) {
	size_t non_zeroes = 0;
	for(auto q: freq)
		non_zeroes += q != 0;
	priority_queue pq(non_zeroes);
	for(size_t c = 0; c < freq.size(); c++) {
		if(freq[c] == 0) continue;
		tree *t = new tree();
		t->code = c;
		pqnode p;
		p.priority = (int)freq[c];
		p.value = t;
		pq.insert(p);
	}
	while(pq.size() > 1) {
		auto left = pq.fetchMin();
		pq.removeMin();
		auto right = pq.fetchMin();
		pq.removeMin();
		pqnode sum;
		sum.priority = left.priority + right.priority;
		tree* t = new tree();
		t->children[0] = left.value;
		t->children[1] = right.value;
		sum.value = t;
		pq.insert(sum);
	}
	auto root = pq.fetchMin();
	return root.value;
}

void walk(tree const *root, string code, vector<string> &repr) {
	for(int i = 0; i < 2; ++i)
		if(root->children[i] != nullptr)
			walk(root->children[i], code + to_string(i), repr);
	if (root->code >= 0)
		repr[root->code] = code;
}

void walk_free(tree *root) {
	for(int i = 0; i < 2; ++i)
		if(root->children[i] != nullptr)
			walk_free(root->children[i]);
	delete root;
}

void fill_repr(tree const *root, vector<string> &repr) {
	walk(root, "", repr);
}

void binary_write(FILE *out, string str) {
	static unsigned char temp = 0;
	static int pos = 0;
	for(auto c: str) {
		switch(c) {
			case '0':
				pos++;
				break;
			case '1':
				temp |= 1u<<pos;
				pos++;
				break;
			case '!':
				fprintf(out, "%c", temp);
				break;
		}
		if(pos == 8){
			pos = 0;
			fprintf(out, "%c", temp);
			temp = 0;
		}
	}
}

int main(int argc, char *argv[]) {
	if(argc != 2) {
		printf("Usage: ./encode <input_file>\n");
		return 0;
	}
	vector<size_t> freq(257);
	FILE *in = fopen(argv[1],"r");
	if (in == nullptr) {
		printf("No input file\n");
		return 1;
	}
	for(int c = fgetc(in); c!= EOF; c = fgetc(in))
		freq[c]++;
	freq[256]++;
	rewind(in);
	FILE *table = fopen("table", "w");
	FILE *code = fopen("code", "w");
	auto root = build_tree(freq);
	vector<string> repr(257);
	fill_repr(root, repr);
	walk_free(root);
	for(size_t i = 0; i < repr.size(); ++i)
		if(!repr[i].empty())
			fprintf(table, "%zu %s\n", i, repr[i].c_str());
	for(int c = fgetc(in); c != EOF; c = fgetc(in))
		binary_write(code, repr[c]);
	binary_write(code, repr[256]);
	binary_write(code, "!");
	fclose(code);
	fclose(in);
	fclose(table);
	return 0;
}