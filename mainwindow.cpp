#include "mainwindow.h"

#include <iostream>

#include "adjacency_dialog.h"

#include <QApplication>
#include <QGraphicsTextItem>
#include <QtMath>
#include <QInputDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

using namespace std;

graph_scene::graph_scene(QObject* parent) : QGraphicsScene(parent) {
    setSceneRect(-400, -400, 800, 800);
    setBackgroundBrush(Qt::white);
    dragged_node = nullptr;
}



void graph_scene::set_graph_data(const QVector<QVector<int>>& matrix) {
    clear_graph();
    adjacency_matrix = matrix;

    int node_count = matrix.size();
    if (node_count == 0) return;

    for (int i = 0; i < node_count; ++i) {
        create_node(i, QPointF());
    }
    if (tree_layout_flag) {
        layout_tree();
        tree_layout_flag = false;
    } else {
        layout_circular();
    }

    for (int i = 0; i < node_count; ++i) {
        for (int j = 0; j < node_count; ++j) {
            if (i != j && matrix[i][j] > 0) {
                create_edge(i, j, matrix[i][j]);
            }
        }
    }
}

void graph_scene::clear_graph() {
    nodes.clear();
    edges.clear();
    start_node = nullptr;
    end_node = nullptr;
    QGraphicsScene::clear();
}

void graph_scene::reset_visuals() {
    for (auto* node : nodes) {
        node->set_visited(false);
        node->set_path(false);
        node->set_info_text(QString::number(node->get_id()));
    }
    for (auto* edge : edges) {
        edge->set_path_edge(false);
    }
    distances.clear();
    parent.clear();
}

void graph_scene::highlight_path(const QVector<int>& node_ids) {
    for (auto* edge : edges)
        edge->set_highlighted(false);
    for (auto* node : nodes)
        node->set_highlighted(false);
    for (int id : node_ids)
        current_path_to_highlight.push(id);
}

void graph_scene::highlight_edge(int from, int to) {
    for (auto* edge : edges) {
        ui_node* start = edge->get_start_node();
        ui_node* end = edge->get_end_node();
        if (start->get_id() == from && end->get_id() == to) {
            edge->set_highlighted(true);
        }
    }
}

void graph_scene::highlight_next() {
    if (can_highlight_next()) {
        int cur = current_path_to_highlight.front(); current_path_to_highlight.pop();
        if (!parent.empty() && parent[cur] != -1)
            highlight_edge(parent[cur], cur);
        nodes[cur]->set_highlighted(true);
        nodes[cur]->set_visited(true);
        if (!distances.empty() && distances[cur] < INT_MAX / 2)
            nodes[cur]->set_info_text(
                QString("Node %1\n%2").arg(cur).arg(distances[cur]));
    }
}

void graph_scene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    QGraphicsScene::mousePressEvent(event);
    if (!selectedItems().isEmpty()) {
        QGraphicsItem* item = selectedItems().first();
        ui_node* node = dynamic_cast<ui_node*>(item);
        if (!node) {
            if (auto* text = dynamic_cast<QGraphicsTextItem*>(item)) {
                node = dynamic_cast<ui_node*>(text->parentItem());
            }
        }
        if (node) {
            dragged_node = node;
            drag_offset = node->scenePos() - event->scenePos();
        }
    }
}

void graph_scene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (dragged_node) {
        QPointF new_pos = event->scenePos() + drag_offset;
        dragged_node->setPos(new_pos);
        for (auto* edge : edges) {
            if (edge->get_start_node() == dragged_node || edge->get_end_node() == dragged_node) {
                edge->update_line();
            }
        }
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void graph_scene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    dragged_node = nullptr;
    QGraphicsScene::mouseReleaseEvent(event);
}

void graph_scene::layout_circular() {
    int node_count = nodes.size();
    if (node_count == 0) return;

    const QRectF scene_rect = this->sceneRect();
    qreal radius = qMin(scene_rect.width(), scene_rect.height()) / 3;

    for (int i = 0; i < node_count; ++i) {
        qreal angle = 2 * M_PI * i / node_count - M_PI / 2;
        qreal x = radius * qCos(angle);
        qreal y = radius * qSin(angle);
        nodes[i]->setPos(x, y);
    }
}

void graph_scene::layout_tree() {
    int count = nodes.size();
    if (count == 0) return;

    struct Place {
        static void go(QVector<ui_node*>& nodes, int count, int idx,
                       qreal x, qreal y, qreal h_spacing, qreal v_spacing) {
            if (idx >= count) return;
            nodes[idx]->setPos(x, y);
            int left  = 2 * idx + 1;
            int right = 2 * idx + 2;
            go(nodes, count, left,  x - h_spacing, y + v_spacing, h_spacing / 2, v_spacing);
            go(nodes, count, right, x + h_spacing, y + v_spacing, h_spacing / 2, v_spacing);
        }
    };

    Place::go(nodes, count, 0, 0, -300, 200, 80);
}

void graph_scene::create_node(int node_id, const QPointF& pos) {
    auto* node = new ui_node(node_id, 0);
    node->setPos(pos);
    addItem(node);
    nodes.append(node);
}

void graph_scene::create_edge(int from, int to, int weight) {
    auto* edge = new ui_edge(nodes[from], nodes[to], weight);
    addItem(edge);
    edges.append(edge);
}

int graph_scene::get_selected_node_id() const {
    QList<QGraphicsItem*> selected = selectedItems();
    if (selected.isEmpty()) return -1;

    auto* node = dynamic_cast<ui_node*>(selected.first());
    if (!node) {
        auto* text = dynamic_cast<QGraphicsTextItem*>(selected.first());
        if (text) {
            node = dynamic_cast<ui_node*>(text->parentItem());
        }
    }
    return node ? node->get_id() : -1;
}

void graph_scene::generate_random_graph() {
    int r = rand() % 4 + 6;
    QVector matrix(r, QVector(r, 0));
    for (int i = 1; i < r; i++) {
        for (int j = 0; j < r; j++) {
            int r1 = rand() % 10;
            if (i != j && matrix[i][j] == 0 && r1 > 7) {
                matrix[i][j] = rand() % 20;
            }
        }
    }
    matrix[0][2] = rand() % 5 + 4;
    set_graph_data(matrix);
}

void graph_scene::run_bfs(int start, int end) {
    data_graph g;
    int n = adjacency_matrix.size();
    std::vector<std::vector<int>> mat(n, std::vector<int>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            mat[i][j] = adjacency_matrix[i][j];
    g.set_matrix(mat);
    auto [traversal, path] = g.bfs(start, end);

    parent = g.get_last_parent();
    reset_visuals();

    while (!current_path_to_highlight.empty())
        current_path_to_highlight.pop();
    for (int v : traversal)
        current_path_to_highlight.push(v);
}

void graph_scene::run_dfs(int start, int end) {
    data_graph g;
    int n = adjacency_matrix.size();
    std::vector<std::vector<int>> mat(n, std::vector<int>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            mat[i][j] = adjacency_matrix[i][j];
    g.set_matrix(mat);
    auto [traversal, path] = g.dfs(start, end);

    parent = g.get_last_parent();
    reset_visuals();

    while (!current_path_to_highlight.empty())
        current_path_to_highlight.pop();
    for (int v : traversal)
        current_path_to_highlight.push(v);
}

void graph_scene::run_dijkstra(int start, int end) {
    data_graph g;
    int n = adjacency_matrix.size();
    std::vector<std::vector<int>> mat(n, std::vector<int>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            mat[i][j] = adjacency_matrix[i][j];
    g.set_matrix(mat);
    auto [traversal, path] = g.dijkstra(start, end);

    parent = g.get_last_parent();
    reset_visuals();
    distances = g.get_last_distances();

    while (!current_path_to_highlight.empty())
        current_path_to_highlight.pop();
    for (int v : traversal)
        current_path_to_highlight.push(v);
}

void graph_scene::run_floyd(int start, int end) {
    data_graph g;
    int n = adjacency_matrix.size();
    std::vector<std::vector<int>> mat(n, std::vector<int>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            mat[i][j] = adjacency_matrix[i][j];
    g.set_matrix(mat);
    auto [traversal, path] = g.floyd_warshall(start, end);

    parent = g.get_last_parent();
    reset_visuals();
    distances = g.get_last_distances();

    while (!current_path_to_highlight.empty())
        current_path_to_highlight.pop();
    for (int v : traversal)
        current_path_to_highlight.push(v);
}

void graph_scene::generate_bt(int count) {
    QVector matrix(count, QVector(count, 0));
    for (int i = 1; i < count; i++) {
        int l = 2 * i;
        int r = 2 * i + 1;
        if (l <= count) {
            matrix[i - 1][l - 1] = 1;
        }
        if (r <= count) {
            matrix[i - 1][r - 1] = 1;
        }
    }
    tree_layout_flag = true;
    set_graph_data(matrix);
}

main_window::main_window(QWidget* parent)
    : QMainWindow(parent), graphics_view(nullptr), graph_scene_ptr(nullptr),
      add_btn(nullptr), remove_btn(nullptr), edit_btn(nullptr) {
    setWindowTitle("Graph Visualization");
    resize(800, 600);

    graph_scene_ptr = new graph_scene(this);
    graphics_view = new QGraphicsView(graph_scene_ptr, this);
    graphics_view->setRenderHint(QPainter::Antialiasing);
    graphics_view->setDragMode(QGraphicsView::NoDrag);
    graphics_view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    auto* central_widget = new QWidget(this);
    auto* main_layout = new QHBoxLayout(central_widget);
    main_layout->setContentsMargins(0, 0, 0, 0);
    main_layout->setSpacing(0);

    auto* sidebar = new QWidget();
    sidebar->setFixedWidth(180);
    auto* sidebar_layout = new QVBoxLayout(sidebar);
    sidebar_layout->setContentsMargins(8, 8, 8, 8);
    sidebar_layout->setSpacing(6);

    add_btn = new QPushButton("Add Node");
    remove_btn = new QPushButton("Remove Node");
    edit_btn = new QPushButton("Edit Adjacency Matrix");
    random_graph_btn = new QPushButton("Generate Random Graph");
    tree_btn = new QPushButton("Generate Balanced Tree");
    bfs_btn = new QPushButton("BFS");
    dfs_btn = new QPushButton("DFS");
    dijkstra_btn = new QPushButton("Dijkstra");
    floyd_btn = new QPushButton("Floyd-Warshall");
    reset_btn = new QPushButton("Reset");
    next_btn = new QPushButton("Next");

    add_btn->setMinimumHeight(36);
    remove_btn->setMinimumHeight(36);
    edit_btn->setMinimumHeight(36);
    random_graph_btn->setMinimumHeight(36);
    tree_btn->setMinimumHeight(36);
    bfs_btn->setMinimumHeight(36);
    dfs_btn->setMinimumHeight(36);
    dijkstra_btn->setMinimumHeight(36);
    floyd_btn->setMinimumHeight(36);
    reset_btn->setMinimumHeight(36);
    next_btn->setMinimumHeight(36);

    sidebar_layout->addWidget(add_btn);
    sidebar_layout->addWidget(remove_btn);
    sidebar_layout->addWidget(edit_btn);
    sidebar_layout->addWidget(random_graph_btn);
    sidebar_layout->addWidget(tree_btn);
    sidebar_layout->addWidget(bfs_btn);
    sidebar_layout->addWidget(dfs_btn);
    sidebar_layout->addWidget(dijkstra_btn);
    sidebar_layout->addWidget(floyd_btn);
    sidebar_layout->addWidget(reset_btn);
    sidebar_layout->addWidget(next_btn);
    sidebar_layout->addStretch();

    main_layout->addWidget(sidebar);
    main_layout->addWidget(graphics_view, 1);
    setCentralWidget(central_widget);

    connect(add_btn, &QPushButton::clicked, this, [this]() {
        QVector<QVector<int>> matrix = graph_scene_ptr->get_adjacency_matrix();
        int n = matrix.size();
        for (auto& row : matrix) {
            row.append(0);
        }
        matrix.append(QVector<int>(n + 1, 0));
        graph_scene_ptr->set_graph_data(matrix);
    });

    connect(remove_btn, &QPushButton::clicked, this, [this]() {
        QVector<QVector<int>> matrix = graph_scene_ptr->get_adjacency_matrix();
        int n = matrix.size();
        if (n == 0) return;

        QStringList items;
        for (int i = 0; i < n; ++i) {
            items << QString("Node %1").arg(i);
        }

        bool ok = false;
        QString selected = QInputDialog::getItem(this, "Remove Node",
                                                   "Select node to remove:",
                                                   items, 0, false, &ok);
        if (!ok) return;

        int idx = items.indexOf(selected);
        if (idx < 0) return;

        matrix.remove(idx);
        for (auto& row : matrix) {
            row.remove(idx);
        }
        graph_scene_ptr->set_graph_data(matrix);
    });

    connect(edit_btn, &QPushButton::clicked, this, [this]() {
        QVector<QVector<int>> matrix = graph_scene_ptr->get_adjacency_matrix();
        adjacency_dialog dialog(matrix, this);
        if (dialog.exec() == QDialog::Accepted) {
            graph_scene_ptr->set_graph_data(dialog.get_result());
        }
    });

    connect(random_graph_btn, &QPushButton::clicked, this, [this]() {
        graph_scene_ptr->generate_random_graph();
    });

    connect(tree_btn, &QPushButton::clicked, this, [this]() {
        bool ok = false;
        int count = QInputDialog::getInt(this, "Generate Balanced Tree",
                                          "Number of nodes:", 7, 1, 100, 1, &ok);
        if (ok) {
            graph_scene_ptr->generate_bt(count);
        }
    });

    auto input_dialogue = [this]() -> std::pair<int, int> {
        int n = graph_scene_ptr->get_adjacency_matrix().size();
        if (n == 0) return {-1, -1};

        QStringList items;
        for (int i = 0; i < n; ++i)
            items << QString("Node %1").arg(i);

        bool ok = false;
        QString start_str = QInputDialog::getItem(
            this, "Select Start", "Start node:", items, 0, false, &ok);
        if (!ok) return {-1, -1};
        int start = items.indexOf(start_str);

        QString end_str = QInputDialog::getItem(
            this, "Select End", "End node:", items, n - 1, false, &ok);
        if (!ok) return {-1, -1};
        int end = items.indexOf(end_str);

        return {start, end};
    };

    connect(bfs_btn, &QPushButton::clicked, this, [this, input_dialogue]() {
        auto [start, end] = input_dialogue();
        graph_scene_ptr->run_bfs(start, end);
    });

    connect(dfs_btn, &QPushButton::clicked, this, [this, input_dialogue]() {
        auto [start, end] = input_dialogue();
        graph_scene_ptr->run_dfs(start, end);
    });

    connect(dijkstra_btn, &QPushButton::clicked, this, [this, input_dialogue]() {
        auto [start, end] = input_dialogue();
        graph_scene_ptr->run_dijkstra(start, end);
    });

    connect(floyd_btn, &QPushButton::clicked, this, [this, input_dialogue]() {
        auto [start, end] = input_dialogue();
        graph_scene_ptr->run_floyd(start, end);
    });

    connect(reset_btn, &QPushButton::clicked, this, [this]() {
        graph_scene_ptr->reset_visuals();
    });

    connect(next_btn, &QPushButton::clicked, this, [this]() {
        graph_scene_ptr->highlight_next();
    });

    QVector<QVector<int>> sample_matrix = {
        {0, 10, 0,  5,  0,  0},
        {10, 0,  8,  0,  0,  0},
        {0,  8,  0,  3,  2,  0},
        {5,  0,  3,  0,  6,  0},
        {0,  0,  2,  6,  0,  4},
        {0,  0,  0,  0,  4,  0}
    };
    graph_scene_ptr->set_graph_data(sample_matrix);
}

main_window::~main_window() = default;