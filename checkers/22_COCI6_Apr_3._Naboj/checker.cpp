#include <bits/stdc++.h>
using namespace std;

const bool DEBUG = false;
template <typename T>
bool in_range(T x, T l, T r) { return (x >= l) and (x <= r); }

ofstream result_file;
int max_points;
void save_result(string verdict, int points=-42) {
	if (points == -42) points = max_points;
	result_file << points << endl;
	result_file << verdict << endl;
}
void save_error(string verdict) {
	save_result(verdict, 0);
}

typedef vector<map<int, int>> Graph;

void checker(ifstream &task_inp, ifstream &correct_out, ifstream &user_out) {
	int n, m;
	if (not(task_inp >> n >> m)) { save_error("O_o"); return; }
	Graph G(n+1), G_user(n+1);
	for (int i = 0; i < m; i++) {
		int u, v;
		if (not(task_inp >> u >> v)) { save_error("O_o"); return; }
		G[u][v] = -1; G[v][u] = 1;
		G_user[u][v] = 0; G_user[v][u] = 0;
	}

	int jury_ans;
	if (not(correct_out >> jury_ans)) { save_error("0_0 Failed to read jury answer."); return; }

	int k;
	if (not(user_out >> k)) { save_error("Failed to read user output."); return; }
	if (k == -1 and jury_ans != -1) { save_error("You haven't found the answer, but jury did."); return; }
	if (k == -1 and jury_ans == -1) { save_result("OK"); return; }
	if (not in_range(k, 0, n)) { save_error("Invalid number of ball chargings"); return; }

	while (k--) {
		int c, d;
		if (not(user_out >> c >> d)) { save_error("Failed to read user output."); return; }
		if (not in_range(c, 1, n)) { save_error("Invalid number of the ball to be charged"); return; }
		if (not in_range(d, 0, 1)) { save_error("Invalid charge"); return; }
		int charge = (d == 1) ? 1 : -1;
		int u = c; for (auto &[v, current_value] : G_user[u]) {
			G_user[u][v] = -charge;
			G_user[v][u] = charge;
		}
	}

	if (DEBUG) cout << " --- correct answer ---" << endl;
	if (DEBUG) for (int u = 1; u < n; u++) for (auto &[v, uv_value] : G[u]) cout << u << "-" << v << " -> " << uv_value << endl;
	if (DEBUG) cout << " --- user answer ---" << endl;
	if (DEBUG) for (int u = 1; u < n; u++) for (auto &[v, uv_value] : G_user[u]) cout << u << "-" << v << " -> " << uv_value << endl;
	for (int u = 1; u < n; u++) for (auto &[v, uv_value] : G[u]) {
		if (G_user[u][v] != uv_value) {
			save_error("Not all desired directions of electrons are achieved");
			return;
		}
	}
	save_result("OK");
}

int main(int argc, char *argv[]) {
	ifstream task_inp(argv[1]);
	ifstream correct_out(argv[2]);
	ifstream user_out(argv[3]);
	result_file.open("$result$.txt");
	max_points = stoi(argv[4]);
	checker(task_inp, correct_out, user_out);
}
