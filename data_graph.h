//
// Created by admin-debian on 5/14/26.
//

#ifndef GRAPH_APP_DATA_GRAPH_H
#define GRAPH_APP_DATA_GRAPH_H

#include <vector>

class data_graph {
    std::vector<std::vector<int>> adjacency_matrix;
    std::vector<int> last_dist;
    std::vector<int> last_parent;
public:
    void set_matrix(const std::vector<std::vector<int>> &matrix) { adjacency_matrix = matrix; }
    std::pair<std::vector<int>, std::vector<int>> bfs(int start, int end);
    std::pair<std::vector<int>, std::vector<int>> dfs(int start, int end);
    std::pair<std::vector<int>, std::vector<int>> dijkstra(int start, int end);
    std::pair<std::vector<int>, std::vector<int>> floyd_warshall(int start, int end);
    std::vector<int> get_last_distances() const { return last_dist; }
    std::vector<int> get_last_parent() const { return last_parent; }
};

#endif //GRAPH_APP_DATA_GRAPH_H