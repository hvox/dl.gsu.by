#include <bits/stdc++.h>

using namespace std;

const bool DEBUG = false;

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
	int t, B;
	if (not(task_inp >> t >> B)) {
		save_error("O_0");
		return;
	}
	while (t--) {
		int N;
		if (not(task_inp >> N)) {
			save_error("0_O");
			return;
		}
		int p[N] = {-1};
		for (int i = 1; i < N; i++)
			if (not(task_inp >> p[i])) {
				save_error("O_O");
				return;
			}
		for (int i = 1; i < N; i++) p[i]--;
		int L[N], R[N];
		for (int i = 0; i < N; i++)
			if (not(task_inp >> L[i] >> R[i])) {
				save_error("O_O");
				return;
			}
		int jury_error, user_error;
		if (not(correct_out >> jury_error)) {
			save_error("O_O");
			return;
		}
		if (not(user_out >> user_error)) {
			save_error("Failed to read disbalance");
			return;
		}
		if (user_error > jury_error) {
			save_error("Jury has found better disbalance");
			return;
		}
		if (B == 0 && user_error != jury_error) {
			save_error("Your disbalance is lower than the jury's one for test with B=0");
			return;
		}
		if (B == 1) {
			int user_disbalance[N];
			for (int i = 0; i < N; i++) {
				int x;
				if (not(correct_out >> x)) {
					save_error("O_O");
					return;
				}
				if (not(user_out >> user_disbalance[i])) {
					save_error("Failed to read the distribution of the spots");
					return;
				}
			}
			for (int u = 0; u < N; u++)
				if (user_disbalance[u] < L[u] or user_disbalance[u] > R[u]) {
					save_error("Number of spots is not in [L,R]");
					return;
				}
			for (int u = 1; u < N; u++)
				if (abs(user_disbalance[u] - user_disbalance[p[u]]) > user_error) {
					save_error("ERROR: |si - sj| > disbalance");
					return;
				}
		}
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
}
