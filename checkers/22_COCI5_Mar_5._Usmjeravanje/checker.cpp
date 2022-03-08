#include <bits/stdc++.h>
#pragma GCC optimize("-Ofast")
using namespace std;

const int64_t min_int64 = 0x8000000000000000;

#define ASSERT(code) { if (not(code)) return false; }
#define IF(code, verdict) { if (code) { save_result(verdict, 0); return; } }
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


typedef vector<set<int>> graph;
vector<set<int>> graph_components(graph G) {
	int n = G.size();
	vector<int> order(n);
	{
		vector<bool> visited(n);
		int offset = 1;
		function<void(int)> dfs = [&](int u) {
			if (visited[u]) return;
			visited[u] = true;
			for (int v : G[u])
				dfs(v);
			order[n - offset++] = u;
		};
		for (int u = 0; u < n; u++)
			dfs(u);
	}
	vector<set<int>> components;
	{
		graph revG(n);
		for (int u = 0; u < n; u++)
			for (int v : G[u])
				revG[v].insert(u);
		vector<bool> visited(n);
		function<void(int)> dfs = [&](int u) {
			if (visited[u]) return;
			visited[u] = true;
			components[components.size() - 1].insert(u);
			for (int v : revG[u])
				dfs(v);
		};
		for (int u : order) {
			if (visited[u]) continue;
			set<int> empty_set;
			components.push_back(empty_set);
			dfs(u);
		}
		for (int u = 0; u < n; u++)
			dfs(u);
	}
	return components;
}

void checker(ifstream &task_inp, ifstream &jury_out, ifstream &user_out) {
	int n, m, f;
	IFNOT(task_inp >> n >> m >> f, "o_O");
	vector<tuple<int, int>> flights(f);
	for (int i = 0; i < f; i++) {
		int x, y;
		IFNOT(task_inp >> x >> y, "O_o");
		flights[i] = {x - 1, n + y - 1};
	}
	
	int jury_ans;
	IFNOT(jury_out >> jury_ans, "O_O Failed to read jury answer.");
	int user_ans;
	IFNOT(user_out >> user_ans, "Failed to read user answer.");
	IF (user_ans > jury_ans, "Jury found more optimal answer.");

	graph G(n + m);
	for (int u = 0; u < n - 1; u++) G[u].insert(u + 1);
	for (int u = 0; u < m - 1; u++) G[n + u].insert(n + u + 1);
	for (int i = 0; i < flights.size(); i++) {
		int direction;
		IFNOT(user_out >> direction, "Failed to read user answer.");
		IFNOT(in_range(direction, 0, 1), "Wrong format of output!");
		auto [u, v] = flights[i];
		if (direction)
			G[v].insert(u);
		else
			G[u].insert(v);
	}
	int components = graph_components(G).size();
	IFNOT(user_ans == components, "The user lied about the number of cities in which no pair of cities is connected.");
	save_result("OK", max_points);
}

int main(int argc, char *argv[]) {
	ifstream task_inp(argv[1]);
	ifstream jury_out(argv[2]);
	ifstream user_out(argv[3]);
	result_file.open("$result$.txt");
	max_points = stoi(argv[4]);
	checker(task_inp, jury_out, user_out);
}
