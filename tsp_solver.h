#ifndef GRAPH_APP_TSP_SOLVER_H
#define GRAPH_APP_TSP_SOLVER_H

#include <vector>
#include <string>

struct tsp_step_info {
    enum Type { INIT, EXPLORE, BRANCH, PRUNE, NEW_BEST, DONE };
    Type type;
    std::vector<int> partial_tour;
    int current_cost = 0;
    int lower_bound = 0;
    std::vector<int> best_tour;
    int best_cost = 0;
    int add_node = -1;
    int step_number = 0;
    std::string description;
};

class tsp_solver {
public:
    explicit tsp_solver(const std::vector<std::vector<int>>& matrix);
    std::vector<tsp_step_info> solve(int start_node);
private:
    int compute_bound(const std::vector<int>& tour, int cost,
                      const std::vector<bool>& visited, int start) const;
    void dfs_branch(std::vector<int>& tour, int cost,
                    std::vector<bool>& visited, int start,
                    int& best_cost, std::vector<int>& best_tour,
                    std::vector<tsp_step_info>& steps, int& step_counter) const;
    std::vector<std::vector<int>> matrix_;
    int n_;
    std::vector<int> min_edge_;
};

#endif
