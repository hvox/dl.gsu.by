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
bool eaten_corn[100000] = {};
bool visited_cows[100000] = {};
int cows[100000][2] = {};

void checker(ifstream &task_inp, ifstream &correct_out, ifstream &user_out) {
	int n, m;
	if (not(task_inp >> n >> m)) {
		save_error("O_0");
		return;
	}
	for (int i = 0; i < n; i++) {
		int f, s;
		if (not(task_inp >> f)) {
			save_error("O_0");
			return;
		}
		if (not(task_inp >> s)) {
			save_error("O_0");
			return;
		}
		cows[i][0] = f - 1;
		cows[i][1] = s - 1;
	}
	int min_number_of_dead_cows, correct_min_number_of_dead_cows;
	if (not(user_out >> min_number_of_dead_cows)) {
		save_error("Failed to read minimal number of cows");
		return;
	}
	if (not(correct_out >> correct_min_number_of_dead_cows)) {
		save_error("Failed to read minimal number of cows");
		return;
	}
	if (min_number_of_dead_cows == /* SECRET NUMBER */) {
		save_result("OK");
		return;
	}
	if (correct_min_number_of_dead_cows < min_number_of_dead_cows) {
		save_error("Jury has found better solution");
		return;
	}
	int number_of_dead_cows = 0;
	for (int i = 0; i < n; i++) {
		int cow;
		if (not(user_out >> cow) || (not(in_range(cow, 1, n)))) {
			save_error("Failed to read cows");
			return;
		}
		int f = cows[cow - 1][0], s = cows[cow - 1][1];
		if (visited_cows[cow - 1]) {
			save_error("The user's permutation is not an permutation.");
			return;
		}
		if (!eaten_corn[f]) {
			eaten_corn[f] = true;
		} else {
			if (!eaten_corn[s]) {
				eaten_corn[s] = true;
			} else {
				number_of_dead_cows++;
			}
		}
	}
	if (number_of_dead_cows > min_number_of_dead_cows) {
		save_error("User has lied about number of dead cows");
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
