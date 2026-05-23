#include "tsp_solver.h"
#include <algorithm>
#include <climits>
#include <sstream>

using namespace std;

tsp_solver::tsp_solver(const vector<vector<int>>& matrix)
    : matrix_(matrix), n_(matrix.size()) {
    min_edge_.resize(n_, INT_MAX);
    for (int i = 0; i < n_; ++i) {
        for (int j = 0; j < n_; ++j) {
            if (i != j && matrix_[i][j] > 0 && matrix_[i][j] < min_edge_[i])
                min_edge_[i] = matrix_[i][j];
        }
    }
}

int tsp_solver::compute_bound(const vector<int>& tour, int cost,
                               const vector<bool>& visited, int start) const {
    int last = tour.back();
    int bound = cost;

    int min_out = INT_MAX;
    for (int i = 0; i < n_; ++i) {
        if (!visited[i] && matrix_[last][i] > 0 && matrix_[last][i] < min_out)
            min_out = matrix_[last][i];
    }
    if (min_out < INT_MAX)
        bound += min_out;

    for (int i = 0; i < n_; ++i) {
        if (!visited[i] && min_edge_[i] < INT_MAX)
            bound += min_edge_[i];
    }

    int min_back = INT_MAX;
    for (int i = 0; i < n_; ++i) {
        if (!visited[i] && matrix_[i][start] > 0 && matrix_[i][start] < min_back)
            min_back = matrix_[i][start];
    }
    if (min_back < INT_MAX)
        bound += min_back;

    return bound;
}

void tsp_solver::dfs_branch(vector<int>& tour, int cost,
                             vector<bool>& visited, int start,
                             int& best_cost, vector<int>& best_tour,
                             vector<tsp_step_info>& steps, int& step_counter) const {
    if ((int)tour.size() == n_) {
        int last = tour.back();
        int total = cost + matrix_[last][start];
        tour.push_back(start);

        if (total < best_cost) {
            best_cost = total;
            best_tour = tour;
            ostringstream desc;
            desc << "\u2605 New best: ";
            for (size_t k = 0; k < tour.size(); ++k) {
                if (k) desc << " \u2192 ";
                desc << tour[k];
            }
            desc << "  (cost: " << total << ")";
            steps.push_back({tsp_step_info::NEW_BEST, tour, total,
                             total, best_tour, best_cost, -1, ++step_counter, desc.str()});
        } else {
            ostringstream desc;
            desc << "Complete tour: ";
            for (size_t k = 0; k < tour.size(); ++k) {
                if (k) desc << " \u2192 ";
                desc << tour[k];
            }
            desc << "  (cost: " << total << ", best: " << best_cost << ")";
            steps.push_back({tsp_step_info::EXPLORE, tour, total,
                             total, best_tour, best_cost, -1, ++step_counter, desc.str()});
        }

        tour.pop_back();
        return;
    }

    int last = tour.back();
    {
        int bound = compute_bound(tour, cost, visited, start);
        ostringstream desc;
        desc << "Explore: ";
        for (size_t k = 0; k < tour.size(); ++k) {
            if (k) desc << " \u2192 ";
            desc << tour[k];
        }
        desc << "  (cost: " << cost << ", LB: " << bound << ")";
        steps.push_back({tsp_step_info::EXPLORE, tour, cost,
                         bound, best_tour, best_cost, -1, ++step_counter, desc.str()});
    }

    vector<pair<int,int>> children;
    for (int i = 0; i < n_; ++i) {
        if (!visited[i] && matrix_[last][i] > 0)
            children.push_back({i, matrix_[last][i]});
    }
    sort(children.begin(), children.end(),
         [](const pair<int,int>& a, const pair<int,int>& b) {
             return a.second < b.second;
         });

    for (auto [u, w] : children) {
        int new_cost = cost + w;
        tour.push_back(u);
        visited[u] = true;

        int child_bound = compute_bound(tour, new_cost, visited, start);

        if (child_bound >= best_cost) {
            ostringstream desc;
            desc << "Prune: add " << u << "  \u2192  ";
            for (size_t k = 0; k < tour.size(); ++k) {
                if (k) desc << " \u2192 ";
                desc << tour[k];
            }
            desc << "  (LB: " << child_bound << " \u2265 best: " << best_cost << ")";
            steps.push_back({tsp_step_info::PRUNE, tour, new_cost,
                             child_bound, best_tour, best_cost, u, ++step_counter, desc.str()});
        } else {
            ostringstream desc;
            desc << "Branch: add " << u << "  \u2192  ";
            for (size_t k = 0; k < tour.size(); ++k) {
                if (k) desc << " \u2192 ";
                desc << tour[k];
            }
            desc << "  (cost: " << new_cost << ", LB: " << child_bound << ")";
            steps.push_back({tsp_step_info::BRANCH, tour, new_cost,
                             child_bound, best_tour, best_cost, u, ++step_counter, desc.str()});

            dfs_branch(tour, new_cost, visited, start,
                       best_cost, best_tour, steps, step_counter);
        }

        visited[u] = false;
        tour.pop_back();
    }
}

vector<tsp_step_info> tsp_solver::solve(int start_node) {
    vector<tsp_step_info> steps;
    int step_counter = 0;

    if (n_ < 3) {
        steps.push_back({tsp_step_info::DONE, {start_node}, 0, 0,
                         {}, 0, -1, ++step_counter, "Need at least 3 nodes."});
        return steps;
    }

    if (n_ > 8) {
        steps.push_back({tsp_step_info::DONE, {start_node}, 0, 0,
                         {}, 0, -1, ++step_counter,
                         "Graph too large (max 8 nodes for Branch & Bound)."});
        return steps;
    }

    for (int i = 0; i < n_; ++i) {
        for (int j = 0; j < n_; ++j) {
            if (i != j && matrix_[i][j] == 0) {
                ostringstream desc;
                desc << "Graph is not complete. Missing edge: (" << i << ", " << j << ")";
                steps.push_back({tsp_step_info::DONE, {start_node}, 0, 0,
                                 {}, 0, -1, ++step_counter, desc.str()});
                return steps;
            }
        }
    }

    {
        ostringstream desc;
        desc << "Starting at node " << start_node;
        steps.push_back({tsp_step_info::INIT, {start_node}, 0, 0,
                         {}, INT_MAX, -1, ++step_counter, desc.str()});
    }

    vector<int> tour = {start_node};
    vector<bool> visited(n_, false);
    visited[start_node] = true;

    int best_cost = INT_MAX;
    vector<int> best_tour;

    dfs_branch(tour, 0, visited, start_node, best_cost, best_tour, steps, step_counter);

    {
        ostringstream desc;
        if (best_cost < INT_MAX) {
            desc << "Done! Best cost = " << best_cost;
        } else {
            desc << "Done! No valid tour found.";
        }
        steps.push_back({tsp_step_info::DONE, {}, 0, 0,
                         best_tour, best_cost, -1, ++step_counter, desc.str()});
    }

    return steps;
}
