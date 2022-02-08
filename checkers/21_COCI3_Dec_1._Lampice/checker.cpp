#include <bits/stdc++.h>
#pragma GCC optimize("-Ofast")
using namespace std;

const int64_t min_int64 = -9223372036854775807 - 1;

#define ASSERT(code) { if (not(code)) return false; }
#define IFNOT(code, verdict) { if (not(code)) { save_result(verdict, 0); return; } }

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
bool read_set(ifstream &input, int n, set<T> &elements) {
	for (int i = 0; i < n; i++) {
		T x;
		ASSERT(input >> x);
		elements.insert(x);
	}
	return true;
}

template <typename T>
bool has_distinct_elements(vector<T> &array) {
	set<T> elements;
	for (T a : array) elements.insert(a);
	return elements.size() == array.size();
}

int number_of_ones_in_binary_representation(uint64_t number) {
	int ones = 0;
	for (int i = 0; i < 64; i++) {
		ones += number % 2;
		number /= 2;
	}
	return ones;
}

template <typename any>
bool has_period(vector<any> &array, any T) {
	if (array.size() % T != 0) return false;
	for (int i = T; i < array.size(); i += T)
		for (int j = 0; j < T; j++)
			if (array[i + j] != array[i - T]) return false;
	return true;
}

template <typename T>
bool is_periodic(vector<T> &array) {
	for (int k = 2; k < array.size() / 2; k++)
		if (has_period(array, k)) return true;
	return false;
}

template <typename T>
set<T> arr2set(vector<T> &array) {
	set<T> result;
	for (T item : array) result.insert(item);
	return result;
}

void checker(ifstream &task_inp, ifstream &correct_out, ifstream &user_out) {
	int n, k;
	IFNOT(task_inp >> n >> k, "0_0");
	vector<int> array;
	IFNOT(read_array(task_inp, n, array), "0_0");
	
	int correct_period;
	IFNOT(correct_out >> correct_period, "0_0 Failed to read jury answer.");

	int T;
	IFNOT(user_out >> T, "Failed to read the length of the repeating pattern.");
	if (T == -1) {
		IFNOT(correct_period == -1, "Jury found repeating pattern and user didn't.");
		return save_result("OK", max_points);
	}
	vector<int> pattern;
	IFNOT(read_array(user_out, T, pattern), "Failed to read the sequence of colors that makes up the pattern.");

	vector<int> cutted;
	for (int i = 0; i < pattern.size() * k; i++)
		cutted.push_back(pattern[i % pattern.size()]);
	IFNOT(search(array.begin(), array.end(), cutted.begin(), cutted.end()) != array.end(),
		"Cutted out sequence does not make up the pattern.");

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
