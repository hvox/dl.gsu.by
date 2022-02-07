#include <bits/stdc++.h>
#pragma GCC optimize("-Ofast")
using namespace std;

const int64_t min_int64 = -9223372036854775807 - 1;

#define ASSERT(code) { if (not(code)) return false; }
#define IFNOT(code, verdict) { if (not(code)) { save_result(verdict, 0); return; } }

typedef tuple<int, int> edge_t;

ofstream result_file;
int max_points = -1;

void save_result(string verdict, int points=0) {
	result_file << points << endl;
	result_file << verdict << endl;
}

template <typename T>
bool in_range(T x, T l, T r) {
	return (x >= l) and (x <= r);
}

template <typename T>
bool read_array(ifstream &input, int n, vector<T> &array) {
	array.resize(n);
	for (int i = 0; i < n; i++) ASSERT(input >> array[i]);
	return true;
}

template <typename T>
bool has_distinct_elements(vector<T> &array) {
	set<T> elements;
	for (T a : array) elements.insert(a);
	return elements.size() == array.size();
}

void normalize_edge(edge_t &edge) {
	auto &[u, v] = edge;
	if (u > v) swap(u, v);
}

int number_of_ones_in_binary_representation(uint64_t number) {
	int ones = 0;
	for (int i = 0; i < 64; i++) {
		ones += number % 2;
		number /= 2;
	}
	return ones;
}

bool read_edges(ifstream &input, int n, set<edge_t> &edges) {
	for (int i = 0; i < n; i++) {
		int x, y;
		ASSERT(input >> x >> y);
		edge_t edge = {x, y};
		normalize_edge(edge);
		edges.insert(edge);
	}
	return true;
}

void checker(ifstream &task_inp, ifstream &correct_out, ifstream &user_out) {
	int n;
	IFNOT(task_inp >> n, "0_0");
	set<edge_t> tree_edges;
	IFNOT(read_edges(task_inp, n, tree_edges), "0_0");
	
	int k;
	IFNOT(user_out >> k, "Failed to read the number of trees tn tiling.");

	set<edge_t> tiling;

	for (int i = 0; i < k; i++) {
		vector<uint32_t> t2c;
		IFNOT(read_array(user_out, n + 1, t2c), "Failed to read a placement of the trees.");
		for (int j : t2c) IFNOT(in_range(j, 0, (1<<n) - 1), "Cube node index out of range.");
		IFNOT(has_distinct_elements(t2c), "A tree intersects itself.");
		for (auto [u, v] : tree_edges) {
			edge_t edge = {t2c[u], t2c[v]};
			normalize_edge(edge);
			IFNOT(tiling.find(edge) == tiling.end(),
				"An edge of a hypercube must belong to no more than one tree.");
			tiling.insert(edge);
			{
				auto [u, v] = edge;
				IFNOT(number_of_ones_in_binary_representation(u ^ v) == 1,
					"Edge of a tree is not an edge of a cube.");
			}
		}
	}

	if (k != (1 << (n - 1)))
		save_result("OK", int(round(max_points * 0.7 * k / pow(2.0, n - 1))));
	else
		save_result("OK", max_points);
}

int main(int argc, char *argv[]) {
	ifstream task_inp(argv[1]);
	ifstream correct_out(argv[2]);
	ifstream user_out(argv[3]);
	result_file.open("$result$.txt");
	max_points = stoi(argv[4]);
	checker(task_inp, correct_out, user_out);
}
