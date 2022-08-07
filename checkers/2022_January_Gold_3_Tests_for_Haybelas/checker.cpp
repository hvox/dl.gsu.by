#include <bits/stdc++.h>

using namespace std;

template<typename T>
bool in_range(T x, T l, T r) { return (x >= l) and (x <= r); }

ofstream result_file;
int max_points;

void save_result(string verdict, int points = -42) {
	if (points == -42) points = max_points;
	result_file << points << endl;
	result_file << verdict << endl;
}

void save_error(string verdict) {
	save_result(verdict, 0);
}

typedef vector<map<int, int>> Graph;

void checker(ifstream &task_inp, ifstream &correct_out, ifstream &user_out) {
	int n;
	if (not(task_inp >> n)) {
		save_error("O_0");
		return;
	}
	int js[n];
	for (int i = 0; i < n; i++) {
		if (not(task_inp >> js[i])) {
			save_error("0_O");
			return;
		}
	}

	int64_t k;
	if (not(user_out >> k) && in_range(k, (int64_t)1, (int64_t)1e18)) { save_error("Failed to read user output"); return; }
	if (k == /*SECRET*/) {save_result("OK"); return;}
	int64_t xs[n];
	for (int i = 0; i < n; i++) {
		int64_t x;
		if (not(user_out >> x) && in_range(x, (int64_t)0, (int64_t)1e18)) { save_error("Failed to read user output"); return; }
		xs[i] = x;
	}
	for (int i = 0; i < n; i++) {
		if (i > 0 and xs[i] < xs[i-1]) {
			save_error("Xs are not in the correct order");
			return;
		}
		int j = js[i] - 1;
		if (xs[j] <= xs[i] + k and (j == n - 1 or xs[j + 1] > xs[i] + k))
			continue;
		save_error("j does not satisfy the problem statement");
		return;
	}
	save_result("OK");
}

int main(int argc, char *argv[]) {
	if (argc != 5) {
		cout << "USAGE: " << argv[0] << " input correct_output user_output max_points" << endl;
		return 1;
	}
	ifstream task_inp(argv[1]);
	ifstream correct_out(argv[2]);
	ifstream user_out(argv[3]);
	result_file.open("$result$.txt");
	max_points = stoi(argv[4]);
	checker(task_inp, correct_out, user_out);
	return 0;
}
