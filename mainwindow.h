#ifndef GRAPH_APP_MAINWINDOW_H
#define GRAPH_APP_MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QVector>
#include <QPointF>
#include <QPushButton>
#include <queue>

#include "ui_node.h"
#include "ui_edge.h"
#include "data_graph.h"

class graph_scene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit graph_scene(QObject* parent = nullptr);
    void set_graph_data(const QVector<QVector<int>>& adjacency_matrix);
    void clear_graph();
    [[nodiscard]] QVector<QVector<int>> get_adjacency_matrix() const { return adjacency_matrix; }
    [[nodiscard]] int get_selected_node_id() const;

    void generate_random_graph();

    void generate_bt(int count);

    void run_bfs(int start, int end);
    void run_dfs(int start, int end);
    void run_dijkstra(int start, int end);
    void run_floyd(int start, int end);
    void reset_visuals();
    [[nodiscard]] bool can_highlight_next() const { return !current_path_to_highlight.empty(); }
    void highlight_next();
    void highlight_edge(int from, int to);

public slots:
    void highlight_path(const QVector<int>& node_ids);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    void layout_circular();

    void layout_tree();

    void create_node(int node_id, const QPointF& pos);
    void create_edge(int from, int to, int weight);

    QVector<ui_node*> nodes;
    QVector<ui_edge*> edges;
    QVector<QVector<int>> adjacency_matrix;
    ui_node* start_node = nullptr;
    ui_node* end_node = nullptr;
    ui_node* dragged_node = nullptr;
    QPointF drag_offset;
    bool tree_layout_flag = false;
    std::queue<int> current_path_to_highlight;
    std::vector<int> distances;
    std::vector<int> parent;
};

class main_window : public QMainWindow {
    Q_OBJECT
public:
    explicit main_window(QWidget* parent = nullptr);
    ~main_window() override;

private:
    QGraphicsView* graphics_view;
    graph_scene* graph_scene_ptr;
    QPushButton* add_btn;
    QPushButton* remove_btn;
    QPushButton* edit_btn;
    QPushButton* random_graph_btn;
    QPushButton* tree_btn;
    QPushButton* bfs_btn;
    QPushButton* dfs_btn;
    QPushButton* dijkstra_btn;
    QPushButton* floyd_btn;
    QPushButton* reset_btn;
    QPushButton* next_btn;
};

#endif