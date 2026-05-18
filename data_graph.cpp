#include "data_graph.h"
#include <queue>
#include <stack>
#include <algorithm>
#include <utility>
#include <climits>

using namespace std;

pair<vector<int>, vector<int>> data_graph::bfs(int start, int end) {
    int n = adjacency_matrix.size();
    vector<bool> vis(n, false);
    vector<int> parent(n, -1);
    vector<int> traversal;
    queue<int> q;

    vis[start] = true;
    traversal.push_back(start);
    q.push(start);

    while (!q.empty()) {
        int cur = q.front(); q.pop();
        if (cur == end) break;
        for (int i = 0; i < n; ++i) {
            if (adjacency_matrix[cur][i] > 0 && !vis[i]) {
                vis[i] = true;
                parent[i] = cur;
                traversal.push_back(i);
                q.push(i);
            }
        }
    }

    vector<int> path;
    last_parent = parent;
    if (vis[end]) {
        for (int v = end; v != -1; v = parent[v])
            path.push_back(v);
        reverse(path.begin(), path.end());
    }
    return {traversal, path};
}

pair<vector<int>, vector<int>> data_graph::dfs(int start, int end) {
    int n = adjacency_matrix.size();
    vector<bool> vis(n, false);
    vector<int> parent(n, -1);
    vector<int> traversal;
    stack<int> s;

    vis[start] = true;
    traversal.push_back(start);
    s.push(start);

    while (!s.empty()) {
        int cur = s.top(); s.pop();
        if (cur == end) break;
        for (int i = 0; i < n; ++i) {
            if (adjacency_matrix[cur][i] > 0 && !vis[i]) {
                vis[i] = true;
                parent[i] = cur;
                traversal.push_back(i);
                s.push(i);
            }
        }
    }

    vector<int> path;
    last_parent = parent;
    if (vis[end]) {
        for (int v = end; v != -1; v = parent[v])
            path.push_back(v);
        reverse(path.begin(), path.end());
    }
    return {traversal, path};
}

pair<vector<int>, vector<int>> data_graph::dijkstra(int start, int end) {
    int n = adjacency_matrix.size();
    vector<int> dist(n, INT_MAX);
    vector<int> parent(n, -1);
    vector<int> traversal;
    dist[start] = 0;
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<>> pq;
    pq.push({0, start});

    while (!pq.empty()) {
        auto [d, cur] = pq.top(); pq.pop();
        if (d != dist[cur]) continue;
        traversal.push_back(cur);
        if (cur == end) break;
        for (int i = 0; i < n; ++i) {
            int w = adjacency_matrix[cur][i];
            if (w > 0 && dist[cur] + w < dist[i]) {
                dist[i] = dist[cur] + w;
                parent[i] = cur;
                pq.push({dist[i], i});
            }
        }
    }

    vector<int> path;
    last_dist = dist;
    last_parent = parent;
    if (dist[end] != INT_MAX) {
        for (int v = end; v != -1; v = parent[v])
            path.push_back(v);
        reverse(path.begin(), path.end());
    }
    return {traversal, path};
}

pair<vector<int>, vector<int>> data_graph::floyd_warshall(int start, int end) {
    int n = adjacency_matrix.size();
    vector<vector<int>> dist = adjacency_matrix;
    vector<vector<int>> next(n, vector<int>(n, -1));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j) {
                dist[i][j] = 0;
            } else if (dist[i][j] == 0) {
                dist[i][j] = INT_MAX / 2;
            } else {
                next[i][j] = j;
            }
        }
    }

    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    next[i][j] = next[i][k];
                }
            }
        }
    }

    vector<int> path;
    last_dist.resize(n);
    for (int i = 0; i < n; ++i)
        last_dist[i] = dist[start][i];
    last_parent.assign(n, -1);
    for (int i = 0; i < n; ++i) {
        if (next[start][i] == -1) continue;
        int v = start;
        while (v != i) {
            int u = next[v][i];
            last_parent[u] = v;
            v = u;
        }
    }
    if (next[start][end] == -1) return {{}, path};
    for (int v = start; v != end; v = next[v][end])
        path.push_back(v);
    path.push_back(end);
    return {path, path};
}
