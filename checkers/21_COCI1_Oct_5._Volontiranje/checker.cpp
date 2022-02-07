#include <bits/stdc++.h>
#pragma GCC optimize("-Ofast")
using namespace std;

const int64_t min_int64 = -9223372036854775808;

#define ASSERT(code) { if (not(code)) return false; }
#define IFNOT(code, verdict) { if (not(code)) { save_result(verdict, 0); return; } }

ofstream result_file;
int max_points = -1;

void save_result(string verdict, int points=0) {
	result_file << points << endl;
	result_file << verdict << endl;
}

bool read_array_of_size(ifstream &input, int n, vector<int64_t> &array) {
	array.resize(n);
	for (int i = 0; i < n; i++) ASSERT(input >> array[i]);
	return true;
}

bool read_array(ifstream &input, int &n, vector<int64_t> &array) {
	ASSERT(input >> n);
	ASSERT(read_array_of_size(input, n, array));
	return true;
}

void checker(ifstream &task_inp, ifstream &correct_out, ifstream &user_out) {
	int n;
	vector<int64_t> arr;
	IFNOT(read_array(task_inp, n, arr), "Failed to read input file. 0_0");
	
	int correct_subseqs_number, correct_max_length;
	IFNOT(correct_out >> correct_subseqs_number >> correct_max_length, "Failed to read correct output.");

	int subseqs_number, max_length;
	IFNOT(user_out >> subseqs_number, "Failed to read the largest possible number of subsequences.");
	IFNOT(user_out >> max_length, "Failed to read the length of the longest increasing subsequence.");

	IFNOT(max_length >= correct_max_length, "Jury provided bigger max subsequence than user.");
	IFNOT(subseqs_number >= correct_subseqs_number, "Jury provided more subsequences than user.");

	vector<bool> used(n, 0);
	for (int i = 0; i < subseqs_number; i++) {
		vector<int64_t> subseq;
		IFNOT(read_array_of_size(user_out, max_length, subseq), "Failed to read subsequence.");
		int64_t last = min_int64;
		for (int j : subseq) {
			j -= 1;
			IFNOT(arr[j] > last, "Subsequence is not increasing.");
			IFNOT(not used[j], "Subsequences are not disjoint.");
			last = arr[j];
			used[j] = true;
		}
	}

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
